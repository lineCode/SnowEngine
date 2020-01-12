//========================================================================
// Program.cs - entry point for the C# Editor

using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace EditorApp
{
    //
    // class Program                        - 
    //
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            EditorForm form = new EditorForm();

            // Hook up our message handler
            MessageHandler messageHandler = form.GetMessageHandler();
            Application.AddMessageFilter(messageHandler);
            Application.Idle += new EventHandler(messageHandler.Application_Idle);
            Application.Run(form);
        }
    }
}