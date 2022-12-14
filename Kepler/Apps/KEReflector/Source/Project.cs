using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;

namespace KEReflector
{
    public class Project
    {
        public string Name { get; set; }
        public string ProjectDirectory { get; set; }
        public string EngineRoot { get; set; }
        public string SourceDir { get; set; }
        public bool bAnyFilesChanged { get; set; } = false;
        public List<ParsedHeader> Headers { get; set; }
        public Dictionary<string, ReflectedClass> ProjectClasses { get; set; } = new Dictionary<string, ReflectedClass>();
        public ParsedHeader SpecialFileHeader = null;
        public readonly string _generatedDirectory;

        private SourceDatabase _sourceDb;

        public Project(string name, string directory, string engineRoot)
        {
            Name = name;
            ProjectDirectory = directory;
            EngineRoot = engineRoot;
            SourceDir = $"{ProjectDirectory}/Source";
            Headers = new List<ParsedHeader>();
            _generatedDirectory = $"{ProjectDirectory}/Generated";
            _sourceDb = new SourceDatabase(_generatedDirectory);
        }

        Dictionary<string, ReflectedClass> CombineProjectClasses()
        {
            Dictionary<string, ReflectedClass> result = new Dictionary<string, ReflectedClass>();

            foreach (var header in Headers)
            {
                foreach (var headerClass in header.Classes)
                {
                    result.Add(headerClass.Key, headerClass.Value);
                }
            }

            return result;
        }

        void ReadDirectory(string directory)
        {
            foreach (var entry in Directory.GetFileSystemEntries(directory))
            {
                FileAttributes attr = File.GetAttributes(entry);
                if (attr == FileAttributes.Directory)
                {
                    ReadDirectory(entry);
                }
                else
                {
                    if (Path.GetExtension(entry) == ".h")
                    {
                        var headerPath = entry.Replace('\\', '/');
                        Headers.Add(new ParsedHeader(headerPath));
                        _sourceDb.AddEntry(headerPath);
                    }
                }
            }
        }

        public void ReadProjectFiles()
        {
            ReadDirectory(SourceDir);
            ProjectClasses = CombineProjectClasses();
            GenerateHeaders();

            _sourceDb.FlushDatabase();
        }

        void WriteGeneratedClassBody(ParsedHeader header, ref StreamWriter fileWriter)
        {
            string fileId = Path.GetRelativePath(ProjectDirectory, header.Path);
            fileId = fileId.Replace('\\', '_');
            fileId = fileId.Replace('.', '_');
            fileId = "FILEID" + fileId.Replace('/', '_');
            
            fileWriter.WriteLine($"#undef FILEID");
            fileWriter.WriteLine($"#define FILEID {fileId}");

            foreach(var cls in header.Classes)
            {
                if(cls.Value.bIsEnum)
                {
                    continue;
                }
                
                if(cls.Value.ReflLine == -1 && !cls.Value.bIsEnum)
                {
                    Console.WriteLine("################# ERROR ######################");
                    Console.WriteLine($"Class {cls.Key} must have reflected_info() call in it.");
                    Console.WriteLine("##############################################");
                }
                
                fileWriter.WriteLine($"#define {fileId}{cls.Value.ReflLine}REFL using Self = {cls.Value.Name};\\");
                if(cls.Value.Parent != null)
                {
                    fileWriter.WriteLine($"using Base = {cls.Value.Parent};\\");
                }
                fileWriter.WriteLine($"friend class R{cls.Value.Name};\\");
                fileWriter.WriteLine("public:\\");
                fileWriter.WriteLine($"static ReflectedClass* GetStaticClass() {{ return R{cls.Value.Name}::Get(); }}\\");
                fileWriter.WriteLine($"inline ReflectedClass* GetClass() const {{ return R{cls.Value.Name}::Get(); }}\\");
                fileWriter.WriteLine("private:");
            }
        }

