/*
*******************************************************************************
*
*   Copyright (C) 2010, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*/
﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;
using System.IO;

namespace PerformanceTest
{
    class PerformanceTestFramework
    {
        private string fileName;
        private int passes;
        private int ops;
        protected int numOfLines;
        protected char[][] Lines;
        protected string[] wLines;
        protected string testName;
        protected string localeName;
        protected string wlocaleName;
        protected byte[] locale;

        protected static int resultsLength = 100;
        protected static char[] results;
        protected static byte[] resultsB;

        protected delegate void TestFunction(int lineNum);

        protected TestFunction myTestFunction;

        public void runTest()
        {
            for (int i = 0; i < passes; i++)
            {
                Console.Out.WriteLine("= {0} begin", testName);

                // Start stopwatch
                Stopwatch sw = Stopwatch.StartNew();


                for (int n = 0; n < numOfLines; n++)
                {
                    myTestFunction(n);
                }

                // Stop stopwatch
                sw.Stop();

                Console.Out.WriteLine("= {0} end {1} {2} {3}", testName, sw.Elapsed.TotalMilliseconds, numOfLines, ops);
            }
        }

        private void loadFile()
        {
            if (!File.Exists(fileName))
            {
                Console.Out.WriteLine("File does not exists: {0}", fileName);
                Environment.Exit(1);
            }

            // Get the number of lines
            StreamReader sr = File.OpenText(fileName);
            while (sr.ReadLine() != null) numOfLines++;
            sr.Close();

            Lines = new char[numOfLines][];
            wLines = new string[numOfLines];

            // Load the file into Lines
            sr = File.OpenText(fileName);
            for (int i = 0; i < numOfLines; i++)
            {
                wLines[i] = sr.ReadLine();
                Lines[i] = wLines[i].ToCharArray();
            }
            sr.Close();
        }

        protected virtual void loadTestFunction()
        {
            // Please override
        }

        public virtual void close()
        {
            // Please override if needed
        }

        private int getOperations()
        {
            int total = 0;

            for (int i = 0; i < numOfLines; i++)
            {
                total += Lines[i].Length;
            }
            return total;
        }

        private void loadArguments(string[] args)
        {
            int i;
            for (i = 0; i < args.Length; i++)
            {
                if (args[i] == "-p")
                {
                    passes = Convert.ToInt32(args[++i]);
                }
                else if (args[i] == "-f")
                {
                    fileName = args[++i];
                }
                else if (args[i] == "-t")
                {
                    // Skip time option
                    i++;
                }
                else if (args[i] == "-r")
                {
                    // Program handled already option
                    i++;
                }
                else if (args[i] == "-L")
                {
                    localeName = args[++i];
                }
                else
                {
                    testName = args[i];
                }
            }

            if (localeName == null)
            {
                localeName = "en_US";
            }

            locale = (new System.Text.UTF8Encoding().GetBytes(localeName.ToCharArray()));

            wlocaleName = localeName.Replace("_", "-");
        }
        public void initTest(string[] args)
        {
            if (args.Length == 1)
            {
                Console.Out.WriteLine("Please enter correct options.");
                Environment.Exit(3);
            }
            loadArguments(args);
            loadFile();
            loadTestFunction();

            results = new char[resultsLength];
            resultsB = new byte[resultsLength];
            ops = getOperations();
        }
    }
}
