# Kepler Game Engine (Temp name)

The engine is built in C++, partially using own C++ parser for reflection code generation.

## Build steps:
1. Clone the repository using the dev branch
2. Open Scripts folder and run ```GenerateEditor.bat```. 
3. Open ```Intermediate/KE1.sln```

If you want to build the runtime (no editor build), you should run ```GenerateRuntime.bat```

Also, anytime you add new .cpp/.h/.cs files to projects you should run ```Generate***NotFresh.bat``` with respect to the build type, 
you have selected before.

## Reflection system

The engine supports simple kind of reflection, using ```reflected``` and ```kmeta``` keywords. If you want to add a class to the reflection database, you should write ```reflected``` before it. A special application __KEReflector__ then reads the header file and adds the class to the database. Only fields, marked as ```reflected``` will be exported and serialized if requested.

*Note* that if a header file contains reflected class, it should include a header with the same name, but ```.gen.h``` instead of ```.h``` (e.g. if header is called *World.h*, the included file should be called *World.gen.h*). Also note, that for now all reflected classes ***MUST*** have default constructors (this is a subject to change in a future roadmap tear). 

If you want to add some special metadata to classes or fields you should use the ```kmeta(...)``` keyword. For now kmeta for clases supports keywords such as ***hideindetails***, ... more will be added.

For fields ***kmeta*** keyword supports following specifiers: 
 * ***hideindetails*** - to hide the field from the details panel in editor. This field will still be serialized
 * ***prechange/postchange*** with the name of the callback function - specify a callback that will be called before the value is changed by the reflection delegate or after (*Note* that this works only if the field value is being changed using the reflection API such as ReflectedField::SetValueFor<>)
 * ***assettype*** - works only for ```AssetTreeNode*``` type. Specify what kind of asset this field can hold. This is a helper field for the editor to know which kinds of assets could be dragged and dropped into the value box. Currently supported are: Map/Material/StaticMesh.
 * ***dragdrop*** - this is almost useless field specifier, but can be used to specify custom dragndropp target. The ***assettype*** specifier also utilizes this one.

```C++
// StaticMeshComponent.h
reflected class StaticMeshComponent : public EntityComponent
{
public:
	// ...

    // We specify that the OnMeshAssetChanged function will be called after the reflection system sets a new asset.
    // To handle this change.
    // And the assettype=StaticMesh specifies that this asset is indeed a static mesh, and we should disallow dropping 
    // e.g. materials into the field
    reflected kmeta(postchange = OnMeshAssetChanged, assettype=StaticMesh)
	AssetTreeNode* Asset {nullptr};

    // Use mesh loader to reload the mesh from the asset
	void OnMeshAssetChanged(AssetTreeNode* pAsset);
private:
	RefPtr<StaticMesh> m_StaticMesh{};
};
```

Every project also must contain a class, marked as ```holding_reflection_data```. This keyword is used to generate special method implementation, named FillReflectionDatabaseEntries, which reads reflection data from the project and adds it to the ReflectionDatabase. The user has to define a method PushClass(RefPtr\<ReflectedClass\>) to add a class into the reflection database. See Testbed class for a better example.

```C++
/// Testbed.h

holding_reflection_data 
class TestbedEngine : public Engine
{
	using Base = Engine;
public:
	TestbedEngine(const TApplicationLaunchParams& LaunchParams);
	
	void FillReflectionDatabaseEntries();
	void PushClass(RefPtr<ReflectedClass> cls);
protected:
	virtual void ChildSetupModuleStack(TModuleStack& ModuleStack) override;
};

// Testbed.cpp
void TestbedEngine::PushClass(RefPtr<ReflectedClass> cls)
{
	ReflectionDatabase::Get()->PushClass(cls);
}

// Note that the FillReflectionDatabaseEntries is generated by the reflection system and should not be defined.
```

### Enums

Enums are also reflected. If you add a ```reflected``` keyword to the enum or enum class, all of its' entries will be reflected with specified or not specified values.

### Reflection Summary

You can acquire a pointer to the ```ReflectedClass``` or ```ReflectedEnum``` instance using the ```GetReflectedClass<T>``` or ```GetReflectedEnum<T>``` functions in the ```Reflection/ReflectionDatabase.h```.
Reflection system helps to serialize certain objects automatically to binary or json data and expose entire classes to the editor.

## World

The world is a collection of entities and their components. All components used with entities must be marked as ```reflected``` in order to be properly serialized when the world is being saved/loaded. At the time of writing this document the user is capable of creating new entities, adding components to them and placing them in the world. Internal systems handle these components as they see correct, but for special class called ```NativeScriptComponent``` which should be used as a parent class for the component to specify user-defined entity behavior it searches for specified Update/Init/Destroyed functions to add world related functionality to them.

Components can be added via ```GameWorld::AddComponent<T>``` or ```EntityHandle::AddComponent<T>``` functions. Also editor can be used for this.

## Editor
It is not as big as it needs to be for now, but the roadmap has all the good stuff ahead.
