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

        public List<Header> Headers { get; set; }

        public readonly string _generatedDirectory;

        public Project(string name, string directory, string engineRoot)
        {
            Name = name;
            ProjectDirectory = directory;
            EngineRoot = engineRoot;
            SourceDir = $"{ProjectDirectory}/Source";
            Headers = new List<Header>();
            _generatedDirectory = $"{ProjectDirectory}/Generated";
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
                        Headers.Add(new Header(entry.Replace('\\', '/')));
                    }
                }
            }
        }

        public void ReadProjectFiles()
        {
            ReadDirectory(SourceDir);
            GenerateHeaders();
        }

        private void GenerateHeaders()
        { 
            if(!Directory.Exists(_generatedDirectory))
            {
                Directory.CreateDirectory(_generatedDirectory);
            }

            foreach(var header in Headers)
            {
                if(header.Classes.Count > 0)
                {
                    var fileName = Path.GetFileNameWithoutExtension(header.Path);
                    var generatedFilePath = $"{_generatedDirectory}/{fileName}.gen.h";
                    StreamWriter fileWriter = null;
                    if (!File.Exists(generatedFilePath))
                    {
                        var writer = File.CreateText(generatedFilePath);
                    }
                    else
                    {
                        fileWriter = new StreamWriter(generatedFilePath);
                    }

                    if(fileWriter != null)
                    {
                        fileWriter.WriteLine("#pragma once");
                        fileWriter.WriteLine("#include \"Reflection/Class.h\"\n\n");
                        fileWriter.WriteLine("namespace ke");
                        fileWriter.WriteLine("{");

                        foreach(var entry in header.Classes)
                        {
                            fileWriter.Write($@"
    class {entry.Name};
    class R{entry.Name} : public ReflectedClass
    {{
    public:
        static R{entry.Name}& Get() {{ static R{entry.Name} This; return This; }}
        virtual String GetName() const override {{ return ""{entry.Name}""; }}");
                            foreach(var field in entry.Fields)
                            {
                                fileWriter.WriteLine($@"
        {field.Type}({entry.Name}::*{field.Name});");
                            }

                            fileWriter.WriteLine(@"
    };");
                        }

                        fileWriter.WriteLine("}");
                        fileWriter.Flush();
                        fileWriter.Close();
                    }
                }
            }
        }
    }
}