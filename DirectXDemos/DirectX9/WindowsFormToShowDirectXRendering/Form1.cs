/*
 * 1. Create WinForm project
2. Create Dll project and export the function in header file
3. using System.Runtime.InteropServices in WinForm project
5. Put the dll in same folder of exe file, on my computer, it's project_dir\bin\x86\Debug

6. Trouble shooting
 1) Dll not found, put the dll in same folder of the exe, and make sure they are in same bit(x86 or x64)
 2) Managed Debugging Assistant 'PInvokeStackImbalance' has detected a problem in 'Managed application path'.

Additional Information: A call to PInvoke function 'SuperSpecialOpenFileFunc' has unbalanced the stack. This is likely because the managed PInvoke signature does not match the unmanaged target signature. Check that the calling convention and parameters of the PInvoke signature match the target unmanaged signature.
see SO question here: http://stackoverflow.com/questions/2390407/pinvokestackimbalance-c-sharp-call-to-unmanaged-c-function
 * 
 * */

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

using System.Runtime.InteropServices;

namespace WindowsFormToShowDirectXRendering
{
    public partial class Form1 : Form
    {
        [DllImport("SimpleDirectX.dll", CallingConvention = CallingConvention.Cdecl, SetLastError = true)]
        static extern void InitD3D(Int32 hWnd);

        [DllImport("SimpleDirectX.dll", CallingConvention = CallingConvention.Cdecl, SetLastError = true)]
        static extern void InitVB();

        [DllImport("SimpleDirectX.dll", CallingConvention = CallingConvention.Cdecl, SetLastError = true)]
        static extern void Render();

        [DllImport("SimpleDirectX.dll", CallingConvention = CallingConvention.Cdecl, SetLastError = true)]
        static extern void Cleanup();

        public Form1()
        {
            InitializeComponent();
            InitD3D(this.Handle.ToInt32());
            InitVB();
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);
            Render();
        }

        protected override void OnClosed(EventArgs e)
        {
            base.OnClosed(e);
            Cleanup();
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }
    }
}
