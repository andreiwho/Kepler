using System;
using System.Collections.Generic;
using System.IO;
using System.Text.Json;
using System.Text.Json.Serialization;

namespace KEReflector
{
    public class DatabaseEntry
    {
        public DateTime ChangeDate { get; set; }
        public bool bWasChanged { get; set; } = false;
    }

    public class SourceDatabase
    {
        public string DatabasePath { get; set; }

        public Dictionary<string, DateTime> DatabaseEntries { get; set; } = new();
        public Dictionary<string, DateTime> NewDatabaseEntries { get; set; } = new();

        public SourceDatabase(string databasePath)
        {
            DatabasePath = $"{databasePath}/Headers.gen.json";
            ReadDatabaseEntries();
        }

        public void AddEntry(string path)
        {
            NewDatabaseEntries.Add(path, File.GetLastWriteTime(path));
        }

        void ReadDatabaseEntries()
        {
            if (!File.Exists(DatabasePath))
            {
                return;
            }
            var json = File.ReadAllText(DatabasePath);
            DatabaseEntries = JsonSerializer.Deserialize<Dictionary<string, DateTime>>(json);
        }

        public bool HasFileChanged(string path)
        {
            if(DatabaseEntries.ContainsKey(path) && NewDatabaseEntries.ContainsKey(path))
            {
                return NewDatabaseEntries[path] > DatabaseEntries[path];
            }
            return true;
        }

        public void FlushDatabase()
        {
            if (DatabaseEntries != null)
            {
                using StreamWriter fileWriter = File.Exists(DatabasePath) ? new StreamWriter(DatabasePath) : File.CreateText(DatabasePath);
                string json = JsonSerializer.Serialize(NewDatabaseEntries, new JsonSerializerOptions { WriteIndented = true });
                fileWriter.Write(json);
            }
        }
    }
}