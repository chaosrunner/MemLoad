using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.Loader;

class Program
{
    static HashSet<int> loggedProcesses = new HashSet<int>();

    static void Main(string[] args)
    {
        string eventLogName = "DotNetMon";
        string eventSourceName = "DLLMON";
        if (!EventLog.SourceExists(eventSourceName))
        {
            EventLog.CreateEventSource(eventSourceName, eventLogName);
        }

        while (true)
        {
            Process[] processes = Process.GetProcesses();
            foreach (Process process in processes)
            {
                try
                {
                    if (process.ProcessName == "taskhostw" || process.ProcessName.Contains("Agent") || process.ProcessName.Contains("ServiceHub") || process.ProcessName.Contains("MSBuild") || process.ProcessName.Contains("devenv") || process.ProcessName.Contains("webViewHost") || process.ProcessName.Contains("Compiler"))
                    {
                        // Ignore taskhostw and Azureagent processes that load mscore and clr.dll.
                        continue;
                    }

                    bool isDotNetProcess = false;
                    List<string> loadedDlls = new List<string>();
                    foreach (ProcessModule module in process.Modules)
                    {
                        string moduleName = module.ModuleName.ToLower();
                        loadedDlls.Add(moduleName);
                        if (moduleName == "clr.dll")
                        {
                            isDotNetProcess = true;
                        }
                    }

                    if (!isDotNetProcess)
                    {
                        continue;
                    }

                    if (!loggedProcesses.Contains(process.Id)) // Check if the process has been logged before.
                    {
                        loggedProcesses.Add(process.Id); // Add the process to the list of logged processes.

                        string message = string.Format("Process: {0} ({1})\nLoaded DLLs: {2}", process.ProcessName, process.Id, string.Join(", ", loadedDlls));
                        EventLog.WriteEntry(eventSourceName, message, EventLogEntryType.Information, 69);

                        // Hook the new .NET process and log its app domain.
                        process.EnableRaisingEvents = true; // Enable raising Exited event for the process.
                        process.Exited += (sender, e) =>
                        {
                            var assemblyLoadContext = AssemblyLoadContext.GetLoadContext(sender.GetType().Assembly);
                            string appDomainName = assemblyLoadContext.Name;
                            string exitMessage = string.Format("Process {0} ({1}) exited. App domain: {2}", process.ProcessName, process.Id, appDomainName);
                            EventLog.WriteEntry(eventSourceName, exitMessage, EventLogEntryType.Information, 420);
                        };
                    }
                }
                catch (Exception ex)
                {
                    continue;
                }
            }

            System.Threading.Thread.Sleep(TimeSpan.FromSeconds(30)); // Sleep for 30 seconds.
        }
    }
}
