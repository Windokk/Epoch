# SHA.me
A psychological horror game


## ROADMAP (Engine)

### Renderer

- [ ] PBR materials/shaders
- [x] FBX importer (single mesh)
- [ ] FBX importer (multiple meshes)
- [x] FBX import (single materials)
- [x] Multiple materials per mesh + FBX import (multiple materials)
- [ ] Skyboxes
- [ ] LODs
- [ ] LODs generator
- [ ] Post processing effects shaders (SSAO, Bloom, Vignette, Tone Mapping, Film Grain, Chromatic Abberration)
- [ ] Frustum culling

### Game code loading and execution

- [x] Game as executable, engine as static library
- [x] Game logic as dynamic library (.dll/.so/.dylib), loaded at runtime
- [x] Custom components defined inside game dll, registered and serialized in engine

### Serialization

- [x] Level importer
    - [x] Material file parser
- [ ] Level exporter
    - [ ] Material file exporter

### Debugging

- [x] Debugger : 
    - [x] Multiple debugger levels : 
        - [x] LOG
        - [x] INFO
        - [x] WARNING
        - [x] ERROR
        - [x] FATAL ERROR (CRASH)
- [ ] Commands (in game/editor)
- [ ] Frame-Debugger (see stats overlay)

### Stats overlay

- [ ] FPS
- [ ] Frame profiler
- [ ] Sound infos
- [ ] Scene infos
- [ ] Hardware infos (CPU, GPU, RAM) (Name, Vendor, Usage...)

### Animation system

- [ ] Animation data structure importing
- [ ] Animation manager
- [ ] Skeletal animations
- [ ] Animation Blending & State Machine
 
### Time Module

- [x] Global time variables : fixedDeltaTime, deltaTime
- [x] Global time constants : time
- [x] Precise time decomposition : milliseconds, second, minutes, hours...