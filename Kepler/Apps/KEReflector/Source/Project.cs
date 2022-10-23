using System;
using System.Collections.Generic;
using System.IO;

namespace KEReflector
{
    public class Project
    {
        public string Name { get; set; }
        public string ProjectDirectory { get; set; }
        public string EngineRoot { get; set; }
        public string SourceDir { get; set; }
        public List<ParsedHeader> Headers { get; set; }

        public Dictionary<string, ReflectedClass> ProjectClasses { get; set; } = new Dictionary<string, ReflectedClass>();

        public ParsedHeader SpecialFileHeader = null;

        public readonly string _generatedDirectory;

        public Project(string name, string directory, string engineRoot)
        {
            Name = name;
            ProjectDirectory = directory;
            EngineRoot = engineRoot;
            SourceDir = $"{ProjectDirectory}/Source";
            Headers = new List<ParsedHeader>();
            _generatedDirectory = $"{ProjectDirectory}/Generated";
        }

        Dictionary<string, ReflectedClass> CombineProjectClasses()
        {
            Dictionary<string, ReflectedClass> result = new Dictionary<string, ReflectedClass>();

            foreach(var header in Headers)
            {
                foreach(var headerClass in header.Classes)
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
                        Headers.Add(new ParsedHeader(entry.Replace('\\', '/')));
                    }
                }
            }
        }

        public void ReadProjectFiles()
        {
            ReadDirectory(SourceDir);
            ProjectClasses = CombineProjectClasses();
            GenerateHeaders();
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
                fileWriter.WriteLine("#include \"Reflection/Class.h\"\n\n");
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
                    fileWriter.Write($@"
    class {entry.Name};
    class R{entry.Name} : public ReflectedClass
    {{
    public:
        R{entry.Name}();
        virtual String GetName() const override {{ return ""{entry.Name}""; }}
        virtual bool HasParent() const override {{ return {hasParent}; }}
        virtual String GetParentName() const override {{ return ""{entryParent}""; }}
");
                    fileWriter.WriteLine(@"
    };");
                }

                fileWriter.WriteLine("}");
                fileWriter.Flush();
                fileWriter.Close();
            }
        }

        void WriteHeaderFileImpl(ParsedHeader header)
        {
            var fileName = Path.GetFileNameWithoutExtension(header.Path);
            var generatedHeaderPath = $"{_generatedDirectory}/{fileName}.gen.cpp";
            StreamWriter fileWriter = null;
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
                fileWriter.WriteLine($"#include \"{header.Path}\"");
                foreach (var entry in header.Classes.Values)
                {
                    if (entry.bIsSpecial)
                    {
                        WriteSpecialCppClass(fileWriter, entry);
                    }
                    else
                    {
                        fileWriter.WriteLine("namespace ke\n{");
                        fileWriter.WriteLine($"\tR{entry.Name}::R{entry.Name}()\n\t{{");

                        // Recursive parent fields
                        fileWriter.WriteLine($"m_ClassId = id64(\"{entry.Name}\");");
                        WriteFieldAccessors(entry, ref fileWriter);

                        fileWriter.WriteLine("\t}");
                        fileWriter.WriteLine("}");
                    }
                }

                fileWriter.Flush();
                fileWriter.Close();
            }
        }

        private void WriteFieldAccessors(ReflectedClass entry, ref StreamWriter fileWriter)
        {
            foreach (var field in entry.Fields)
            {
                fileWriter.WriteLine($@"
        PushField(""{field.Name}"", ReflectedField{{ id64(""{field.Type}""),
            [](void* pHandler) {{ return (void*)&(({entry.Name}*)pHandler)->{field.Name}; }},
            [](void* pHandler, void* pValue) {{ (({entry.Name}*)pHandler)->{field.Name} = *({field.Type}*)pValue; }}}});");
            }

            if(entry.Parent != "None" && entry.Parent != null)
            {
                if(ProjectClasses.ContainsKey(entry.Parent))
                {
                    WriteFieldAccessors(ProjectClasses[entry.Parent], ref fileWriter);
                }
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
        CHECK(m_Classes.Contains(id64(""{headerClass.Name}""))); 
        return m_Classes[id64(""{headerClass.Name}"")]; 
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
                GenerateHeaderFiles(header);
            }
        }
    }
}