using System;
using System.Diagnostics;
using System.IO;

namespace KECacheChecker
{
    public class Program
    {
        public static void Main(string[] args)
        {
            if(args.Length == 0)
            {
                Console.WriteLine("Error: First arg must be engine root");
                Environment.Exit(1);
            }

            bool bRunsEditor = false;
            if(args.Length > 1)
            {
                int value = int.Parse(args[1]);
                bRunsEditor = value == 1 ? true : false;
            }

            string engineRoot = args[0];
            string markFilePath = $"{engineRoot}/Intermediate/_regenerate";
            if (File.Exists(markFilePath))
            {
                // Regenerate cache
                string correctBat = bRunsEditor ? "GenerateEditorNotFresh" : "GenerateRuntimeNotFresh";
                var processInfo = new ProcessStartInfo("cmd.exe", "/c " + correctBat);
                processInfo.CreateNoWindow = true;
                processInfo.UseShellExecute = false;
                processInfo.RedirectStandardError = true;
                processInfo.RedirectStandardOutput = true;
                processInfo.WorkingDirectory = $"{args[0]}/Scripts";

                var process = Process.Start(processInfo);

                process.OutputDataReceived += (object sender, DataReceivedEventArgs e) =>
                    Console.WriteLine("output>>" + e.Data);
                process.BeginOutputReadLine();

                process.ErrorDataReceived += (object sender, DataReceivedEventArgs e) =>
                    Console.WriteLine("error>>" + e.Data);
                process.BeginErrorReadLine();

                process.WaitForExit();

                Console.WriteLine("ExitCode: {0}", process.ExitCode);
                process.Close();

                File.Delete(markFilePath);
                // Environment.Exit(1);
            }
        }
    }
}