using System.Collections.Generic;
using System.Diagnostics;
using SharpestInjector;
using System.Windows;
using System.Linq;
using System.IO;
using System;

namespace NoFocusLossGUI
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

            Dll32 = PeFile.Parse("NoFocusLoss.dll");
            Dll64 = PeFile.Parse("NoFocusLoss64.dll");
        }

        public List<ProcessInfo> ProcessBindTest = new List<ProcessInfo>();

        PeFile Dll32;
        PeFile Dll64;

        private void Refresh(object sender, RoutedEventArgs e)
        {
            ProcessBindTest.Clear();

            var strings = new List<string>();
            foreach (var process in Process.GetProcesses())
            {
                var proc = Injector.GetProcessInfo(process);

                if (proc.Modules.Count == 0 || proc.WindowHandle == IntPtr.Zero)
                    continue;                

                string fileName = Path.GetFileName(proc.Modules.First().Value.Path);
                proc.FileName = fileName;

                ProcessBindTest.Add(proc);
            }

            ProcessBindTest = ProcessBindTest.OrderBy(x => x.ToString(), StringComparer.OrdinalIgnoreCase).ToList(); // I tried using data bindings, but MVVM just sucks
            foreach(var proc in ProcessBindTest)
            {
                Processes.Items.Add(proc);
            }
        }

        private void Inject(object sender, RoutedEventArgs e)
        {
            var selected = Processes.SelectedItem as ProcessInfo;

            if (selected == null)
                return;

            PeFile dll = Dll64;

            if (selected.Is64Bit == false)
            {
                dll = Dll32;
            }
            
            Injector.Inject(selected, dll);

            Processes.Items.Remove(selected);
            InjectedProcesses.Items.Add(selected);
        }

        private void Unload(object sender, RoutedEventArgs e)
        {
            var selected = InjectedProcesses.SelectedItem as ProcessInfo;

            if (selected == null)
                return;

            PeFile dll = Dll64;

            if (selected.Is64Bit == false)
            {
                dll = Dll32;
            }

            Injector.Unload(selected, dll);

            InjectedProcesses.Items.Remove(selected);
            Processes.Items.Add(selected);
        }
    }
}
