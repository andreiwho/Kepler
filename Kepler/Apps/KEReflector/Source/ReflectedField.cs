using System;
using System.Collections.Generic;

namespace KEReflector
{
    public class ReflectedField
    {
        public string Name { get; set; }
        public string Type { get; set; }
        public string DisplayName { get; set; }
        public bool bIsPointer { get; set; } = false;
        public bool bIsRefPtr { get; set; } = false;
        public List<string> MetadataSpecifiers { get; set; } = new List<string>();
    }
}