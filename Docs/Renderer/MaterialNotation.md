# Kepler material notation (json)

The engine uses '.json'-like material notation (*actually, it is json*).  
Every material must specify __pipeline__ it will use, some properties specific to that pipeline (*e.g. specular strength, emissive strength, uv-scaling, etc.*)  

## Example
Material difinition looks roughly like this (*'Engine://Materials/Mat_DefaultUnlit.kmat'*):
```json
{
    "Material": {
        "Pipeline": "DefaultUnlit",
        "Samplers": {
            // Samplers in form:
            // "Albedo": "Game://Ground.png", etc...  
            // Supported samplers for now are specified by the pipeline and shaders, so care should be taken
        }
    }
}
```

***Note: Shaders and Pipelines do not necessary have their parameter and sampler names ideally mapped.***  
Specifically, ```hlsl``` shaders have Samplers and Textures as separate objects, but the engine has them as a single ```TTextureSampler2D``` object, thus mappings are rather oriented on slots than the names.

# Material notation manual

***The most important thing is that the "Material" and "Pipeline" tags must be specified (look at example above)***  
* If pipeline declares samplers, either a __path__ to the texture or ```null``` ***must*** be defined for the corresponding slot in the material file. 
  ```json
  //...
  "Samplers": {
      "Albedo": "Game://Ground.png",
      "Normal": null,   // Note to devs, pipelines must support null for sampler slots
  }
  ```

