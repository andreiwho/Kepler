using System;
using System.Collections.Generic;
using System.IO;

namespace KEReflector
{
    public class ReflectedClass
    {   
        public string Name { get; set; }
        public string Type { get; set; }
        public string Parent { get; set; }
        public bool bIsSpecial { get; set; } = false;
        public string HeaderPath { get; set; }
        public List<ReflectedField> Fields { get; set; }
        public List<string> MetadataSpecifiers { get; set; } = new List<string>();

        public ReflectedClass(string name, string parent)
        {
            Name = name;
            Parent = parent;

            Fields = new List<ReflectedField>();
        }
    }
}