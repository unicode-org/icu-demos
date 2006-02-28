/*
*******************************************************************************
*
*   Copyright (C) 2006, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*/
using System;
using System.Collections;
using System.IO;
using System.Diagnostics;
using System.Text.RegularExpressions;

namespace icu
{
	/// <summary>
	/// This tool generates a C# wrapper around ICU4C code
	/// </summary>
	class GenICUWrapper
	{
		static string popen(string command, string arguments) 
		{
			Process p = new Process();
			StreamWriter sw;
			StreamReader sr;
			StreamReader err;

			ProcessStartInfo psI = new ProcessStartInfo(command, arguments);
			psI.UseShellExecute = false;
			psI.RedirectStandardInput = true;
			psI.RedirectStandardOutput = true;
			psI.RedirectStandardError = true;
			psI.CreateNoWindow = true;
			p.StartInfo = psI;
   
			p.Start();
			sw = p.StandardInput;
			sr = p.StandardOutput;
			err = p.StandardError;
			sw.AutoFlush = true;
			sw.Close();

			p.Close();

			return sr.ReadToEnd();
		}
		static private string ReplaceAll(string source, string from, string to) 
		{
			Regex replacer = new Regex(from, RegexOptions.Singleline);
			return replacer.Replace(source, to);
		}

		//"(typedef\\s+(struct|enum)([^{]+\\{[^}]+\\}|\\s+\\w\\s+)\\w;|(\\bU_STABLE\\b|\\btypedef\\b|\\bstruct\\b|\\benum\\b)[^;]+;)", RegexOptions.Compiled | RegexOptions.Singleline);
		private static Regex stableHeadersRegEx = new Regex("(\\bU_STABLE\\b[^;]+;|\\benum\\b\\s*\\w+\\s*\\{[^;]*;|\\bstruct\\s+\\w+|typedef\\s+(const\\s*)?void\\s*\\*?\\s+\\w+\\s*;|typedef\\s+enum\\s*\\{[^}]+\\}[^;]*;)", RegexOptions.Compiled | RegexOptions.Singleline);
		private static Regex isFunctionRegEx = new Regex("\\bU_STABLE\\b[^;]+;", RegexOptions.Compiled | RegexOptions.Singleline);
		private static Regex endOfEnumRegEx = new Regex("\\w+\\s*;$", RegexOptions.Compiled | RegexOptions.Singleline);
		private static ArrayList definedStructs = new ArrayList();
		private static string versionSuffix = "";
		private static string[] unusableTypesCS = {
													"[",
													"...",
													"va_list ",
													"UNESCAPE_CHAR_AT ",
													"UMtx",
													"UMemAllocFn",
													"UTraceEntry",
													"u_cleanup",
													"UEnumCharNamesFn",
													"UCharEnumTypeRange",
													"UConverterToUCallback",
													"UConverterFromUCallback",
													"UConverterToUnicodeArgs",
													"UConverterFromUnicodeArgs",
													"UDataInfo",
													"UDataMemoryIsAcceptable",
													"u_nl_catd"
													};
		private struct RegexToMatch 
		{
			public Regex from;
			public string to;
			public RegexToMatch(string from, string to) 
			{
				this.from = new Regex(from, RegexOptions.Singleline);
				this.to = to;
			}
		}
		private static RegexToMatch[] typeMap = {
													new RegexToMatch("\\bconst\\s?", ""),
													new RegexToMatch("\\(\\s*void\\s*\\)", "()"),
													// Remove C# keywords.
													new RegexToMatch("\\block\\b", "lock_"),
													new RegexToMatch("\\bbase\\b", "base_"),
													new RegexToMatch("\\bvalue\\b", "val"),
													new RegexToMatch("\\bstring\\b", "str"),
													// Convert to C# types
													new RegexToMatch("\\buint8_t\\b", "byte"),
													new RegexToMatch("\\buint16_t\\b", "ushort"),
													new RegexToMatch("\\buint32_t\\b", "uint"),
													new RegexToMatch("\\buint64_t\\b", "ulong"),
													new RegexToMatch("\\bint8_t\\b", "sbyte"),
													new RegexToMatch("\\bint16_t\\b", "short"),
													new RegexToMatch("\\bint32_t\\b", "int"),
													new RegexToMatch("\\bint64_t\\b", "long"),
													new RegexToMatch("\\bUBool\\b", "bool"),
													new RegexToMatch("\\bchar\\b", "byte"),
													new RegexToMatch("\\bUChar\\b", "char"),
													new RegexToMatch("\\bwchar_t\\b", "char"),
													new RegexToMatch("\\bUChar32\\b", "int"),
													new RegexToMatch("\\bUDate\\b", "double"),
													new RegexToMatch("\\s*UErrorCode\\s*\\*\\s*", " ref UErrorCode "),
													new RegexToMatch("\\bUBiDiLevel\\b", "byte"),
													new RegexToMatch("\\bUVersionInfo\\b", "int"),
													new RegexToMatch("\\bUCollationStrength\\b", "UColAttributeValue"),
													new RegexToMatch("\\s*\\(\\s*", "("),
													new RegexToMatch("\\s*\\)\\s*", ")"),
												};

