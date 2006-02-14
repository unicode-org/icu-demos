/*
 * Copyright (C) 2005-2006, International Business Machines
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
		public enum UErrorCode 
		{
			U_USING_FALLBACK_WARNING = -128,
			U_ERROR_WARNING_START = -128,
			U_USING_DEFAULT_WARNING = -127,
			U_SAFECLONE_ALLOCATED_WARNING = -126,
			U_STATE_OLD_WARNING = -125,
			U_STRING_NOT_TERMINATED_WARNING = -124,
			U_SORT_KEY_TOO_SHORT_WARNING = -123,
			U_AMBIGUOUS_ALIAS_WARNING = -122,
			U_DIFFERENT_UCA_VERSION = -121,
			U_ERROR_WARNING_LIMIT,
			U_ZERO_ERROR = 0,
			U_ILLEGAL_ARGUMENT_ERROR = 1,
			U_MISSING_RESOURCE_ERROR = 2,
			U_INVALID_FORMAT_ERROR = 3,
			U_FILE_ACCESS_ERROR = 4,
			U_INTERNAL_PROGRAM_ERROR = 5,
			U_MESSAGE_PARSE_ERROR = 6,
			U_MEMORY_ALLOCATION_ERROR = 7,
			U_INDEX_OUTOFBOUNDS_ERROR = 8,
			U_PARSE_ERROR = 9,
			U_INVALID_CHAR_FOUND = 10,
			U_TRUNCATED_CHAR_FOUND = 11,
			U_ILLEGAL_CHAR_FOUND = 12,
			U_INVALID_TABLE_FORMAT = 13,
			U_INVALID_TABLE_FILE = 14,
			U_BUFFER_OVERFLOW_ERROR = 15,
			U_UNSUPPORTED_ERROR = 16,
			U_RESOURCE_TYPE_MISMATCH = 17,
			U_ILLEGAL_ESCAPE_SEQUENCE = 18,
			U_UNSUPPORTED_ESCAPE_SEQUENCE = 19,
			U_NO_SPACE_AVAILABLE = 20,
			U_CE_NOT_FOUND_ERROR = 21,
			U_PRIMARY_TOO_LONG_ERROR = 22,
			U_STATE_TOO_OLD_ERROR = 23,
			U_TOO_MANY_ALIASES_ERROR = 24,
			U_ENUM_OUT_OF_SYNC_ERROR = 25,
			U_INVARIANT_CONVERSION_ERROR = 26,
			U_INVALID_STATE_ERROR = 27,
			U_COLLATOR_VERSION_MISMATCH = 28,
			U_USELESS_COLLATOR_ERROR = 29,
			U_NO_WRITE_PERMISSION = 30,
			U_STANDARD_ERROR_LIMIT,
			U_BAD_VARIABLE_DEFINITION=0x10000,
			U_PARSE_ERROR_START = 0x10000,
			U_MALFORMED_RULE,
			U_MALFORMED_SET,
			U_MALFORMED_SYMBOL_REFERENCE,
			U_MALFORMED_UNICODE_ESCAPE,
			U_MALFORMED_VARIABLE_DEFINITION,
			U_MALFORMED_VARIABLE_REFERENCE,
			U_MISMATCHED_SEGMENT_DELIMITERS,
			U_MISPLACED_ANCHOR_START,
			U_MISPLACED_CURSOR_OFFSET,
			U_MISPLACED_QUANTIFIER,
			U_MISSING_OPERATOR,
			U_MISSING_SEGMENT_CLOSE,
			U_MULTIPLE_ANTE_CONTEXTS,
			U_MULTIPLE_CURSORS,
			U_MULTIPLE_POST_CONTEXTS,
			U_TRAILING_BACKSLASH,
			U_UNDEFINED_SEGMENT_REFERENCE,
			U_UNDEFINED_VARIABLE,
			U_UNQUOTED_SPECIAL,
			U_UNTERMINATED_QUOTE,
			U_RULE_MASK_ERROR,
			U_MISPLACED_COMPOUND_FILTER,
			U_MULTIPLE_COMPOUND_FILTERS,
			U_INVALID_RBT_SYNTAX,
			U_INVALID_PROPERTY_PATTERN,
			U_MALFORMED_PRAGMA,
			U_UNCLOSED_SEGMENT,
			U_ILLEGAL_CHAR_IN_SEGMENT,
			U_VARIABLE_RANGE_EXHAUSTED,
			U_VARIABLE_RANGE_OVERLAP,
			U_ILLEGAL_CHARACTER,
			U_INTERNAL_TRANSLITERATOR_ERROR,
			U_INVALID_ID,
			U_INVALID_FUNCTION,
			U_PARSE_ERROR_LIMIT,
			U_UNEXPECTED_TOKEN=0x10100,
			U_FMT_PARSE_ERROR_START=0x10100,
			U_MULTIPLE_DECIMAL_SEPARATORS,
			U_MULTIPLE_DECIMAL_SEPERATORS = U_MULTIPLE_DECIMAL_SEPARATORS,
			U_MULTIPLE_EXPONENTIAL_SYMBOLS,
			U_MALFORMED_EXPONENTIAL_PATTERN,
			U_MULTIPLE_PERCENT_SYMBOLS,
			U_MULTIPLE_PERMILL_SYMBOLS,
			U_MULTIPLE_PAD_SPECIFIERS,
			U_PATTERN_SYNTAX_ERROR,
			U_ILLEGAL_PAD_POSITION,
			U_UNMATCHED_BRACES,
			U_UNSUPPORTED_PROPERTY,
			U_UNSUPPORTED_ATTRIBUTE,
			U_FMT_PARSE_ERROR_LIMIT,
			U_BRK_ERROR_START=0x10200,
			U_BRK_INTERNAL_ERROR,
			U_BRK_HEX_DIGITS_EXPECTED,
			U_BRK_SEMICOLON_EXPECTED,
			U_BRK_RULE_SYNTAX,
			U_BRK_UNCLOSED_SET,
			U_BRK_ASSIGN_ERROR,
			U_BRK_VARIABLE_REDFINITION,
			U_BRK_MISMATCHED_PAREN,
			U_BRK_NEW_LINE_IN_QUOTED_STRING,
			U_BRK_UNDEFINED_VARIABLE,
			U_BRK_INIT_ERROR,
			U_BRK_RULE_EMPTY_SET,
			U_BRK_UNRECOGNIZED_OPTION,
			U_BRK_MALFORMED_RULE_TAG,
			U_BRK_ERROR_LIMIT,
			U_REGEX_ERROR_START=0x10300,
			U_REGEX_INTERNAL_ERROR,
			U_REGEX_RULE_SYNTAX,
			U_REGEX_INVALID_STATE,
			U_REGEX_BAD_ESCAPE_SEQUENCE,
			U_REGEX_PROPERTY_SYNTAX,
			U_REGEX_UNIMPLEMENTED,
			U_REGEX_MISMATCHED_PAREN,
			U_REGEX_NUMBER_TOO_BIG,
			U_REGEX_BAD_INTERVAL,
			U_REGEX_MAX_LT_MIN,
			U_REGEX_INVALID_BACK_REF,
			U_REGEX_INVALID_FLAG,
			U_REGEX_LOOK_BEHIND_LIMIT,
			U_REGEX_SET_CONTAINS_STRING,
			U_REGEX_ERROR_LIMIT,
			U_IDNA_ERROR_START=0x10400,
			U_IDNA_PROHIBITED_ERROR,
			U_IDNA_UNASSIGNED_ERROR,
			U_IDNA_CHECK_BIDI_ERROR,
			U_IDNA_STD3_ASCII_RULES_ERROR,
			U_IDNA_ACE_PREFIX_ERROR,
			U_IDNA_VERIFICATION_ERROR,
			U_IDNA_LABEL_TOO_LONG_ERROR,
			U_IDNA_ZERO_LENGTH_LABEL_ERROR,
			U_IDNA_ERROR_LIMIT,
			U_STRINGPREP_PROHIBITED_ERROR = U_IDNA_PROHIBITED_ERROR,
			U_STRINGPREP_UNASSIGNED_ERROR = U_IDNA_UNASSIGNED_ERROR,
			U_STRINGPREP_CHECK_BIDI_ERROR = U_IDNA_CHECK_BIDI_ERROR,
			U_ERROR_LIMIT=U_IDNA_ERROR_LIMIT
		};

		// TODO: This is messy. A lot of this can be avoided by using the --disable-renaming configure option
		// TODO: Consider using managed C++ instead of the C API. Of course, --disable-renaming should still be used.
		[DllImport("icuuc" + UVersion.U_ICU_VERSION_SHORT + ".dll", EntryPoint="ucnv_countAvailable"+UVersion.U_ICU_VERSION_SUFFIX)]
		public static extern int ucnv_countAvailable();
		[DllImport("icuuc" + UVersion.U_ICU_VERSION_SHORT + ".dll", EntryPoint="ucnv_getAvailableName"+UVersion.U_ICU_VERSION_SUFFIX)]
		public static extern byte * ucnv_getAvailableName(int n);
		[DllImport("icuuc" + UVersion.U_ICU_VERSION_SHORT + ".dll", EntryPoint="ucnv_countAliases"+UVersion.U_ICU_VERSION_SUFFIX)]
		public static extern unsafe short ucnv_countAliases(byte *alias, ref UErrorCode pErrorCode);
		[DllImport("icuuc" + UVersion.U_ICU_VERSION_SHORT + ".dll", EntryPoint="ucnv_getAlias"+UVersion.U_ICU_VERSION_SUFFIX)]
		public static extern unsafe byte * ucnv_getAlias(byte *alias, short n, ref UErrorCode pErrorCode);

		public ConversionForm()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			int convCount = ucnv_countAvailable();

			for (int idx = 0; idx < convCount; idx++) {
				unsafe {
					string convName = ToString(ucnv_getAvailableName(idx));
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
			byte *convNameBytes = ucnv_getAvailableName(box.SelectedIndex);
			UErrorCode errorCode = UErrorCode.U_ZERO_ERROR;
			aliasListBox.Items.Clear();
			if (convNameBytes != null) {
				short aliasCount = ucnv_countAliases(convNameBytes, ref errorCode);
				for (short idx = 0; idx < aliasCount; idx++) {
					string convAlias = ToString(ucnv_getAlias(convNameBytes, idx, ref errorCode));
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
