SynthDrone — Architecture Document
Status: Draft v0.3 — Updated May 2026 Author: Jules (Alta-Ares) Context: Rebuilt from scratch to understand PixGen internals

1. Pourquoi SynthDrone existe
Le pipeline de détection de drones d'Alta-Ares repose sur :
* Un modèle YOLO pour détecter des drones dans des flux caméra LWIR (thermique)
* Un classificateur binaire secondaire pour filtrer les faux positifs
Le problème : les données d'entraînement sont rares, notamment pour des drones sur fond terrain/sol.
PixGen est un outil existant qui génère ces données synthétiques via Unreal Engine 5 + Cesium. Mais son code n'était pas compris internalement — SynthDrone est une réécriture from scratch pour en maîtriser chaque composant.

2. Vue d'ensemble
graph TB
    subgraph UE5["Unreal Engine 5 — SynthScene.umap"]
        CESIUM["CesiumGeoreference\n+ Google Photorealistic 3D Tiles\nTerrain géoréférencé GPS\n(Zaporizhzhia, Ukraine)"]
        DRONE["BP_Drone\nMesh: Geran21/Shahed\nVol: vitesse × DeltaSeconds\n+ oscillation sinusoïdale Z"]
        CAM["BP_Bounding_Box_Camera\nTracking imparfait du drone\nOffset aléatoire Roll/Pitch/Yaw\n± MaxOffsetDegrees"]
        PPV["PostProcessVolume\nInfinite Unbound\nColor Grading Saturation=0\nSimulation IR / LWIR"]
        PLUGIN["Plugin: 2D Bounding Box\nCapture PNG + CSV par frame\nCustom Depth Stencil"]
    end

    subgraph OUTPUT["Output"]
        IMG["output/{id}/images/\n0.png, 1.png, ..."]
        LABELS["output/{id}/labels/\n0.csv, 1.csv, ..."]
    end

    subgraph TRAINING["Pipeline ML"]
        YOLO["YOLO Detector LWIR"]
        CLASSIFIER["Binary Classifier\nfiltre faux positifs"]
    end

    CESIUM --> CAM
    DRONE --> CAM
    PPV --> CAM
    CAM --> PLUGIN
    PLUGIN --> IMG
    PLUGIN --> LABELS
    IMG --> YOLO
    LABELS --> YOLO
    YOLO --> CLASSIFIER

3. Stack technique
Composant	Technologie	Version
Moteur 3D	Unreal Engine	5.5.4
Terrain géoréférencé	Cesium for Unreal	2.17.0
Tuiles 3D	Google Photorealistic 3D Tiles via Cesium Ion	Asset ID 2275207
Mesh drone	Geran21 (Shahed/Geran-2)	—
Capture + labels	2D Bounding Box for Deep Learning Plugin	Plasma-Lab
Simulation IR	PostProcessVolume Color Grading + MI_ThermalGrayscale	AdvancedThermalVision
Scripting batch	Python (Unreal Python API)	À venir
OS	macOS	—
4. Structure du projet
SynthDrone/
├── Content/
│   ├── SynthDrone/
│   │   ├── BP_Drone.uasset
│   │   └── BP_Camera.uasset             # legacy — remplacée
│   ├── AdvancedThermalVision/           # Copié depuis PixGen
│   │   ├── MaterialInstances/
│   │   │   ├── MI_ThermalGrayscale.uasset
│   │   │   ├── MI_ThermalGrayscale2.uasset
│   │   │   ├── MI_ThermalGrayscale_Variante1.uasset
│   │   │   └── MI_Thermal.uasset
│   │   ├── Materials/
│   │   └── MaterialFunctions/
│   │       ├── MF_ThermalColorRange.uasset
│   │       └── MF_LightBuffer.uasset
│   ├── shahed/
│   │   ├── Geran21.uasset
│   │   ├── GeranThermal.uasset
│   │   └── textures/
│   └── uav_BP.uasset                    # Référence PixGen (ne pas modifier)
├── Plugins/
│   ├── CesiumForUnreal/
│   └── BoundingBox2D/
├── output/
│   └── {scenario_id}/
│       ├── images/   → 0.png, 1.png, ...
│       └── labels/   → 0.csv, 1.csv, ...
└── ARCHITECTURE.md

5. Niveaux UE5
Niveau	Statut	Description
NewMap	❌ Abandonné	Template Open World avec Landscape non supprimable
SynthScene	✅ Actif	Empty Level propre, sans Landscape parasite
Leçon apprise : toujours partir d'un Empty Level (pas Open World) pour éviter le Landscape World Partition non supprimable.
Leçon apprise : le PostProcessVolume s'ajoute via Window → Place Actors (pas via le clic droit viewport ni la barre Cmd).

6. Composants Blueprint
6.1 BP_Drone
Components :
* StaticMesh → Geran21
    * Render Custom Depth Pass = ✅
    * Custom Depth Stencil Value = 1
