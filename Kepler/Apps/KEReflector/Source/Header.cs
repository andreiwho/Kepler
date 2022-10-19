using System;
using System.IO;
using System.Collections.Generic;

namespace KEReflector
{
    public class Header
    {
        public string Path { get; set; }

        public HashSet<Class> Classes { get; }

        private char[] _separators = new char[]
        {
            ' ',
            ',',
        };

        public Header(string path)
        {
            Path = path;
            Classes = ParseClasses();
        }

        private HashSet<Class> ParseClasses()
        {
            HashSet<Class> classes = new HashSet<Class>();
            var tokens = File.ReadAllText(Path).Trim().Split(_separators);
            for(int index = 0; index < tokens.Length; ++index)
            {
                var token = tokens[index];
                if(token == "reflected")
                {
                    if(tokens.Length > index + 1)
                    {
                        Console.WriteLine(tokens[index + 2]);
                    }
                    else
                    {
                        Console.WriteLine(tokens.Length);
                    }
                }
            }

            return classes;
        }
    }
}