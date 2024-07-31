/*
Xplorer - A real-time editor for the Oberheim Xpander and Matrix-12 synths
Copyright (C) 2012-2024 Pascal Schmitt

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

using MidiApp.MidiController.Service;
using MidiApp.MidiController.View;
using System;
using System.Reflection;
using System.Threading;
using System.Windows.Forms;
using Xplorer.Controller.Service.Settings;
using Xplorer.View;

namespace Xplorer
{
    /// <summary>
    /// Application main class
    /// </summary>
    internal static class Program
    {
        private static SplashScreenForm _splashScreenForm;

        /// <summary>
        /// Splash form thread
        /// </summary>
        private static void SplashThread()
        {
            Application.Run(_splashScreenForm);
        }

        /// <summary>
        /// main entry point
        /// </summary>
        /// <param name="args">The args.</param>
        [STAThread]
        private static void Main(string[] args)
        {
            // command line argument
            const string CLI_RESET = "/reset";

            //avoid multiple instances
            using (FileMutex fileMutex = new FileMutex(Application.CommonAppDataPath, "xplorer.~lock"))
            {
                if (!fileMutex.IsLocked())
                {
                    fileMutex.Lock();
                    Application.EnableVisualStyles();
                    Application.SetCompatibleTextRenderingDefault(false);

                    //Add the event handler for handling UI thread exceptions to the event.

                    //#if !DEBUG
                    Application.ThreadException += new System.Threading.ThreadExceptionEventHandler(TopLevelExceptionHandler.UIThreadExceptionHandler);
                    //Set the unhandled exception mode to force all Windows Forms errors to go through our handler.
                    Application.SetUnhandledExceptionMode(UnhandledExceptionMode.CatchException);
                    //Add the event handler for handling non-UI thread exceptions to the event.
                    AppDomain.CurrentDomain.UnhandledException += new UnhandledExceptionEventHandler(TopLevelExceptionHandler.NonUIThreadExceptionHandler);

                    //#endif
                    LogGeneralInformations();

                    // handle command line switches
                    if (args.Length > 0 && args[0] == CLI_RESET)
                    {
                        DialogResult dr = MessageBox.Show("This will delete your settings. You will need to restart the application. Sure ?", "Warning", MessageBoxButtons.YesNo, MessageBoxIcon.Exclamation);
                        if (dr == DialogResult.Yes)
                        {
                            AllUsersSettingsService.ResetSettings();
                            // dot not call restart() else we will restart with the same args
                            // no really need to unlock, since stream will be closed automagically
                            fileMutex.Unlock();
                            return;
                        }
                    }

                    //starts splashscreen
                    _splashScreenForm = new SplashScreenForm() { StepCount = 3 };
                    // force resource manager singleton instanciation in main thread
                    string dummy = Properties.Resources.EnumLFOTriggerSources_LFO1;

                    Thread thread = new Thread(SplashThread)
                    {
                        IsBackground = true,
                        Name = "SplashScreenThread",
                        Priority = ThreadPriority.Normal
                    };
                    thread.Start();

                    // starts mainform
                    Application.Run(new MainForm(_splashScreenForm));
                } // IsLocked
                else
                {
                    MessageBox.Show("An instance of the application is already running", "Warning", MessageBoxButtons.OK);
                    return;
                }
            }
        }

        /// <summary>
        /// Logs the general informations into log file
        /// </summary>
        private static void LogGeneralInformations()
        {
            AssemblyName name = Assembly.GetEntryAssembly().GetName();
            string intro = string.Format("{0} version: {1}.{2}.{3}.{4}{6}{5}",
                name.Name,
                name.Version.Major, name.Version.Minor, name.Version.Build, name.Version.Revision,
                Properties.Resources.GuruStatement,
                Environment.NewLine
                );
            Logger.WriteLine(intro);
            Logger.WriteLine("Logger.TraceLevel is: " + Logger.Switch.Level.ToString());
        }
    }
}