Variables (Instance Editable) :
Variable	Type	Défaut
FlightSpeed	Float	5000.0
OscillationAmplitude	Float	100.0
OscillationFrequency	Float	10.0
EventGraph (Event Tick) :
FlightSpeed × DeltaSeconds → X
Sin(GameTime) × OscillationAmplitude → Z
→ Make Vector → Add Actor World Offset
Actor Tags (Class Defaults) :
* drone — label CSV
* BB — détection plugin
⚠️ Tags définis dans Class Defaults → Compile pour persister entre niveaux.

6.2 BP_Bounding_Box_Camera
Origine : Plugin Plasma-Lab, modifié avec logique de tracking.
Components :
* SceneCaptureComponent2D
* SceneCaptureComponent2D_PP
Variables ajoutées :
Variable	Type	Instance Editable
TargetActor	BP_Drone (Object Ref)	✅
MaxOffsetDegrees	Float	✅
Event BeginPlay :
Setup 2D Bounding Boxes
Event Tick :
Get Actor Location (self) → Start ┐
                                   ├─ Find Look at Rotation
Get Actor Location (drone) → Target ┘
  + Random Offset (±MaxOffsetDegrees sur Roll/Pitch/Yaw)
  → Combine Rotators → Set Actor Rotation
  → Take Screenshot With Bounding Box (Path: /output/{id})
Philosophie de tracking : la caméra simule un opérateur humain imparfait. Le drone peut être centré, décentré, en bordure, ou hors frame — diversité réaliste pour l'entraînement.
Résolution : 1000×600 px (Details → Camera Settings)

6.3 PostProcessVolume
Configuration :
* Infinite Extent (Unbound) = ✅
* Color Grading → Global → Saturation = 0 (noir et blanc propre)
⚠️ S'ajoute via Window → Place Actors uniquement — pas disponible via clic droit viewport.

7. Simulation IR / LWIR
Approche retenue
PostProcessVolume (Infinite Unbound)
  Color Grading → Global → Saturation = 0
  Contrast légèrement > 1.0
Propre, sans bruit, terrain lisible. Simule une caméra IR basique.
Approches testées et abandonnées
Approche	Résultat
MI_ThermalGrayscale sur SceneCaptureComponent2D	❌ Très bruité
MI_ThermalGrayscale2 sur SceneCaptureComponent2D	⚠️ Légèrement meilleur mais bruité
MI_ThermalGrayscale_Variante1 sur PostProcessVolume	⚠️ Bruité
Assets disponibles (AdvancedThermalVision, copié depuis PixGen)
MI_ThermalGrayscale / 1 / 2 / Variante1    — niveaux de gris
MI_Thermal / Variante1 / 2                  — fausses couleurs IR
MI_NightVision                               — vision nocturne
MF_ThermalColorRange                         — mapping température → couleur

8. Format des labels
CSV
drone,258,322,474,361
Champ	Description
drone	Classe (= premier Actor Tag)
x1, y1	Coin supérieur gauche du bounding box
x2, y2	Coin inférieur droit du bounding box
YOLO (option dans node Blueprint)
drone, x_center, y_center, width, height   (normalisé 0-1)

9. Configuration géographique
Paramètre	Valeur
Lieu	Zaporizhzhia, Ukraine
Latitude	47.8388
Longitude	35.1396
Height	500.0 m
Asset Cesium Ion	Google Photorealistic 3D Tiles (ID: 2275207)
10. Setup onboarding
git clone <repo> ~/Code/SynthDrone
# Double-clic SynthDrone.uproject → UE5 recompile plugins
# Ouvrir SynthScene.umap
Project Settings obligatoires : Edit → Project Settings → Rendering → Custom Depth-Stencil Pass → Enabled With Stencil
Connexion Cesium Ion : Panneau Cesium → coller le token → Enter

11. Roadmap
Priorité	Tâche
🔴	Trajectoires chasing — caméra derrière le drone, pas en face
🔴	Diversité trajectoires — lignes, virages, spirales, zigzag
🔴	Valider rendu IR (Color Grading Saturation=0)
🟠	Python batch — scénarios automatiques (GPS, altitude, distance, trajectoire)
🟠	TrackingLag — simuler retard opérateur
🟠	Signature thermique drone — Geran21 chaud (blanc) vs fond froid
🟡	Plusieurs GPS locations par batch
🟡	Numérotation automatique scénarios
12. Différences avec PixGen
Aspect	PixGen	SynthDrone
Compréhension	Boîte noire	Entièrement compris
Niveau	cesiumTest.umap	SynthScene.umap (Empty Level)
Drone mesh	Geran21	Geran21 (copié)
Tracking	Boîte noire	Find Look at Rotation + offset aléatoire explicite
IR	PostProcessVolume inconnu	Color Grading Saturation=0
Bounding Box	Plugin Plasma-Lab	Plugin Plasma-Lab (identique)
Batch	Script Python non compris	À construire
13. Références
* Cesium for Unreal
* 2D Bounding Box Plugin — Plasma-Lab
* PixGen (référence interne)
* Unreal Python API

