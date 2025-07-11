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

### Serialization

- [ ] Level importer
    - [ ] Material file parser
- [ ] Level exporter
    - [ ] Material file exporter

### Debugger

- [ ] Debugger : 
    - [ ] Multiple debugger levels : 
        - [ ] LOG/INFO
        - [ ] WARNING
        - [ ] ERROR
        - [ ] FATAL ERROR (CRASH)
    - [ ] Commands (in game/editor)

- [ ] Frame-Debugger (see stats overlay)
- [ ] Crash Handler

### Stats overlay

- [ ] FPS
- [ ] Frame profiler
- [ ] Sound infos
- [ ] Scene infos
- [ ] Hardware infos (CPU, GPU, RAM) (Name, Vendor, Usage...)

### Unified Math Interface

- [ ] Base types : float, int, vec2, vec3, vec4, mat2, mat3, mat4
- [ ] Physics types : Ray, Frustum geometry
- [ ] Interpolation : Lerp, Slerp (Custom curves)

### Animation system

- [ ] Animation data structure importing
- [ ] Animation manager
- [ ] Skeletal animations
- [ ] Animation Blending & State Machine
 