		static private bool ContainsUnusableType(string[] unusableTypes, string line) 
		{
			for (int idx = 0; idx < unusableTypes.Length; idx++) 
			{
				if (line.IndexOf(unusableTypes[idx]) >= 0) 
				{
					return true;
				}
			}
			return false;
		}
		static private string ReplaceTypes(string lineToConvert) 
		{
			for (int idx = 0; idx < typeMap.Length; idx++) 
			{
				lineToConvert = typeMap[idx].from.Replace(lineToConvert, typeMap[idx].to);
			}
			return lineToConvert;
		}
		static private string GetFunctionName(string lineToParse) 
		{
			int end = lineToParse.IndexOf("(");
			if (end < 0) 
			{
				throw new FormatException("Parse Exception: Line does not contain a function name.\nline=" + lineToParse);
			}
			int beginning = lineToParse.LastIndexOf(" ", end);
			if (beginning < 0) 
			{
				throw new FormatException("Parse Exception: Line does not contain a function name.\nline=" + lineToParse);
			}
			string retVal = lineToParse.Substring(beginning + 1, end - beginning - 1);
			return retVal;
		}
		static private string GenerateCS(string processedHeader, string libName) 
		{
			string parsedHeader;
			string functionName = "";
			if (processedHeader.IndexOf("class") >= 0) 
			{
				// We can't use unmanaged C++ classes in C# right now.
				return "//C++\n";
			}
			else
			{
				parsedHeader = "//C\n";
			}
			foreach (Match matchedItem in stableHeadersRegEx.Matches(processedHeader)) 
			{
				string matchedString = matchedItem.Value;
				string unsafeKeyword;
				if (isFunctionRegEx.IsMatch(matchedString)) 
				{
					matchedString = ReplaceAll(matchedString, "\\bU_EXPORT2\\b", "");
					matchedString = ReplaceAll(matchedString, "\\s\\s+", " ");
					if (ContainsUnusableType(unusableTypesCS, matchedString)) 
					{
						parsedHeader += "// " + matchedString + "\n";
					}
					else 
					{
						matchedString = ReplaceTypes(matchedString);
						if (matchedString.IndexOf("*") >= 0 || matchedString.IndexOf("u_cleanup") >= 0) 
						{
							unsafeKeyword = " unsafe";
						}
						else 
						{
							unsafeKeyword = "";
						}
						if (versionSuffix.Length > 0) 
						{
							functionName = ", EntryPoint=\""+GetFunctionName(matchedString)+versionSuffix+"\"";
						}
						matchedString = ReplaceAll(matchedString, "\\bU_STABLE\\b", "public static extern" + unsafeKeyword);
						parsedHeader += "[DllImport(\""+libName+"\""+functionName+")]\n" + matchedString + "\n";
					}
				}
				else if (matchedString.IndexOf("typedef enum ") >= 0) 
				{
					matchedString = ReplaceAll(matchedString, "typedef ", "");
					matchedString = ReplaceAll(matchedString, ",\\s*", ",\n");
					matchedString = ReplaceAll(matchedString, "\\{\\s*", "{\n");
					string enumName = endOfEnumRegEx.Match(matchedString).Value;
					enumName = ReplaceAll(enumName, "\\s*;", "");
					matchedString = ReplaceAll(matchedString, "enum", "enum " + enumName);
					matchedString = endOfEnumRegEx.Replace(matchedString, "");
					parsedHeader += "public " + matchedString + "\n";
				}
				else if (matchedString.IndexOf("enum ") >= 0)
				{
					matchedString = ReplaceAll(matchedString, "\\}\\s*\\w+\\s*;", "};");
					matchedString = ReplaceAll(matchedString, ",\\s*", ",\n");
					matchedString = ReplaceAll(matchedString, "\\{\\s*", "{\n");
					matchedString = ReplaceAll(matchedString, "\\bINT32_MAX\\b", "int.MaxValue");
					parsedHeader += "public " + matchedString + "\n";
				}
				else if (matchedString.IndexOf("struct ") >= 0)
				{
					if (definedStructs.Contains(matchedString)) 
					{
						continue;
					}
					definedStructs.Add(matchedString);
					parsedHeader += "[StructLayout(LayoutKind.Sequential)]\npublic " + matchedString + " {}\n";
				}
				else if (matchedString.IndexOf("typedef ") >= 0)
				{
					definedStructs.Add(matchedString);
					matchedString = ReplaceAll(matchedString, "typedef\\s+(const\\s*)?void\\s*\\*?\\s+", "struct ");
					matchedString = ReplaceAll(matchedString, ";", "");
					if (definedStructs.Contains(matchedString)) 
					{
						continue;
					}
					definedStructs.Add(matchedString);
					parsedHeader += "[StructLayout(LayoutKind.Sequential)]\npublic " + matchedString + " {}\n";
				}
				else 
				{
					Console.WriteLine("{0} is an unknown match", matchedString);
					Environment.Exit(-1);
				}
			}
			return parsedHeader;
		}

