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
        public bool bIsEnum { get; set; } = false;
        public bool bIsValueSemanticType { get; set; } = false;
        public Dictionary<string, string> MetadataSpecifiers { get; set; } = new();
    }
}