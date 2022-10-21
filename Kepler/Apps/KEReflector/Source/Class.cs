using System;
using System.Collections.Generic;
using System.IO;

namespace KEReflector
{
    public class ReflectedClass
    {   
        public string Name { get; set; }
        public string Parent { get; set; }

        public List<ReflectedField> Fields { get; set; }

        public ReflectedClass(string name, string parent)
        {
            Name = name;
            Parent = parent;

            Fields = new List<ReflectedField>();
        }
    }
}