		static private void printHelp() 
		{
			Console.WriteLine("\t--classname  Name of class");
			Console.WriteLine("\t--libname    Name of library");
			Console.WriteLine("\t--suffix     Suffix used on each function");
			Console.WriteLine("\t--include    The directory containing the ICU headers");
		}

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static int Main(string[] args)
		{
			string includePath = null;
			string className = null;
			string libName = null;
			for (int idx = 0; idx < args.Length; idx++) 
			{
				if (args[idx].Equals("--classname")) 
				{
					if (idx + 1 >= args.Length) 
					{
						Console.WriteLine("--classname is missing required argument");
						Environment.Exit(-1);
					}
					className = args[++idx];
				}
				else if (args[idx].Equals("--libname")) 
				{
					if (idx + 1 >= args.Length) 
					{
						Console.WriteLine("--libname is missing required argument");
						Environment.Exit(-1);
					}
					libName = args[++idx];
				}
				else if (args[idx].Equals("--include")) 
				{
					if (idx + 1 >= args.Length) 
					{
						Console.WriteLine("--include is missing required argument");
						Environment.Exit(-1);
					}
					includePath = args[++idx];
				}
				else if (args[idx].Equals("--suffix")) 
				{
					if (idx + 1 >= args.Length) 
					{
						Console.WriteLine("--suffix is missing required argument");
						Environment.Exit(-1);
					}
					versionSuffix = args[++idx];
				}
			}
			if (className == null || includePath == null || libName == null) 
			{
				Console.WriteLine("Required arguments are missing.");
				printHelp();
				Environment.Exit(-1);
			}
			if (!Directory.Exists(includePath))
			{
				Console.WriteLine("{0} is not a directory.", includePath);
				return -1;
			}
			bool fileParsed = false;
			Console.WriteLine("using System.Runtime.InteropServices;");
			Console.WriteLine("namespace icu {");
			Console.WriteLine("class "+className+" {");
			foreach (string header in Directory.GetFiles(includePath, "*.h")) 
			{
				string processedHeader;
				fileParsed = true;
				Console.WriteLine("#region {0}", header);
				try 
				{
					processedHeader = popen("cpp", "-P -nostdinc -x c " + header);
				}
				catch (SystemException) 
				{
					Console.WriteLine("Can't preprocess {0}.\nIs the Cygwin preprocessor in your PATH?", header);
					return -1;
				}
				Console.Write(GenerateCS(processedHeader, libName));
				Console.WriteLine("#endregion");
			}
			if (!fileParsed) 
			{
				Console.WriteLine("//No headers found.");
			}
			Console.WriteLine("}");
			Console.WriteLine("}");
			return 0;
		}
	}
}
