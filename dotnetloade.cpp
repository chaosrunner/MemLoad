using System;
using System.Management;
using System.Diagnostics;

class Program
{
    static void Main(string[] args)
    {
        // Create a new process watcher
        var processWatcher = new ManagementEventWatcher(new WqlEventQuery("SELECT * FROM Win32_ProcessStartTrace"));

        // Attach an event handler for new process starts
        processWatcher.EventArrived += ProcessWatcher_EventArrived;

        // Start watching for process starts
        processWatcher.Start();

        Console.WriteLine("Waiting for .NET processes to start...");

        // Wait for user input to exit
        Console.ReadLine();

        // Stop watching for process starts
        processWatcher.Stop();
    }

    private static void ProcessWatcher_EventArrived(object sender, EventArrivedEventArgs e)
    {
        // Get the process name and module names
        string processName = e.NewEvent.Properties["ProcessName"].Value.ToString();
        string moduleName = e.NewEvent.Properties["ModuleName"].Value.ToString();

        // Check if the process has loaded the CLR.dll and mscorlib.dll modules
        if (moduleName.Equals("clr.dll", StringComparison.OrdinalIgnoreCase) ||
            moduleName.Equals("mscorlib.dll", StringComparison.OrdinalIgnoreCase))
        {
            Console.WriteLine("DOT NET FOUND in process: " + processName);
        }
    }
}
