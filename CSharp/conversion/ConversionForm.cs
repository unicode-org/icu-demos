/*
 * Copyright (C) 2005, International Business Machines
 *   Corporation and others.  All Rights Reserved.
 *
 * Original Author: George Rhoten
 */

using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.Runtime.InteropServices;

namespace ICU_Wrapper
{

	/// <summary>
	/// This is a GUI view of the ICU converter alias table.
	/// </summary>
	/// This is a crude example of how to create a C# wrapper around ICU.
	public class ConversionForm : System.Windows.Forms.Form
	{
		private System.Windows.Forms.ComboBox conveterBox;
		private System.Windows.Forms.ListBox aliasListBox;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		// TODO: This is messy. A lot of this can be avoided by using the --disable-renaming configure option
		// TODO: Consider using managed C++ instead of the C API. Of course, --disable-renaming should still be used.
		[DllImport("icuuc" + UVersion.U_ICU_VERSION_SHORT + "d.dll")]
		public static extern int ucnv_countAvailable_3_4();
		[DllImport("icuuc" + UVersion.U_ICU_VERSION_SHORT + "d.dll", CharSet=CharSet.Ansi)]
		public static extern byte * ucnv_getAvailableName_3_4(int n);
		[DllImport("icuuc" + UVersion.U_ICU_VERSION_SHORT + "d.dll", CharSet=CharSet.Ansi)]
		public static extern unsafe short ucnv_countAliases_3_4(byte *alias, int *pErrorCode);
		[DllImport("icuuc" + UVersion.U_ICU_VERSION_SHORT + "d.dll", CharSet=CharSet.Ansi)]
		public static extern unsafe byte * ucnv_getAlias_3_4(byte *alias, short n, int *pErrorCode);

		public ConversionForm()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			int convCount = ucnv_countAvailable_3_4();

			for (int idx = 0; idx < convCount; idx++) {
				unsafe {
					string convName = ToString(ucnv_getAvailableName_3_4(idx));
					conveterBox.Items.Add(convName);
				}
			}
			conveterBox.SelectedIndex = 0;
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(ConversionForm));
			this.conveterBox = new System.Windows.Forms.ComboBox();
			this.aliasListBox = new System.Windows.Forms.ListBox();
			this.SuspendLayout();
			// 
			// conveterBox
			// 
			this.conveterBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this.conveterBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.conveterBox.Location = new System.Drawing.Point(0, 0);
			this.conveterBox.Name = "conveterBox";
			this.conveterBox.Size = new System.Drawing.Size(288, 21);
			this.conveterBox.TabIndex = 0;
			this.conveterBox.SelectedIndexChanged += new System.EventHandler(this.conveterBox_SelectedIndexChanged);
			// 
			// aliasListBox
			// 
			this.aliasListBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
				| System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this.aliasListBox.IntegralHeight = false;
			this.aliasListBox.Location = new System.Drawing.Point(0, 24);
			this.aliasListBox.Name = "aliasListBox";
			this.aliasListBox.Size = new System.Drawing.Size(288, 184);
			this.aliasListBox.TabIndex = 1;
			// 
			// ConversionForm
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(288, 205);
			this.Controls.Add(this.aliasListBox);
			this.Controls.Add(this.conveterBox);
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Name = "ConversionForm";
			this.Text = "ICU4C# Wrapper Demo";
			this.ResumeLayout(false);

		}
		#endregion

		private unsafe string ToString(byte *bstr) {
			string ustr = null;
			if (bstr != null) {
				ustr = "";
				while (*bstr != 0) {
					ustr += (char)*(bstr++);
				}
			}
			return ustr;
		}

		private unsafe void conveterBox_SelectedIndexChanged(object sender, System.EventArgs e) {
			ComboBox box = (ComboBox)sender;
			byte *convNameBytes = ucnv_getAvailableName_3_4(box.SelectedIndex);
			int errorCode = 0;
			aliasListBox.Items.Clear();
			if (convNameBytes != null) {
				short aliasCount = ucnv_countAliases_3_4(convNameBytes, &errorCode);
				for (short idx = 0; idx < aliasCount; idx++) {
					string convAlias = ToString(ucnv_getAlias_3_4(convNameBytes, idx, &errorCode));
					aliasListBox.Items.Add(convAlias);
				}
			}
		}

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new ConversionForm());
		}
	}
}
