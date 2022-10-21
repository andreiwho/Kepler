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
    }

    public class ParsedHeader
    {
        public string Path { get; set; }

        public HashSet<ReflectedClass> Classes { get; }

        public ParsedHeader(string path)
        {
            Path = path;
            Classes = ParseClasses();

            foreach (var c in Classes)
            {
                if (c.Parent != null)
                {
                    Console.WriteLine($"Class name: {c.Name} has parent {c.Parent}");
                }
                else
                {
                    Console.WriteLine($"Class name: {c.Name}");
                }

                foreach(var field in c.Fields)
                {
                    Console.WriteLine($"Class {c.Name} has field {field.Name} of type {field.Type}");
                }
            }
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
            ParseType,
            ParseName,
            CheckHasParent,
            ParseParent,
            Finished,
        }

        List<ParsedToken> ParseTokens(List<string> tokens)
        {
            List<ParsedToken> result = new List<ParsedToken>();
            EParseStage currentStage = EParseStage.None;
            ParsedToken currentToken = null;

            foreach (var token in tokens)
            {
                switch(currentStage)
                {
                    case EParseStage.None:
                        if(token == "reflected")
                        {
                            if (currentToken != null)
                            {
                                result.Add(currentToken);
                            }
                            currentToken = new ParsedToken();
                            currentToken.bIsSpecial = false;
                            currentStage = EParseStage.ParseType;
                            continue;
                        }
                        else if(token == "holding_reflection_data")
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
                    case EParseStage.ParseType:
                        currentToken.Type = token;
                        currentStage = EParseStage.ParseName;
                        break;
                    case EParseStage.ParseName:
                        currentToken.Name = token;
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
                            currentToken.Parent = token;
                            currentStage = EParseStage.None;
                            result.Add(currentToken);
                            currentToken = null;
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
                if(token.Type == "class" || token.Type == "struct")
                {
                    if (currentClass != null)
                    {
                        classes.Add(currentClass);
                    }
                    currentClass = new ReflectedClass(token.Name, token.Parent);
                    currentClass.bIsSpecial = token.bIsSpecial;
                    currentClass.Type = token.Type;
                    currentClass.HeaderPath = Path;
                }
                else
                {
                    if(currentClass != null)
                    {
                        currentClass.Fields.Add(new ReflectedField
                        {
                            Name = token.Name,
                            Type = token.Type,
                        });
                    }
                }
            }

            if (currentClass != null)
            {
                classes.Add(currentClass);
            }

            return classes;
        }
    }
}