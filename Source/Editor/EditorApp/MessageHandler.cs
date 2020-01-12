// MessageHandler.cs - filters Windows messages from C# to the C++ EditorApp

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace EditorApp
{
    //========================================================================
    // class MessageHandler                     - 
    //
    // We need to hook up our own message handler, since it's difficult 
    // getting the proper C wndProc params from using the standard C#
    // event handlers.
    //
    //========================================================================

    public class MessageHandler : IMessageFilter 
    {
        const int WM_LBUTTONDOWN = 0x0201;
        const int WM_LBUTTONUP = 0x0202;
        const int WM_LBUTTONDBLCLK = 0x0203;
        const int WM_RBUTTONDOWN = 0x0204;
        const int WM_RBUTTONUP = 0x0205;
        const int WM_RBUTTONDBLCLK = 0x0206;
        const int WM_MBUTTONDOWN = 0x0207;
        const int WM_MBUTTONUP = 0x0208;
        const int WM_MBUTTONDBLCLK = 0x0209;

        const int WM_KEYDOWN = 0x0100;
        const int WM_KEYUP = 0x0101;
        const int WM_SYSKEYDOWN = 0x0104;
        const int WM_SYSKEYUP = 0x0105;
        const int WM_CLOSE = 0x0010;

        Panel m_displayPanel;
        EditorForm m_parent;
        bool m_fakeFocus;
        System.Drawing.Point m_mouseDownPosition;

        // We take both the EditorForm's handle and its displayPanel handle, since messages
        // will sometimes be for the form, or the display panel.
        public MessageHandler( Panel displayPanel, EditorForm parent )
        {
            m_fakeFocus = false;
            m_displayPanel = displayPanel;
            m_parent = parent;
            m_mouseDownPosition = new System.Drawing.Point(0, 0); 
        }

        void CheckFakeFocus()
        {
            System.Drawing.Point position = Cursor.Position;
            System.Drawing.Point relativeToForm = m_displayPanel.PointToClient(position);
            m_fakeFocus = (relativeToForm.X >= 0 && relativeToForm.Y >= 0 &&
                relativeToForm.X < m_displayPanel.Width && relativeToForm.Y < m_displayPanel.Width);
            if (m_fakeFocus)
            {
                m_mouseDownPosition = position;
            }
        }

        public bool PreFilterMessage(ref Message m)
        {
            // Intercept messages only if they occur for the EditorForm
            // or its display panel.
            if (m.Msg == WM_LBUTTONDOWN || m.Msg == WM_RBUTTONDOWN || m.Msg == WM_MBUTTONDOWN)
                CheckFakeFocus();

            if (m.HWnd == m_displayPanel.Handle || (m_fakeFocus && (m.Msg==WM_KEYDOWN || m.Msg==WM_KEYUP) ) )
            {
                switch (m.Msg)
                {
                    case WM_LBUTTONDOWN:
                    case WM_RBUTTONDOWN:
                    case WM_MBUTTONDOWN:
                    case WM_KEYDOWN:
                    case WM_KEYUP:
                    case WM_SYSKEYDOWN:
                    case WM_SYSKEYUP:
                    case WM_LBUTTONUP:
                    case WM_LBUTTONDBLCLK:
                    case WM_RBUTTONUP:
                    case WM_RBUTTONDBLCLK:
                    case WM_MBUTTONUP:
                    case WM_MBUTTONDBLCLK:
                    case WM_CLOSE:
                        {
                            NativeMethods.WndProc(m_displayPanel.Handle, m.Msg, m.WParam.ToInt32(), m.LParam.ToInt32());
                            // If the left mouse button is up, try doing a 
                            // ray cast to see if it intersects with an actor
                            if (m_fakeFocus && m.Msg == WM_LBUTTONUP)
                            {
                                System.Drawing.Point position = Cursor.Position;
                                double distance = Math.Round(Math.Sqrt(Math.Pow((position.X - m_mouseDownPosition.X), 2) + Math.Pow((position.Y - m_mouseDownPosition.Y), 2)), 1);
                                if (distance < 3)
                                {
                                    m_parent.PickActor();
                                }
                            }
                            return true;
                        }
                }
            }
            return false;
        }

        //
        // Application_Idle                     - 2
        //
        public void Application_Idle(object sender, EventArgs e)
        {
            try
            {
                // Render the scene if we are idle
                NativeMethods.RenderFrame();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
            m_parent.Invalidate();
        }

    }
}
