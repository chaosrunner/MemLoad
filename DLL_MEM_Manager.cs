using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;

class Program
{
    [DllImport("Dbghelp.dll")]
    static extern bool MiniDumpWriteDump(IntPtr hProcess, uint processId, IntPtr hFile, uint dumpType, IntPtr exceptionParam, IntPtr userStreamParam, IntPtr callbackParam);

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

                    string message = string.Format("Process: {0} ({1})\nLoaded DLLs: {2}", process.ProcessName, process.Id, string.Join(", ", loadedDlls));
                    EventLog.WriteEntry(eventSourceName, message, EventLogEntryType.Information, 69);

                    using (FileStream fs = new FileStream(string.Format("C:\\{0}.snap", process.ProcessName), FileMode.Create))
                    {
                        if (!MiniDumpWriteDump(process.Handle, (uint)process.Id, fs.SafeFileHandle.DangerousGetHandle(), 2, IntPtr.Zero, IntPtr.Zero, IntPtr.Zero))
                        {
                            Console.WriteLine("Failed to create memory dump");
                        }
                        else
                        {
                            Console.WriteLine("Memory dump created successfully");
                        }
                    }
                }
                catch (Exception ex)
                {
                    string message = string.Format("Error occurred while processing {0} ({1}): {2}", process.ProcessName, process.Id, ex.Message);
                    EventLog.WriteEntry(eventSourceName, message, EventLogEntryType.Error, 1001);
                }
            }

            System.Threading.Thread.Sleep(TimeSpan.FromSeconds(30)); // Sleep for 30 seconds.
        }
    }
}
