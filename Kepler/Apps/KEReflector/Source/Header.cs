using System;
using System.IO;
using System.Collections.Generic;
using System.Text;

namespace KEReflector
{
    public class ParsedToken
    {
        public string Name { get; set; }
        public string Type { get; set; }
        public string Parent { get; set; }
        public bool bIsSpecial { get; set; } = false;
        public bool bIsPointer { get; set; } = false;
        public bool bIsRefPtr { get; set; } = false;
        public bool bIsEnum { get; set; } = false;
        public bool bIsEnumClass { get; set; } = false;
        // Change to Dictionary, to be able to pass values after '='
        public Dictionary<string, string> MetaSpecifiers { get; set; } = new();
        public Dictionary<string, int> EnumEntries { get; set; } = new();
    }

    public class ParsedHeader
    {
        public string Path { get; set; }

        public Dictionary<string, ReflectedClass> Classes { get; }

        public ParsedHeader(string path)
        {
            Path = path;
            Classes = ParseClasses();
        }

        private static readonly char[] _specialChars = new char[]
        {
            ',',
            ';',
            '<',
            '>',
            '{',
            '}',
            ':',
            '[',
            ']',
            '+',
            '-',
            '*',
            '/',
            '(',
            ')',
            '=',
            '"'
        };

        void TokenizeLine(string line, ref List<string> tokens)
        {
            line = line.Trim();
            StringBuilder builder = new StringBuilder();

            if (line.StartsWith("//") || line.StartsWith("/*") || line.StartsWith("#"))
            {
                return;
            }

            foreach (char c in line)
            {
                // Whitespace
                if (c == ' ' || c == '\r' || c == '\n')
                {
                    if (builder.Length > 0)
                    {
                        tokens.Add(builder.ToString().Trim());
                        builder.Clear();
                    }
                    continue;
                }

                foreach (var specialChar in _specialChars)
                {
                    if (c == specialChar)
                    {
                        if (builder.Length > 0)
                        {
                            tokens.Add(builder.ToString().Trim());
                            builder.Clear();
                        }

                        tokens.Add(c.ToString());
                        goto end;
                    }
                }

                builder.Append(c);
            end:
                ;
            }

            if (builder.Length > 0)
            {
                tokens.Add(builder.ToString().Trim());
                builder.Clear();
            }
        }

        enum EParseStage
        {
            None,
            ParseMeta,
            ParseMetaArgs,
            ParseMetaValue,
            ParseTemplateWrapper,
            ParseType,
            ParsePointer,
            ParseName,
            ParseEnumValues,
            ParseEnumValueNumber,
            CheckHasParent,
            ParseParent,
            Finished,
        }

