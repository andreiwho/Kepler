Roadmap:
===============================
## Tier 1:

### - Application
* Module stack
* Main loop
* Basic structure

### - Renderer
* Shaders shoud be compiled from text files and loaded into the program
* Shader input layout should be reflected
* Shader uniform layout ***must*** be set by hand (including constant buffer parameters and samplers)
* Vertex buffers, index buffers and transfer buffers
* Image loading and displaying on a plane
* GLTF2.0 model loading and rendering
* Rendering onto render target and displaying it on a plane
* ***Render thread*** is the main executor of the rendering commands. 
* * Executing render commands on another thread must be ***forbidden***

### - Audio
* Loading of audio clips
* Ability to play selected audio clip
* Audio streaming for long tracks

### - VFS
* Management of the working directory
* Setting up default directories for in-game assets
* Virtual folders such as ***'Game/'*** ***'Engine/'*** 
* * Also would be great to add ability for custom folders in VFS (*e.g. '/Shaders' '/Resources'*)

## Tier 2:

### - ECS
* Basic ***entity component system***
* Entities, components and game world.
* * Entity object (no name yet)
* * Mesh component (*mesh + material*)
* * Audio source component
* Scene system (saving and loading game worlds)
* Abstracting render thread submits for client code
* Abstracting camera into a distinct entity

### - Renderer
* Z pre-pass
* Divide renderer into simple passes
* Deferred shading (basic level for now, no PBR)
* Global renderer state needs to handle a bunch of things:
* * Managing cameras and projections
* * Handling all render targets and recreating them as needed
* * Managing access of the depth targets
* * ***Global renderer-wise TParamBuffer*** which will hold all of the information for drawing primitives

### - Application
 * Move client code into ***testbed*** module
 * Prepare for editor creation

### - Audio

* ...make it ***3D!***
* and able to work with the Audio source component

*Important note: Engine is being developed as a modular and extensible thing, though the main gameplay scripting language will be C++*