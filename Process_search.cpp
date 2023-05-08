using System;
using System.ServiceProcess;
using System.Diagnostics;
using System.Threading;
using System.IO;

namespace MyWindowsService
{
    public partial class MyService : ServiceBase
    {
        private Thread monitoringThread;
        private bool isMonitoring;

        public MyService()
        {
            InitializeComponent();
        }

        protected override void OnStart(string[] args)
        {
            // Start the monitoring thread
            isMonitoring = true;
            monitoringThread = new Thread(MonitorLoadedModules);
            monitoringThread.Start();
        }

        protected override void OnStop()
        {
            // Stop the monitoring thread
            isMonitoring = false;
            monitoringThread.Join();
        }

        private void MonitorLoadedModules()
        {
            while (isMonitoring)
            {
                // Get all running processes
                Process[] processes = Process.GetProcesses();

                foreach (Process process in processes)
                {
                    try
                    {
                        // Get the loaded modules for the process
                        ProcessModuleCollection modules = process.Modules;

                        foreach (ProcessModule module in modules)
                        {
                            // Check if the module name contains "mscore" or "clr"
                            if (module.ModuleName.Contains("mscore") || module.ModuleName.Contains("clr"))
                            {
                                // Log the loaded module to a file
                                string logMessage = $"{DateTime.Now}: {module.ModuleName} loaded in {process.ProcessName} ({process.Id})";
                                LogToFile(logMessage);
                            }
                        }
                    }
                    catch (Exception ex)
                    {
                        // Handle any exceptions that occur while getting the loaded modules
                        string errorMessage = $"{DateTime.Now}: Error getting loaded modules for {process.ProcessName} ({process.Id}): {ex.Message}";
                        LogToFile(errorMessage);
                    }
                }

                // Sleep for 5 seconds before checking again
                Thread.Sleep(5000);
            }
        }

        private void LogToFile(string message)
        {
            // Log the message to a file named "monitor.log"
            string logPath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "monitor.log");
            using (StreamWriter writer = new StreamWriter(logPath, true))
            {
                writer.WriteLine(message);
            }
        }
    }
}
