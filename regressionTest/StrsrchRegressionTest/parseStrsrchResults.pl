#!/usr/bin/perl

if ($#ARGV != 1) {
    print "Please give the result directory and result log file name.\n";
    exit(1);
}

my $TOP_RESULT_DIR=$ARGV[0];
my $RESULT_FILE=$ARGV[1];
my $SAME_FILE="same.log";
open OUTFILE, "> $RESULT_FILE";
open SAME, "> $SAME_FILE";

my @localeDirs = `ls $TOP_RESULT_DIR`;

foreach my $localeDir (@localeDirs) {
    chomp $localeDir;

    my $locale = $localeDir;

    $localeDir = $TOP_RESULT_DIR."/".$localeDir;

    my @collationStringDirs = `ls $localeDir`;

    foreach my $collationStringDir (@collationStringDirs) {
        chomp $collationStringDir;
        
        my $writtenSameLog = 0;

        my $collationString = $collationStringDir;

        $collationStringDir = $localeDir."/".$collationStringDir;

        my @filesSkip = ();

        my @logs1 = `ls $collationStringDir`;

        if (@logs1 > 1) {
            my $printHeader = 1;
            my @logs2 = `ls $collationStringDir`;

            foreach my $log1 (@logs1) {
                chomp $log1;
                push(@filesSkip, $log1);

                foreach my $log2 (@logs2) {
                    chomp $log2;
                    my $compareFiles = 1;

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
                            my @ICU1Lines;
                            my @ICU2Lines;
                            my $index = 0;
                            my $firstLine = 1;
                            
                            my ($ICU1, $UCA1) = split(/-/, $log1);
                            my ($ICU2, $UCA2) = split(/-/, $log2);

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
                                
                                if ($firstLine == 1) {
                                    $firstLine = 0;
                                } else {          
                                    $ICU1Lines[$index] = $line1;
                                    $ICU2Lines[$index++] = $line2;
                                }
                                
                                if ($line1 ne $line2) {
                                    $equal = 0;
                                    last;
                                }
                            }
                            if ($equal == 0) {
                                if ($printHeader == 1) {
                                    $printHeader = 0;
                                    print OUTFILE "Locale: \"$locale\" and Colation String: \"$collationString\"\n";
                                }
                                print OUTFILE "==>> DIFF ICU($ICU1)-UCA($UCA1)";
                                if (length($ICU1) == 3) {
                                    print OUTFILE "~~";
                                }
                                print OUTFILE "~vs~ICU($ICU2)-UCA($UCA2)\n";
                                
                                print OUTFILE "Results: (start length)\n";
                                
                                for (my $i = 0; $i < $index; $i++) {
                                    print OUTFILE "\t[ICU($ICU1)]($ICU1Lines[$i]) -- [ICU($ICU2)]($ICU2Lines[$i])\n";
                                }
                                $writtenSameLog = 0;
                            } else {
                                if ($printHeader == 1) {
                                    $printHeader = 0;
                                    print SAME "Locale: \"$locale\" and Colation String: \"$collationString\"\n";
                                }
                                print SAME "??>> SAME ICU($ICU1)-UCA($UCA1)";

                                if (length($ICU1) == 3) {
                                    print SAME "~~";
                                }
                                print SAME "~vs~ICU($ICU2)-UCA($UCA2)\n";
                                
                                print SAME "Results: (start length)\n";
                                
                                for (my $i = 0; $i < $index; $i++) {
                                    print SAME "\t[ICU($ICU1)]($ICU1Lines[$i]) -- [ICU($ICU2)]($ICU2Lines[$i])\n";
                                }
                                $writtenSameLog = 1;
                            }
                        }
                    }
                }
            }

            if ($writtenSameLog == 1) {
                print SAME "\n";
            } else {
                print OUTFILE "\n";
            }
        }
    }
}

close OUTFILE;
close SAME;

system ("cat $SAME_FILE >> $RESULT_FILE");
system("rm -f $SAME_FILE");

print("DONE\n");
exit(0);

