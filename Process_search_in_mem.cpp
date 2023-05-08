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

                // Get all AppDomains for the process
                AppDomain[] appDomains = process.AppDomains;

                foreach (AppDomain appDomain in appDomains)
                {
                    // Get the loaded assemblies for the AppDomain
                    Assembly[] assemblies = appDomain.GetAssemblies();

                    foreach (Assembly assembly in assemblies)
                    {
                        // Check if the assembly name contains "mscore" or "clr"
                        if (assembly.FullName.Contains("mscore") || assembly.FullName.Contains("clr"))
                        {
                            // Log the loaded assembly to a file
                            string logMessage = $"{DateTime.Now}: {assembly.FullName} loaded in {appDomain.FriendlyName} ({process.ProcessName}, {process.Id})";
                            LogToFile(logMessage);
                        }
                    }
                }

                // Get all modules in memory for the process
                ProcessModuleCollection modulesInMemory = process.Modules;

                foreach (ProcessModule moduleInMemory in modulesInMemory)
                {
                    // Check if the module name contains "mscore" or "clr"
                    if (moduleInMemory.ModuleName.Contains("mscore") || moduleInMemory.ModuleName.Contains("clr"))
                    {
                        // Log the loaded module to a file
                        string logMessage = $"{DateTime.Now}: {moduleInMemory.ModuleName} loaded in memory for {process.ProcessName} ({process.Id})";
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
