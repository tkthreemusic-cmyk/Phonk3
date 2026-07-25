# 🎹 Phonk Synth — Instrument VST3

<div align="center">

![Version](https://img.shields.io/badge/Version-1.0.0-blue)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20macOS-orange)
![Format](https://img.shields.io/badge/Format-VST3%20%7C%20Standalone-green)
![License](https://img.shields.io/badge/License-Proprietary-red)

**Phonk Synth** est un synthétiseur subtractif professionnel conçu spécifiquement pour la production de musique **Phonk**, trap et hip-hop. Il intègre tous les éléments essentiels pour créer des basses 808 punchy, des leads distortés et des pads atmosphériques.

</div>

---

## ✨ Fonctionnalités Principales

### 🎛️ Moteur de Synthèse Avancé
- **2 Oscillateurs** (Saw, Square, Sine, Triangle) avec détune indépendant
- **Sub Oscillateur** dédié pour les basses fréquences profondes
- **Générateur de Bruit** pour ajouter de la texture et de l'air
- **Mode Poly/Mono** avec Glide Time pour les slides de 808 caractéristiques

### 🔊 Chaîne d'Effets Intégrée
| Effet | Description |
|-------|-------------|
| **Drive** | Saturation Memphis (Tape, Fuzz, Crush) pour la chaleur analogique |
| **Lo-Fi** | Bitcrusher et réduction de sample rate pour l'esthétique vintage |
| **Chorus** | Modulation stéréo pour largeur et mouvement |
| **Reverb** | Réverbération algorithmique pour l'ambiance spatiale |

### 🎚️ Filtre Résonant
- Types: **Low-Pass**, **High-Pass**, **Band-Pass**
- Enveloppe de filtre modulable
- Key Tracking pour le suivi automatique des notes

---

## 🎯 Pour Commencer : Preset 808 Phonk Classique

Paramètres recommandés pour une basse 808 authentique :

```
Voice Mode = Mono
Glide Time = 80-150ms
Osc 1 = Sine (Level 100%)
Sub Level = 70-100%
Filter Cutoff = 800-2000Hz
Filter Resonance = Modéré
Filter Env Amount = Faible à Modéré
Amp Attack = Très court
Amp Release = Moyen à Long
Drive Mode = Tape (Amount ~30-40%)
Reverb Mix = 10-15%
```

---

## 📦 Installation

### Windows (FL Studio)

```powershell
# Compiler avec Visual Studio 2022 Community
cd PhonkSynth
cmake -B build -G "Visual Studio 17 2022"
cmake --build build --config Release
```

Le fichier `.vst3` compilé se trouve dans :
```
build\PhonkSynth_artefacts\Release\VST3\Phonk Synth.vst3
```

**Installation :**
1. Copiez le fichier `.vst3` dans `C:\Program Files\Common Files\VST3\`
2. Dans FL Studio : **Options → Manage Plugins → Find Plugins (Rescan)**

### macOS

```bash
cmake -B build -G Xcode
cmake --build build --config Release
```

Copiez le `.vst3` dans `~/Library/Audio/Plug-Ins/VST3/`

---

## 🎛️ Guide des Paramètres

| Section | Paramètres |
|---------|------------|
| **Oscillateurs** | Wave (Saw/Square/Sine/Triangle), Level, Detune |
| **Voix** | Mode (Poly/Mono), Glide Time |
| **Sub / Noise** | Sub Level, Noise Level |
| **Amp Envelope** | Attack, Decay, Sustain, Release |
| **Filtre** | Type (LP/HP/BP), Cutoff, Resonance, Env Amount, Key Track |
| **Filter Env** | Attack, Decay, Sustain, Release |
| **Drive** | Amount, Mode (Tape/Fuzz/Crush), Tone |
| **Lo-Fi** | Bit Depth, Sample Reduce, Mix |
| **Chorus** | Rate, Depth, Mix |
| **Reverb** | Size, Damping, Mix |
| **Output** | Output Gain |

---

## 🔧 Mode Mono / Glide — Les Slides de 808

En mode **Mono**, si vous jouez une nouvelle note *pendant que la précédente est encore tenue* (legato), le synthétiseur **ne redéclenche pas les enveloppes** : il glisse simplement la hauteur vers la nouvelle note avec la durée reglée par **Glide Time**.

C'est exactement ce qui crée l'effet de **glissando caractéristique des basses 808** en phonk. Si vous jouez des notes qui ne se chevauchent pas, chaque note redéclenche normalement (attaque franche).

En mode **Poly** (jusqu'à 8 voix), chaque note est indépendante — idéal pour les mélodies et pads.

---

## 📁 Structure du Projet

```
PhonkSynth/
├── CMakeLists.txt              # Configuration CMake + JUCE
├── README.md                   # Documentation
└── Source/
    ├── PluginProcessor.h/.cpp  # Traitement audio, gestion MIDI, effets maître
    ├── PluginEditor.h/.cpp     # Interface graphique
    ├── Synth/
    │   ├── PhonkSynthVoice.h/.cpp  # Moteur de synthèse d'une voix
    │   └── PhonkSynthSound.h       # Configuration du son
    └── DSP/
        ├── PhonkDrive.h            # Saturation (Tape/Fuzz/Crush)
        └── LofiCrusher.h           # Bitcrusher / réduction sample-rate
```

---

## 🛠️ Prérequis de Compilation

- **Windows** : Visual Studio 2022 Community (module C++), CMake 3.22+, Git
- **macOS** : Xcode, CMake 3.22+

---

## 📝 Licence

Plugin propriétaire — TonStudio

---

<div align="center">

**Fait avec ❤️ pour la communauté Phonk**

</div>
