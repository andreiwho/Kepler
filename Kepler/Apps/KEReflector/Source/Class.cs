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
        public bool bIsEnum { get; set; } = false;
        public string HeaderPath { get; set; }
        public List<ReflectedField> Fields { get; set; }
        public Dictionary<string, int> EnumValues { get; set; } = new();
        public Dictionary<string, string> MetadataSpecifiers { get; set; } = new();
        public int ReflLine { get; set; } = 0;


        public ReflectedClass(string name, string parent)
        {
            Name = name;
            Parent = parent;

            Fields = new List<ReflectedField>();
        }
    }
}