        List<ParsedToken> ParseTokens(List<string> tokens)
        {
            List<ParsedToken> result = new List<ParsedToken>();
            EParseStage currentStage = EParseStage.None;
            ParsedToken currentToken = null;
            KeyValuePair<string, string> currentMetaSpecifier = new();
            KeyValuePair<string, int> currentEnumValue = new();

            foreach (var token in tokens)
            {
            startagain:
                switch (currentStage)
                {
                    case EParseStage.None:
                        if (token == "reflected")
                        {
                            if (currentToken != null)
                            {
                                result.Add(currentToken);
                            }
                            currentToken = new ParsedToken();
                            currentToken.bIsSpecial = false;
                            currentStage = EParseStage.ParseMeta;
                            continue;
                        }
                        else if (token == "holding_reflection_data")
                        {
                            if (currentToken != null)
                            {
                                result.Add(currentToken);
                            }
                            currentToken = new ParsedToken();
                            currentToken.bIsSpecial = true;
                            currentStage = EParseStage.ParseType;
                            continue;
                        }
                        break;
                    case EParseStage.ParseMeta:
                        if (token == "kmeta")
                        {
                            currentStage = EParseStage.ParseMetaArgs;
                        }
                        else
                        {
                            currentStage = EParseStage.ParseType;
                            goto startagain;
                        }
                        continue;
                    case EParseStage.ParseMetaArgs:
                        if (token == "(" || token == ",")
                        {
                            continue;
                        }

                        if (token == ")")
                        {
                            currentStage = EParseStage.ParseType;
                            continue;
                        }

                        if (token == "=")
                        {
                            currentStage = EParseStage.ParseMetaValue;
                            continue;
                        }

                        if (currentToken != null)
                        {
                            currentMetaSpecifier = new(token.ToLower(), "");
                            currentToken.MetaSpecifiers.Add(token.ToLower(), "");
                        }
                        break;
                    case EParseStage.ParseMetaValue:
                        if(currentMetaSpecifier.Key.Length > 0)
                        {
                            currentMetaSpecifier = new(currentMetaSpecifier.Key, token);
                            if(currentToken.MetaSpecifiers.ContainsKey(currentMetaSpecifier.Key))
                            {
                                currentToken.MetaSpecifiers[currentMetaSpecifier.Key] = currentMetaSpecifier.Value;
                            }
                            else
                            {
                                currentToken.MetaSpecifiers.Add(currentMetaSpecifier.Key, currentMetaSpecifier.Value);
                            }
                        }
                        currentStage = EParseStage.ParseMetaArgs;
                        break;
                    case EParseStage.ParseType:
                        if (token == "RefPtr")
                        {
                            currentToken.bIsRefPtr = true;
                            currentToken.bIsPointer = true;
                            currentStage = EParseStage.ParseTemplateWrapper;
                            continue;
                        }

                        if (token == "enum")
                        {
                            currentToken.bIsEnum = true;
                            continue;
                        }

                        if (currentToken.bIsEnum && token == "class")
                        {
                            currentStage = EParseStage.ParseName;
                            continue;
                        }

                        currentToken.Type = token;
                        currentStage = EParseStage.ParseName;
                        break;
                    case EParseStage.ParseTemplateWrapper:
                        if (token == "<")
                        {
                            continue;
                        }

                        if (token == ">")
                        {
                            currentStage = EParseStage.ParseName;
                            continue;
                        }

                        currentToken.Type = token;
                        break;
                    case EParseStage.ParseName:
                        if (token == "*")
                        {
                            currentToken.bIsPointer = true;
                            continue;
                        }
                        currentToken.Name = token;

                        if (currentToken.bIsEnum)
                        {
                            currentStage = EParseStage.ParseEnumValues;
                            continue;
                        }

                        currentStage = EParseStage.CheckHasParent;
                        break;
                    case EParseStage.CheckHasParent:
                        foreach (char c in _specialChars)
                        {
                            if (token == c.ToString())
                            {
                                if (token == ":")
                                {
                                    currentStage = EParseStage.ParseParent;
                                    goto end;
                                }
                                currentStage = EParseStage.None;
                                goto end;
                            }
                        }
                        break;
                    case EParseStage.ParseParent:
                        if (currentStage == EParseStage.ParseParent)
                        {
                            if (token == "public" || token == "private" || token == "protected" || token == "virtual")
                            {
                                continue;
                            }

                            if (currentToken.bIsEnum || currentToken.bIsEnumClass)
                            {
                                currentStage = EParseStage.ParseEnumValues;
                                continue;
                            }

                            // TODO: Do we need to contain enum parents?
                            currentToken.Parent = token;
                            currentStage = EParseStage.None;
                            result.Add(currentToken);
                            currentToken = null;
                        }
                        break;
                    case EParseStage.ParseEnumValues:
                        if (token == "{")
                        {
                            continue;
                        }

                        if (token == ",")
                        {
                            if(currentEnumValue.Key != null)
                            {
                                currentToken.EnumEntries.Add(currentEnumValue.Key, currentEnumValue.Value);
                                currentEnumValue = new();
                            }
                            continue;
                        }

                        if(token == "=")
                        {
                            currentStage = EParseStage.ParseEnumValueNumber;
                            continue;
                        }

                        if (token == "}")
                        {
                            if (currentEnumValue.Key != null)
                            {
                                currentToken.EnumEntries.Add(currentEnumValue.Key, currentEnumValue.Value);
                                currentEnumValue = new();
                            }
                            currentStage = EParseStage.None;
                            result.Add(currentToken);
                            currentToken = null;
                            continue;
                        }

                        currentEnumValue = new(token, -1);

                        break;
                    case EParseStage.ParseEnumValueNumber:
                        int value;
                        if(int.TryParse(token, out value))
                        {
                            if(currentEnumValue.Key != null)
                            {
                                currentEnumValue = new(currentEnumValue.Key, value);
                                currentStage = EParseStage.ParseEnumValues;
                                continue;
                            }
                        }
                        break;
                }
            end:
                ;
            }

            if (currentToken != null)
            {
                result.Add(currentToken);
            }

            return result;
        }

        private Dictionary<string, ReflectedClass> ParseClasses()
        {
            Dictionary<string, ReflectedClass> classes = new Dictionary<string, ReflectedClass>();
            List<string> tokens = new List<string>();

            var lines = File.ReadAllLines(Path);
            foreach (var line in lines)
            {
                TokenizeLine(line, ref tokens);
            }

            var reflectedTokens = ParseTokens(tokens);
            ReflectedClass currentClass = null;
            foreach (var token in reflectedTokens)
            {
                if (token.Type == "class" || token.Type == "struct" || token.bIsEnum || token.bIsEnumClass)
                {
                    if (currentClass != null)
                    {
                        classes.Add(currentClass.Name, currentClass);
                    }

                    currentClass = new ReflectedClass(token.Name, token.Parent)
                    {
                        bIsSpecial = token.bIsSpecial,
                        Type = token.Type,
                        HeaderPath = Path,
                        MetadataSpecifiers = token.MetaSpecifiers,
                        bIsEnum = token.bIsEnum || token.bIsEnumClass,
                        EnumValues = token.EnumEntries
                    };
                }
                else
                {
                    if (currentClass != null)
                    {
                        string DisplayName = "";
                        if (token.Name.StartsWith("m_"))
                        {
                            DisplayName = token.Name.Substring(2);
                        }
                        else if (token.Name.StartsWith("b") || token.Name.StartsWith("_"))
                        {
                            DisplayName = token.Name.Substring(1);
                        }
                        else
                        {
                            DisplayName = token.Name;
                        }

                        currentClass.Fields.Add(new ReflectedField
                        {
                            Name = token.Name,
                            Type = token.Type,
                            DisplayName = DisplayName,
                            MetadataSpecifiers = token.MetaSpecifiers,
                            bIsPointer = token.bIsPointer,
                            bIsRefPtr = token.bIsRefPtr,
                            bIsEnum = token.bIsEnum,
                        });
                    }
                }
            }

            if (currentClass != null)
            {
                classes.Add(currentClass.Name, currentClass);
            }

            return classes;
        }
    }
}