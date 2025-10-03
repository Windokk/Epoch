<center>

# Epoch Engine

<picture>
  <source media="(prefers-color-scheme: dark)" srcset=".github/EpochLogoDarkMode.png" width="20%">
  <source media="(prefers-color-scheme: light)" srcset=".github/EpochLogoLightMode.png" width="20%">
  <img alt="Fallback image description" src="default-image.png">
</picture>

<br>

Yet another game engine
(full of bugs)

</center>

## Screenshots

## FEATURES

### Rendering

- PBR shaders/materials
- Light system : Directionnal, Spot, Point
- Shadow maps : CSM, PCF
- Model component (ECS)

#### UI

- Loading fonts
- Text components (ECS)
- UI Pass Type

### Audio System

- 3D Stereo audio
- Audio source component (ECS)

### Filesystem

- Cosem Packer/Unpacker
- Reading/Writing files

### Resources Manager

- Loading/Unloading per folder + file type

### Game Module

- Game module loading at runtime in a DLL
- Code execution across DLL

### Editor Module

- Editor module loaded at runtime in a DLL
- FPS Counter

### Debugging

- Debug levels (Log, Info, Warning, Error, Fatal)

### ECS

- Level hierarchy
- Local/Global components ids
- Actor ids

### Events System

- Sending/Recieving events (across custom class/across engine) via a dispatcher

### Inputs System

- Checking for keyboard/mouse inputs
- Cursor state

### Level System

- Loading/Unloading levels
- Adding actors to the level

### Serialization

- Level de-serializer
- Material de-serializer

### Physics System

- Jolt integration
- Physics shapes visualizing

### Time System

- Time queries (delta, fixedDelta, current global (world) time, current app time)
- Timespeed (for physics calculations, particles and custom classes)

## ROADMAP

### Editor

- [ ] Basic Level Editor (ImGUI)
- [ ] Material Editor
- [ ] Console
- [ ] Stats overlay
  - [ ] Frame profiler
  - [ ] Sound infos
  - [ ] Scene infos
  - [ ] Hardware infos (CPU, GPU, RAM) (Name, Vendor, Usage...)

### Rendering

- [ ] Transparency
- [ ] FBX importer (multiple meshes)
- [ ] Skyboxes
- [ ] LODs
- [ ] LODs generator
- [ ] Post processing effects shaders (SSAO, Bloom, Vignette, Tone Mapping, Film Grain, Chromatic Abberration)

#### Serialization

- [ ] Level exporter
- [ ] Material file exporter

#### Debugging

- [ ] Commands (in game/editor)
- [ ] Frame-Debugger (see stats overlay)

#### Animation system

- [ ] Animation data structure importing (from FBX)
- [ ] Animation data manager
- [ ] Skeletal animations
- [ ] Animation Blending
- [ ] Animation State Machine

#### UI

- [ ] GUI components (3D/Canvas) : text, button, input text, scroll bar, image, layout

## Credits

- Models :
  - "Rubik's Cube" (<https://skfb.ly/6U7pp>) by RED2000 is licensed under Creative Commons Attribution (<http://creativecommons.org/licenses/by/4.0/>).
  - "Sponza" Model downloaded from Morgan McGuire's [Computer Graphics Archive](https://casual-effects.com/data)
