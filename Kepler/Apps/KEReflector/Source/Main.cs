using System;
using System.Diagnostics;
using System.IO;

namespace KEReflector
{
    public class Program
    {
        public static Project ParseCommandLine(string[] args)
        {
            if (args.Length == 0)
            {
                Console.WriteLine("ERROR: The first argument always MUST be a path to the root of the project");
                Environment.Exit(1);
            }
            string ProjectRoot = args[0];
            if (args.Length < 2)
            {
                Console.WriteLine("ERROR: The second argument always MUST be a project name");
                Environment.Exit(1);
            }
            string ProjectName = args[1];
            Console.WriteLine($"Specified project root: {ProjectRoot}. Specified project name: {ProjectName}");
            Console.WriteLine();

            return new Project(ProjectName, $"{ProjectRoot}/{ProjectName}", ProjectRoot);
        }

        public static void Main(string[] args)
        {
            Console.WriteLine();
            Console.WriteLine($"Generating reflection info for directory {args[0]}");
            Project project = ParseCommandLine(args);
            project.ReadProjectFiles();

            if (project.bAnyFilesChanged)
            {
                RegenerateSolution(project);
            }
        }

        public static void RegenerateSolution(Project project)
        {
            string markFilePath = $"{project.EngineRoot}/Intermediate/_regenerate";
            if(File.Exists(markFilePath))
            {
                return;
            }
            else
            {
                File.CreateText(markFilePath).Flush();
                Console.WriteLine("NOTE: Project regeneration requested");
            }
        }
    }
}