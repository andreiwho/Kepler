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

        public Project(string name, string directory, string engineRoot)
        {
            Name = name;
            ProjectDirectory = directory;
            EngineRoot = engineRoot;
            SourceDir = $"{ProjectDirectory}/Source";
            Headers = new List<Header>();
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
                        Headers.Add(new Header(entry));
                    }
                }
            }
        }

        public void ReadProjectFiles()
        {
            ReadDirectory(SourceDir);
        }
    }
}