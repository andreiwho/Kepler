using System;
using System.IO;

namespace KEReflector
{
    public class ReflectedClass
    {   
        public string Name { get; set; }
        public string Parent { get; set; }

        public ReflectedClass(string name)
        {
            Name = name;
        }
    }
}