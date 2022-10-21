using System;
using System.IO;
using System.Collections.Generic;
using System.Text;

namespace KEReflector
{
    public enum ETokenKind
    {
        Unknown,
        ClassDecl,
        ParentDecl,
        FieldDecl,
    }

    public struct ReflectionToken
    {
        public string Name;
        public string Type;
        public ETokenKind Kind;
    }

    public class Header
    {
        public string Path { get; set; }

        public HashSet<ReflectedClass> Classes { get; }

        public Header(string path)
        {
            Path = path;
            Classes = ParseClasses();

            foreach(var c in Classes)
            {
                if(c.Parent != null)
                {
                    Console.WriteLine($"Class name: {c.Name} has parent {c.Parent}");
                }
                else
                {
                    Console.WriteLine($"Class name: {c.Name}");
                }
            }
        }

        private static char[] _specialChars = new char[]
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

        List<ReflectionToken> ParseTokens(List<string> tokens)
        {
            List<ReflectionToken> result = new List<ReflectionToken>();

            bool bReflectedValue = false;
            bool bPrevTokenWasClass = false;
            ETokenKind tokenKind = ETokenKind.Unknown;
            foreach (var token in tokens)
            {
                if (token == "reflected")
                {
                    bReflectedValue = true;
                    continue;
                }

                if (token == "class" && bReflectedValue)
                {
                    tokenKind = ETokenKind.ClassDecl;
                    continue;
                }

                if (bReflectedValue)
                {
                    switch (tokenKind)
                    {
                        case ETokenKind.ClassDecl:
                            result.Add(new ReflectionToken
                            {
                                Name = token,
                                Kind = tokenKind
                            });
                            bPrevTokenWasClass = true;
                            tokenKind = ETokenKind.Unknown;
                            break;
                        case ETokenKind.ParentDecl:
                            if(token == "public" || token == "private" || token == "protected" || token == "virtual")
                            {
                                continue;
                            }
                            result.Add(new ReflectionToken
                            {
                                Name = token,
                                Kind = tokenKind
                            });
                            tokenKind = ETokenKind.Unknown;
                            break;
                        default:
                            break;
                    }
                }

                foreach (char c in _specialChars)
                {
                    if (token == c.ToString())
                    {
                        if(token == ":" && bPrevTokenWasClass)
                        {
                            tokenKind = ETokenKind.ParentDecl;
                            goto end;
                        }

                        bReflectedValue = false;
                    }
                end:
                    ;
                }
            }
            return result;
        }

        private HashSet<ReflectedClass> ParseClasses()
        {
            HashSet<ReflectedClass> classes = new HashSet<ReflectedClass>();
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
                switch (token.Kind)
                {
                    case ETokenKind.ClassDecl:
                        if(currentClass != null)
                        {
                            classes.Add(currentClass);
                        }
                        currentClass = new ReflectedClass(token.Name);
                        break;
                    case ETokenKind.ParentDecl:
                        if(currentClass != null)
                        {
                            currentClass.Parent = token.Name;
                            classes.Add(currentClass);
                            currentClass = null;
                        }
                        break;
                    default:
                        currentClass = null;
                        break;
                }
            }

            if(currentClass != null)
            {
                classes.Add(currentClass);
            }

            return classes;
        }
    }
}