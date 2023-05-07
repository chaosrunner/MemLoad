using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;

class Program
{
    static void Main(string[] args)
    {
        List<string> dllNames = new List<string> { "example.dll", "other.dll" }; // replace with the names of the DLLs you want to check
        List<string> ignoredProcesses = new List<string> { "svchost.exe", "taskmgr.exe" }; // replace with the process names to ignore

        Process[] processes = Process.GetProcesses();
        foreach (Process process in processes)
        {
            try
            {
                if (ignoredProcesses.Contains(process.ProcessName.ToLower()))
                {
                    continue; // ignore this process
                }

                // Use lazy loading to get module information only when necessary
                foreach (ProcessModule module in new Lazy<ProcessModuleCollection>(() => process.Modules))
                {
                    string fileName = module.FileName.ToLower();
                    foreach (string dllName in dllNames)
                    {
                        if (fileName.Contains(dllName.ToLower()) && !fileName.Contains("system32"))
                        {
                            Console.WriteLine($"DLL '{dllName}' is loaded into process '{process.ProcessName}' with process ID {process.Id}.");

                            // create a sub process with the same name as the process using the DLL
                            ProcessStartInfo startInfo = new ProcessStartInfo(process.ProcessName + ".exe");
                            startInfo.UseShellExecute = false;

                            // start the sub process and load the same DLL
                            Process subProcess = Process.Start(startInfo);
                            if (subProcess != null)
                            {
                                subProcess.EnableRaisingEvents = true;
                                subProcess.Exited += (sender, e) =>
                                {
                                    // handle sub process exit event
                                };

                                subProcess.Exited += (sender, e) =>
                                {
                                    // handle sub process exit event
                                };

                                ProcessModule subModule = subProcess.Modules.Cast<ProcessModule>()
                                    .FirstOrDefault(m => m.FileName.ToLower().Contains(dllName.ToLower()) && !m.FileName.ToLower().Contains("system32"));
                                if (subModule != null)
                                {
                                    Console.WriteLine($"DLL '{dllName}' is loaded into sub process '{subProcess.ProcessName}' with process ID {subProcess.Id}.");
                                }
                            }
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                // handle exception if any
            }
        }

        Console.ReadLine();
    }
}