        void WriteHeaderFileCode(ParsedHeader header)
        {
            var fileName = Path.GetFileNameWithoutExtension(header.Path);
            

            var generatedHeaderPath = $"{_generatedDirectory}/{fileName}.gen.h";
            StreamWriter fileWriter;
            if (!File.Exists(generatedHeaderPath))
            {
                fileWriter = File.CreateText(generatedHeaderPath);
            }
            else
            {
                fileWriter = new StreamWriter(generatedHeaderPath);
            }

            if (fileWriter != null)
            {
                fileWriter.WriteLine("#pragma once");
                WriteGeneratedClassBody(header, ref fileWriter);
                fileWriter.WriteLine("#include \"Reflection/Class.h\"\n");
                fileWriter.WriteLine("namespace ke");
                fileWriter.WriteLine("{");

                foreach (var entry in header.Classes.Values)
                {
                    if (entry.bIsSpecial)
                    {
                        continue;
                    }

                    string entryParent = entry.Parent != null ? entry.Parent : "None";
                    string hasParent = entry.Parent != null ? "true" : "false";
                    string isEnum = entry.bIsEnum ? "true" : "false";
                    string reflectedParent = entry.bIsEnum ? "ReflectedEnum" : "ReflectedClass";
                    string isComponent = IsClassComponent(entry) ? "true" : "false";

                    fileWriter.Write($@"
    // class {entry.Name};
    class R{entry.Name} : public {reflectedParent}
    {{
        static R{entry.Name}* Instance;
    public:
        R{entry.Name}();
        static R{entry.Name}* Get() {{ return Instance; }}
        virtual String GetName() const override {{ return ""{entry.Name}""; }}
        virtual bool HasParent() const override {{ return {hasParent}; }}
        virtual String GetParentName() const override {{ return ""{entryParent}""; }}
        virtual bool IsEnum() const override {{ return {isEnum}; }}
        virtual void* Construct(void* pAddress) const override;
        virtual void* RegistryConstruct(entt::entity entity, entt::registry& registry) const override;
        virtual bool IsComponentClass() const override {{ return {isComponent}; }}");
                    fileWriter.WriteLine(@"
    };");
                }

                fileWriter.WriteLine("}");
                fileWriter.Flush();
                fileWriter.Close();
            }
        }

        bool IsClassComponent(ReflectedClass inClass)
        {
            if(inClass == null)
            {
                return false;
            }

            if(inClass.Parent == null)
            {
                return false;
            }

            if(inClass.Parent == "EntityComponent")
            {
                return true;
            }

            if (!ProjectClasses.ContainsKey(inClass.Parent))
            {
                return false;
            }

            return IsClassComponent(ProjectClasses[inClass.Parent]);
        }

        void WriteHeaderFileImpl(ParsedHeader header)
        {
            var fileName = Path.GetFileNameWithoutExtension(header.Path);
            var generatedCppPath = $"{_generatedDirectory}/{fileName}.gen.cpp";
            StreamWriter fileWriter = null;
            if (!File.Exists(generatedCppPath))
            {
                fileWriter = File.CreateText(generatedCppPath);
            }
            else
            {
                fileWriter = new StreamWriter(generatedCppPath);
            }

            if (fileWriter != null)
            {
                fileWriter.WriteLine($"#include \"{header.Path}\"");
                fileWriter.WriteLine("#include \"World/Game/GameWorld.h\"\n");

                foreach (var entry in header.Classes.Values)
                {
                    if (entry.bIsSpecial)
                    {
                        WriteSpecialCppClass(fileWriter, entry);
                    }
                    else
                    {
                        fileWriter.WriteLine("namespace ke\n{");
                        fileWriter.WriteLine($"\tR{entry.Name}* R{entry.Name}::Instance = nullptr;");
                        fileWriter.WriteLine($"\tR{entry.Name}::R{entry.Name}()\n\t{{");

                        // Recursive parent fields
                        fileWriter.WriteLine($"\t\tInstance = this;");
                        fileWriter.WriteLine($"\t\tm_ClassId = ClassId(\"{entry.Name}\");");

                        FillClassMetadata(fileWriter, entry);
                        fileWriter.WriteLine($"\t\tm_Metadata = {entry.Name}Metadata;");

                        if (entry.bIsEnum)
                        {
                            WriteEnumAccessors(entry, ref fileWriter);
                        }
                        else
                        {
                            WriteFieldAccessors(entry, ref fileWriter);
                        }

                        fileWriter.WriteLine("\t}");

                        string setupComponent = IsClassComponent(entry) ? $"GameWorld::SetupNativeComponent<{entry.Name}>()" : "";

                        // Construct
                        fileWriter.WriteLine($@"
    void* R{entry.Name}::Construct(void* pAddress) const
    {{
        return new(pAddress) {entry.Name}();
    }}

    void* R{entry.Name}::RegistryConstruct(entt::entity entity, entt::registry& registry) const
    {{
        {setupComponent};
        return &registry.emplace<{entry.Name}>(entity);
    }}
");


                        fileWriter.WriteLine("}");
                    }
                }

                fileWriter.Flush();
                fileWriter.Close();
            }
        }

        private void WriteMetadataSpecifier(StreamWriter fileWriter, string objName, string specifier, string value)
        {
            fileWriter.WriteLine($"\t\t{objName}Metadata.{specifier} = {value};");
        }

        private void FillClassMetadata(StreamWriter fileWriter, ReflectedClass reflectedClass)
        {
            fileWriter.WriteLine($@"
        ClassMetadata {reflectedClass.Name}Metadata{{}};");
            foreach (var specifier in reflectedClass.MetadataSpecifiers)
            {
                if (specifier.Key == "hideindetails")
                {
                    WriteMetadataSpecifier(fileWriter, reflectedClass.Name, "bHideInDetails", "true");
                }
            }
        }

        private void FillFieldMetadata(StreamWriter fileWriter, ReflectedField field, ReflectedClass parentClass)
        {
            fileWriter.WriteLine($@"
        FieldMetadata {field.DisplayName}Metadata{{}};");
            foreach (var specifier in field.MetadataSpecifiers)
            {
                if (specifier.Key == "readonly")
                {
                    WriteMetadataSpecifier(fileWriter, field.DisplayName, "bReadOnly", "true");
                }

                if (specifier.Key == "hideindetails")
                {
                    WriteMetadataSpecifier(fileWriter, field.DisplayName, "bHideInDetails", "true");
                }

                if(specifier.Key == "editcond")
                {
                    WriteMetadataSpecifier(fileWriter, field.DisplayName, "bHasEditCondition", "true");
                    WriteMetadataSpecifier(fileWriter, field.DisplayName, "EditConditionAccessor", $"[](void* pObject) {{ return (({parentClass.Name}*)pObject)->{specifier.Value}; }};");
                }

                if (specifier.Key == "editspeed")
                {
                    WriteMetadataSpecifier(fileWriter, field.DisplayName, "EditSpeed", $"{specifier.Value}");
                }

                if(specifier.Key == "dragdrop")
                {
                    WriteMetadataSpecifier(fileWriter, field.DisplayName, "bEnableDragDrop", "true");
                }

                if(specifier.Key == "assettype")
                {
                    WriteMetadataSpecifier(fileWriter, field.DisplayName, "bEnableDragDrop", "true");
                    WriteMetadataSpecifier(fileWriter, field.DisplayName, "FieldAssetType", $"EFieldAssetType::{specifier.Value}");
                }

                if(specifier.Key == "clampmin")
                {
                    WriteMetadataSpecifier(fileWriter, field.DisplayName, "ClampMin", specifier.Value);
                }

                if (specifier.Key == "clampmax")
                {
                    WriteMetadataSpecifier(fileWriter, field.DisplayName, "ClampMax", specifier.Value);
                }
            }

            if (field.bIsPointer)
            {
                WriteMetadataSpecifier(fileWriter, field.DisplayName, "bIsPointer", "true");
            }

            if (field.bIsRefPtr)
            {
                WriteMetadataSpecifier(fileWriter, field.DisplayName, "bIsRefPtr", "true");
            }

            if (ProjectClasses.ContainsKey(field.Type) && ProjectClasses[field.Type].bIsEnum)
            {
                WriteMetadataSpecifier(fileWriter, field.DisplayName, "bIsEnum", "true");
            }
        }

        private void WriteFieldAccessors(ReflectedClass entry, ref StreamWriter fileWriter)
        {
            foreach (var field in entry.Fields)
            {
                FillFieldMetadata(fileWriter, field, entry);

                string preChangeHandler = "";
                string preChangeHandlerPtr = "";
                if(field.MetadataSpecifiers.ContainsKey("prechange"))
                {
                    preChangeHandler = $"(({entry.Name}*)pHandler)->{field.MetadataSpecifiers["prechange"]}(*({field.Type}*)pValue);";
                    preChangeHandlerPtr = $"(({entry.Name}*)pHandler)->{field.MetadataSpecifiers["prechange"]}(({field.Type}*)pValue);";
                }


                string postChangeHandler = "";
                string postChangeHandlerPtr = "";
                if(field.MetadataSpecifiers.ContainsKey("postchange"))
                {
                    postChangeHandler = $"(({entry.Name}*)pHandler)->{field.MetadataSpecifiers["postchange"]}(*({field.Type}*)pValue);";
                    postChangeHandlerPtr = $"(({entry.Name}*)pHandler)->{field.MetadataSpecifiers["postchange"]}(({field.Type}*)pValue);";
                }

                if (field.bIsPointer)
                {
                    string getString = $"return (void*)(({entry.Name}*)pHandler)->{field.Name}";
                    if (field.MetadataSpecifiers.ContainsKey("get"))
                    {
                        getString = $"return (void*)(({entry.Name}*)pHandler)->{field.MetadataSpecifiers["get"]}()";
                    }

                    string setString = $"(({entry.Name}*)pHandler)->{field.Name} = ({field.Type}*)pValue";
                    if(field.MetadataSpecifiers.ContainsKey("set"))
                    {
                        setString = $"(({entry.Name}*)pHandler)->{field.MetadataSpecifiers["set"]}(({field.Type}*)pValue)";
                    }
                    
                    fileWriter.WriteLine($@"
        PushField(""{field.DisplayName}"", ReflectedField{{ ClassId(""{field.Type}""), 
            {field.DisplayName}Metadata,
            [](void* pHandler) {{ {getString}; }},
            [](void* pHandler, void* pValue) {{ {preChangeHandlerPtr} {setString}; {postChangeHandlerPtr} }}}});");
                }
                else
                {
                    string getString = $"return (void*)&(({entry.Name}*)pHandler)->{field.Name}";
                    if (field.MetadataSpecifiers.ContainsKey("get"))
                    {
                        getString = $"return (void*)(({entry.Name}*)pHandler)->{field.MetadataSpecifiers["get"]}()";
                    }

                    string setString = $"(({entry.Name}*)pHandler)->{field.Name} = *({field.Type}*)pValue";
                    if (field.MetadataSpecifiers.ContainsKey("set"))
                    {
                        setString = $"(({entry.Name}*)pHandler)->{field.MetadataSpecifiers["set"]}(*({field.Type}*)pValue)";
                    }

                    fileWriter.WriteLine($@"
        PushField(""{field.DisplayName}"", ReflectedField{{ ClassId(""{field.Type}""), 
            {field.DisplayName}Metadata,
            [](void* pHandler) {{ {getString}; }},
            [](void* pHandler, void* pValue) {{ {preChangeHandler} {setString}; {postChangeHandler} }}}});");
                }
            }

            if (entry.Parent != "None" && entry.Parent != null)
            {
                if (ProjectClasses.ContainsKey(entry.Parent))
                {
                    WriteFieldAccessors(ProjectClasses[entry.Parent], ref fileWriter);
                }
            }
        }

        void WriteEnumAccessors(ReflectedClass entry, ref StreamWriter fileWriter)
        {
            if (!entry.bIsEnum)
            {
                return;
            }

            int lastEnumValue = 0;
            foreach (var enumValue in entry.EnumValues)
            {
                if(enumValue.Value != -1)
                {
                    lastEnumValue = enumValue.Value;
                }
                fileWriter.WriteLine($"\t\tPushEnumValue(\"{enumValue.Key}\", {lastEnumValue});");
                lastEnumValue++;
            }
        }

        private void WriteSpecialCppClass(StreamWriter fileWriter, ReflectedClass entry)
        {
            foreach (var projectHeader in Headers)
            {
                if (entry.HeaderPath == projectHeader.Path)
                {
                    continue;
                }

                if (projectHeader.Classes.Count > 0)
                {
                    fileWriter.WriteLine($"#include \"{projectHeader.Path}\"");
                }
            }

            fileWriter.WriteLine(@"
namespace ke
{");
            fileWriter.WriteLine($@"
    void {entry.Name}::FillReflectionDatabaseEntries()
    {{");
            foreach (var projectHeader in Headers)
            {
                foreach (var headerClass in projectHeader.Classes.Values)
                {
                    if (headerClass.bIsSpecial)
                    {
                        continue;
                    }

                    fileWriter.WriteLine($"\t\tPushClass(ke::MakeRef(ke::New<R{headerClass.Name}>()));");
                }
            }
            fileWriter.WriteLine("\t}");

            foreach (var projectHeader in Headers)
            {
                foreach (var headerClass in projectHeader.Classes.Values)
                {
                    if (headerClass.bIsSpecial)
                    {
                        continue;
                    }

                    fileWriter.WriteLine($@"
    template<> RefPtr<ReflectedClass> {entry.Name}::GetClass<{headerClass.Name}>() 
    {{ 
        CHECK(m_Classes.Contains(ClassId(""{headerClass.Name}""))); 
        return m_Classes[ClassId(""{headerClass.Name}"")]; 
    }}");
                }
            }
            fileWriter.WriteLine("}");
        }

        private void GenerateHeaderFiles(ParsedHeader header)
        {
            if (header.Classes.Count > 0)
            {
                WriteHeaderFileCode(header);
                WriteHeaderFileImpl(header);
            }
        }

        private void GenerateHeaders()
        {
            if (!Directory.Exists(_generatedDirectory))
            {
                Directory.CreateDirectory(_generatedDirectory);
            }

            foreach (var header in Headers)
            {
                if(header.Classes.Count == 0)
                {
                    var generatedHeaderPath = $"{_generatedDirectory}/{Path.GetFileNameWithoutExtension(header.Path)}.gen.h";
                    var generatedCppPath = $"{_generatedDirectory}/{Path.GetFileNameWithoutExtension(header.Path)}.gen.cpp";
                    
                    if(File.Exists(generatedHeaderPath))
                    {
                        File.Delete(generatedHeaderPath);
                    }

                    if (File.Exists(generatedCppPath))
                    {
                        File.Delete(generatedCppPath);
                    }
                }

                bAnyFilesChanged |= _sourceDb.HasFileChanged(header.Path);
                if (_sourceDb.HasFileChanged(header.Path))
                {
                    GenerateHeaderFiles(header);
                }
            }

            if (bAnyFilesChanged)
            {
                foreach (var header in Headers)
                {
                    bool bHasSpecialClass = false;
                    foreach (var headerClass in header.Classes)
                    {
                        if (headerClass.Value.bIsSpecial)
                        {
                            bHasSpecialClass = true;
                            break;
                        }
                    }

                    if (bHasSpecialClass)
                    {
                        GenerateHeaderFiles(header);
                    }
                }
            }
        }
    }
}