#!/usr/bin/perl

if ($#ARGV != 2) {
    print "Please give the (1) result directory, (2) difference log file name, (3) same log file name, and (4) plain text file of test data.\n";
    exit(1);
}

my $TEST_CASES_FILE="collRegData.txt";
open TESTDATA, "< $TEST_CASES_FILE";
my @TEST_CASES = <TESTDATA>;
close TESTDATA;

my $TOP_RESULT_DIR=$ARGV[0];
my $RESULT_FILE1=$ARGV[1];
my $RESULT_FILE2=$ARGV[2];

open OUTFILE, "> $RESULT_FILE1";
open SAMEFILE, "> $RESULT_FILE2";

my @localeDirs = `ls $TOP_RESULT_DIR`;

foreach my $localeDir (@localeDirs) {
    chomp $localeDir;

    my $locale = $localeDir;

    print "Working on $locale\n";

    $localeDir = $TOP_RESULT_DIR."/".$localeDir;

    my @collationStringDirs = `ls $localeDir`;

    foreach my $collationStringDir (@collationStringDirs) {
        chomp $collationStringDir;

        my $collationString = $collationStringDir;

        $collationStringDir = $localeDir."/".$collationStringDir;

        my @filesSkip = ();

        my @logs1 = `ls $collationStringDir`;

        if (@logs1 > 1) {
            my @logs2 = `ls $collationStringDir`;
            my $text = "";
            my $whichFile = 0;
            
            $text = $text."\n";
            
            $text = $text."Locale: \"$locale\" and Collation String: \"$collationString\"\n";

            #print OUTFILE "Locale: \"$locale\" and Colation String: \"$collationString\"\n";

            foreach my $log1 (@logs1) {
                chomp $log1;
                push(@filesSkip, $log1);

                foreach my $log2 (@logs2) {
                    chomp $log2;
                    my $compareFiles = 1;
                    
                    my @lines1 = ();
                    my @lines2 = ();
                    my $resultPrinted = 0;

                    foreach my $fileName (@filesSkip) {
                        if ($log2 eq $fileName) {
                            $compareFiles = 0;
                            last;
                        }
                    }

                    if ($compareFiles) {
                        my $log1path = $collationStringDir."/".$log1;
                        my $log2path = $collationStringDir."/".$log2;

                        if ($log1 ne $log2) {
                            my $text2 = "";
                            my ($ICU1, $UCA1, $Type1) = split(/-/, $log1);
                            my ($ICU2, $UCA2, $Type2) = split(/-/, $log2);

                            open FILE1, "< ", $log1path or die $!;
                            open FILE2, "< ", $log2path or die $!;

                            my @contents1 = <FILE1>;
                            my @contents2 = <FILE2>;

                            close FILE1;
                            close FILE2;

                            my $count = 0;
                            my $equal = 1;
                            foreach my $line1 (@contents1) {
                                chomp $line1;
                                $line2 = @contents2[$count++];
                                chomp $line2;
                                if ($line1 ne $line2) {
                                    push(@lines1, "    ".$line1);
                                    push(@lines2, "    ".$line2);
                                    
                                    if ($resultPrinted == 0) {
                                        $text2 = $text2. "==>> DIFF ";
                                        #print OUTFILE "==>> DIFF ";
                                        $resultPrinted = 1;
                                    } 
                                }
                            }
                            if ($resultPrinted == 0) {
                                $text2 = $text2."??>> SAME ";
                                #print OUTFILE "??>> SAME ";
                                $resultPrinted = 2;
                            }
                            
                            $text2 = $text2."$Type1-ICU($ICU1)-UCA($UCA1)";
                            #print OUTFILE "$Type1-ICU($ICU1)-UCA($UCA1)";
                            if (length($ICU1) == 3) {
                                $text2 = $text2."~~";
                                #print OUTFILE "~~";
                            }
                            $text2 = $text2."~vs~$Type2-ICU($ICU2)-UCA($UCA2)\n";
                            #print OUTFILE "~vs~$Type2-ICU($ICU2)-UCA($UCA2)\n";
                            
                            # Print the differences
                            if ($resultPrinted == 1) {
                                $whichFile = 1;
                                print OUTFILE "$text.$text2";
                                print OUTFILE "\nDIFFERENCES:\n\n";
                                
                                my $length = $#lines1;
                                
                                for (my $i = 0; $i < $length; $i++) {
                                    my $index = &getIndex(@lines1[$i]);
                                    print OUTFILE "*TestCase==>> @TEST_CASES[$index]\n";
                                    print OUTFILE "  **$Type1-ICU($ICU1)-UCA($UCA1)\n@lines1[$i]\n\n";
                                    print OUTFILE "  **$Type2-ICU($ICU2)-UCA($UCA2)\n@lines2[$i]\n\n";
                                }
                            } else {
                                $whichFile = 2;
                                print SAMEFILE "$text.$text2";
                            }
                        }
                    }
                }
            }
        }
    }
}
close SAMEFILE;
close OUTFILE;
print("DONE!\n");
exit(0);

sub getIndex {
    my ($s) = @_;
    
    my @parsed1 = split (/\[/, $s);
    my @parsed2 = split(/\]/, @parsed1[1]);
     
    return (int(@parsed2[0]) - 1);
}
