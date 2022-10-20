using System;
using System.IO;
using System.Collections.Generic;

namespace KEReflector
{
    public class Header
    {
        public string Path { get; set; }

        public HashSet<ReflectedClass> Classes { get; }

        public Header(string path)
        {
            Path = path;
            Classes = ParseClasses();
        }

        private HashSet<ReflectedClass> ParseClasses()
        {
            HashSet<ReflectedClass> classes = new HashSet<ReflectedClass>();
            return classes;
        }
    }
}