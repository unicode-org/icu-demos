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
using PerformanceTest;
using NormPerfTest;
using CollPerfTest;

namespace PerformanceTestMain
{
    class PerfTestMain
    {
        static string getPerfType(string[] args)
        {
            string type = null;
            for (int i = 0; i < args.Length; i++)
            {
                if (args[i] == "-r")
                {
                    type = args[i + 1];
                    break;
                }
            }
            return type;
        }

        static void Main(string[] args)
        {
            string type = getPerfType(args);
            if (type == null)
            {
                Console.Out.WriteLine("Please enter which test you want to run.");
                Environment.Exit(5);
            }

            PerformanceTestFramework perf = null;
            if (type == "normperf")
            {
                perf = new NormalizationPerformanceTest();
            }
            else if (type == "collperf")
            {
                perf = new CollationPerformanceTest();
            }
            else
            {
                Console.Out.WriteLine("Please enter correct test you want to run.");
                Environment.Exit(4);
            }
            perf.initTest(args);

            perf.runTest();

            perf.close();
        }
    }
}
