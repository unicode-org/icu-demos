/********************************************************************
 * COPYRIGHT: 
 * Copyright (c) 2001, International Business Machines Corporation and
 * others. All Rights Reserved.
 ********************************************************************/

#include "cppcnvt.h"

#ifdef ICU_UNICODECONVERTER_USE_DEPRECATES
#include "unicode/locid.h"
#include "unicode/unistr.h"
#include "unicode/ucnv_err.h"
#include "cstring.h"
#include <stdio.h>

#define NUM_CODEPAGE 1
#define MAX_FILE_LEN 1024*20
#define UCS_FILE_NAME_SIZE 512

/*writes and entire UnicodeString along with a BOM to a file*/
void WriteToFile(const UnicodeString *a, FILE *myfile); 
/*Case insensitive compare*/
int32_t strCaseIcmp(const char* a1,const char * a2); 
/*returns an action other than the one provided*/
UConverterFromUCallback otherUnicodeAction(UConverterFromUCallback MIA);
UConverterToUCallback otherCharAction(UConverterToUCallback MIA);
/*Asciifies the UErrorCodes*/
#define myErrorName(errorCode) u_errorName(errorCode)

void ConvertTest::runIndexedTest( int32_t index, UBool exec, const char* &name, char* /*par*/ )
{
    if (exec) logln("TestSuite ConvertTest: ");
    switch (index) {
        case 0: name = "TestConvert"; if (exec) TestConvert(); break;
        case 1: name = "TestAmbiguous"; if (exec) TestAmbiguous(); break;
        default: name = ""; break; //needed to end loop
    }
}

/* Test is also located in ccapitst.c */
void ConvertTest::TestConvert() 
{
    char                myptr[4];
    char                save[4];
    int32_t             testLong1;
    int16_t             rest                = 0;
    int32_t             x                   = 0;
    FILE*               ucs_file_in         = NULL;
    UChar               BOM                 = 0x0000;
    UChar               myUChar             = 0x0000;
    char*               mytarget            = new char[MAX_FILE_LEN];
    char*               mytarget_1          = mytarget;
    char*               mytarget_use        = mytarget;
    char*               consumed            = NULL;
    char*               output_cp_buffer    = new char[MAX_FILE_LEN];
    UChar*              ucs_file_buffer     = new UChar[MAX_FILE_LEN];
    UChar*              ucs_file_buffer_use = ucs_file_buffer;
    UChar*              my_ucs_file_buffer  = new UChar[MAX_FILE_LEN];
    UChar*              my_ucs_file_buffer_1 = my_ucs_file_buffer;
    int32_t             i                   = 0;
    int8_t              ii                  = 0;
    uint16_t            codepage_index      = 0;
    int32_t             cp                  = 0;
    UErrorCode          err                 = U_ZERO_ERROR;
    const char* const*  available_conv      = NULL;
    char       ucs_file_name[UCS_FILE_NAME_SIZE];
    UConverterFromUCallback MIA1, MIA1_2;
    UConverterToUCallback   MIA2, MIA2_2;
    const void         *MIA1Context, *MIA1Context2, *MIA2Context, *MIA2Context2;
    UnicodeConverter* someConverters[5];
    /******************************************************************
                                Checking Unicode -> ksc
     ******************************************************************/

    const char*      CodePagesToTest[NUM_CODEPAGE] =
    {
       "IBM-949"
    };

    const uint16_t   CodePageNumberToTest[NUM_CODEPAGE] =
    {
        949
    };

/*    const int32_t        CodePagesAsciiControls[NUM_CODEPAGE]    =
    { 
        0xFFFFFFFF
    };

    const int32_t        CodePagesOtherControls[NUM_CODEPAGE]    =
    {
         0x00000005
    };*/

    const int8_t     CodePagesMinChars[NUM_CODEPAGE] =
    {
        1
    };

    const int8_t     CodePagesMaxChars[NUM_CODEPAGE] =
    {
        2
    };

    const int16_t    CodePagesSubstitutionChars[NUM_CODEPAGE] =
    {
        (int16_t)0xAFFEu
    };

    const char*      CodePagesTestFiles[NUM_CODEPAGE] =
    {
        "uni-text.bin" 
    };

    const UConverterPlatform CodePagesPlatform[NUM_CODEPAGE] =
    {
        UCNV_IBM
    };

/*    const UConverterToUCallback CodePagesMissingCharAction[NUM_CODEPAGE] =
    {
        UCNV_TO_U_CALLBACK_SUBSTITUTE
    };
    
    const UConverterFromUCallback CodePagesMissingUnicodeAction[NUM_CODEPAGE] =
    {
      UCNV_FROM_U_CALLBACK_SUBSTITUTE
    };*/

    const Locale CodePagesLocale[NUM_CODEPAGE] =
    {
        Locale::getKorean()
    };

    UConverterFromUCallback fromUAction = NULL;
    const void* fromUContext = NULL;
    UConverterToUCallback toUAction = NULL;
    const void* toUContext = NULL;

    /*Calling all the UnicodeConverter API and checking functionality*/

        /*Tests UnicodeConverter::getAvailableNames*/
    logln("\n---Testing UnicodeConverter::getAvailableNames...");
    available_conv = UnicodeConverter::getAvailableNames(testLong1, err);

    if (U_FAILURE(err)) 
    {
        errln("Error getting Available names!");
        return;
    }
    logln("Number of available Codepages:%d\t", testLong1);
    while (testLong1--)
        logln("\t\t[%s]", available_conv[testLong1]);

    ucnv_flushCache();

    /* Do some tests w/ UnicodeConverter, some w/ UnicodeConverter */

    someConverters[0] = new UnicodeConverter("ibm-1123",err);
    if (U_FAILURE(err)) {
        errln ((UnicodeString)"FAILURE a! " + myErrorName(err));
        return;
    }
    someConverters[1] = new UnicodeConverter("ibm-1123",err);
    if (U_FAILURE(err)) {
        errln ((UnicodeString)"FAILURE b! " + myErrorName(err));
        return;
    }
    someConverters[2] = new UnicodeConverter("ibm-1123",err);
    if (U_FAILURE(err)) {
        errln ((UnicodeString)"FAILURE c! " + myErrorName(err));
        return;
    }

    someConverters[3] = new UnicodeConverter("ibm-1383", err);
    if (U_FAILURE(err)) {
        errln ((UnicodeString)"FAILURE d! " + myErrorName(err));
        return;
    }
    someConverters[4] = new UnicodeConverter("ibm-949", err);
    if (U_FAILURE(err)) {
        errln ((UnicodeString)"FAILURE e! " + myErrorName(err));
        return;
    }
   
    logln("\n---Testing UnicodeConverter::flushCache...");
    if (UnicodeConverter::flushCache()==0)
        logln("Flush cache ok");
    else
        errln("Flush Cache failed");
    
    delete someConverters[0];
    delete someConverters[1];
    delete someConverters[2];
    delete someConverters[3];
    if (UnicodeConverter::flushCache()==2)
        logln("Flush cache ok");
    else
        errln("Flush Cache failed");
    
    delete someConverters[4];
    if (UnicodeConverter::flushCache()==1)
        logln("Flush cache ok");
    else
        errln("Flush Cache failed");

    logln("\n---Testing UnicodeConverter::UnicodeConverter()...");
    someConverters[0] = new UnicodeConverter;
    someConverters[1] = new UnicodeConverter;
    someConverters[2] = new UnicodeConverter("utf8", err);
    if (U_FAILURE(err)) {
        errln ((UnicodeString)"FAILURE! " + myErrorName(err));
        return;
    }

    logln("\n---Testing getName...");

    someConverters[1]->getName(err);
    if(U_FAILURE(err)) {
        errln("getName for Converters[1] failed!!!");
    } else {
        logln(UnicodeString("Result of Converters[1]->getName() was ") + UnicodeString(someConverters[1]->getName(err)));
    }

    someConverters[0]->getName(err);
    if(U_FAILURE(err)) {
        errln("getName for Converters[0] failed!!!");
    } else {
        logln(UnicodeString("Result of Converters[0]->getName() was ") + UnicodeString(someConverters[0]->getName(err)));
    }

    logln("\n---Testing UnicodeConverter::operator==...");
    if (((*someConverters[1] == *someConverters[0])==true)&&
    (*someConverters[1] == *someConverters[2])==false)
      logln("Equality test ok");
    else {
      if(!((*someConverters[1] == *someConverters[0])==true)) {
        errln("Equality test failed line " + UnicodeString() + 244);
      }
      if(!((*someConverters[1] == *someConverters[2])==false)) {
        errln("Equality test failed line " + UnicodeString() + 247);
      }
    }
    
    logln("\n---Testing UnicodeConverter::operator!=...");
    if (((*someConverters[1] != *someConverters[0])==false)&&
    (*someConverters[1] != *someConverters[2])==true)
      logln("Not Equal test ok");
    else
        errln("Not Equal test failed");
    
    logln("\n---Testing UnicodeConverter::operator=...");
    someConverters[3] = new UnicodeConverter;
    *someConverters[3] = *someConverters[2];
    if ((*someConverters[2] == *someConverters[3]))
      logln("Equality test ok");
    else
      errln("Equality test failed line "  + UnicodeString() + 262);

    delete someConverters[0];
    delete someConverters[1];
    delete someConverters[2];
    delete someConverters[3];

    for (codepage_index=0; codepage_index <  NUM_CODEPAGE; codepage_index++)
    {
        err = U_ZERO_ERROR;
        i = 0;
        char* index = 0;
        strcpy(ucs_file_name, IntlTest::loadTestData(err));
        if (!*ucs_file_name) {
            return;
        }
        index=strrchr(ucs_file_name,(char)U_FILE_SEP_CHAR);

        if((unsigned int)(index-ucs_file_name) != (strlen(ucs_file_name)-1)){
                *(index+1)=0;
        }
        
        strcat(ucs_file_name,".."U_FILE_SEP_STRING);
        if(U_FAILURE(err)){
             char  errmsg[UCS_FILE_NAME_SIZE + 128];
             sprintf(errmsg, "Couldn't open the testdata... Exiting...Error:%s \n", u_errorName(err));
        }
        
        strcat(ucs_file_name, CodePagesTestFiles[codepage_index]);

        ucs_file_in = fopen(ucs_file_name, "rb");
        if (!ucs_file_in) 
        {
            char  errmsg[UCS_FILE_NAME_SIZE + 128];
            sprintf(errmsg, "Couldn't open the Unicode file [%s]... Exiting...\n", ucs_file_name);
            errln(errmsg);
            return;
        }

        /*Creates a converter*/

        UnicodeConverter* myConverter = new UnicodeConverter(CodePageNumberToTest[codepage_index],UCNV_IBM, err);

        if (!myConverter || U_FAILURE(err))
        {
            errln("Error Creating the converter from %s codepage.\nMake sure you ran the makeconv tool to create %s.cnv\nFailed with err=%s (%d)",
                    CodePagesToTest[codepage_index],
                    CodePagesToTest[codepage_index],
                    u_errorName(err), err); 
            return;
        }


        /*Tests getMaxBytesPerChar and getMinBytesPerChar*/
        logln("\n---Testing UnicodeConverter::getMaxBytesPerChar...");
        if (myConverter->getMaxBytesPerChar()==CodePagesMaxChars[codepage_index])
            logln("Max byte per character OK");
        else
            errln("Max byte per character failed");

        logln("\n---Testing UnicodeConverter::getMinBytesPerChar...");
        if (myConverter->getMinBytesPerChar()==CodePagesMinChars[codepage_index])
            logln("Min byte per character OK");
        else
            errln("Min byte per character failed");


        /*getSubstitutions char*/
        logln("\n---Testing UnicodeConverter::getSubstitutionChars...");
        ii=4;
        myConverter->getSubstitutionChars(myptr,ii,err);

        for(x=0;x<ii;x++) rest = (int16_t)(((unsigned char)rest << 8) + (unsigned char)myptr[x])
            ;
        if (rest==CodePagesSubstitutionChars[codepage_index])
            logln("Substitution character ok");
        else
            errln("Substitution character failed.");



        logln("\n---Testing UnicodeConverter::setSubstitutionChars RoundTrip Test ...");
        myConverter->setSubstitutionChars(myptr, ii, err);
        if (U_FAILURE(err))
            errln ("FAILURE! " + (UnicodeString)myErrorName(err));
        myConverter->getSubstitutionChars(save, ii, err);
        if (U_FAILURE(err))
            errln ("FAILURE! " + (UnicodeString)myErrorName(err));
        if (strncmp(save, myptr, ii))
            errln("Saved substitution character failed");
        else
            logln("Saved substitution character ok");

        /*resetState*/
        logln("\n---Testing UnicodeConverter::resetState...");
        myConverter->resetState();


        /*getName*/
        UnicodeString* testUnistr          =   new UnicodeString();
        logln("\n---Testing UnicodeConverter::getName...");
        if (strCaseIcmp(myConverter->getName(err), CodePagesToTest[codepage_index]))
            errln("getName failed");
        else
            logln("getName ok");

        /*getDisplayName*/
        testUnistr->remove();
        logln("\n---Testing UnicodeConverter::getDisplayName...");
        myConverter->getDisplayName(CodePagesLocale[codepage_index],*testUnistr);

        /*printUChar(T_UnicodeString_getUChars(testUnistr));
        logln("\nAbove is DisplayName!!");*/


        /*getMissingUnicodeAction*/
        /*    logln("\n---Testing UnicodeConverter::getMissingUnicodeAction...");
        if ((MIA1 = myConverter->getMissingUnicodeAction()) != CodePagesMissingUnicodeAction[codepage_index]) errln("Missing action failed");
        else logln("Missing action ok");*/



        /*getMissingCharAction*/
        /*    logln("\n---Testing UnicodeConverter::getMissingCharAction...");
        if ((MIA2 = myConverter->getMissingCharAction()) != CodePagesMissingCharAction[codepage_index]) errln("Missing action failed");
        else logln("Missing action ok");*/

        myConverter->getMissingUnicodeAction(&MIA1, &MIA1Context);
        myConverter->getMissingCharAction(&MIA2, &MIA2Context);

        /*setMissingUnicodeAction*/
        logln("\n---Testing UnicodeConverter::setMissingUnicodeAction...");
        myConverter->setMissingUnicodeAction(otherUnicodeAction(MIA1), &BOM, &fromUAction, &fromUContext, err);
        if (U_FAILURE(err) || fromUAction != MIA1 || fromUContext != MIA1Context)
        {
            errln ("FAILURE! " + (UnicodeString)myErrorName(err));
        }
        myConverter->getMissingUnicodeAction(&MIA1_2, &MIA1Context2);
        if (MIA1_2 != otherUnicodeAction(MIA1) || MIA1Context2 != &BOM)
        {
            errln("Missing action failed");
        }
        else
        {
            logln("Missing action ok");
        }


        logln("\n---Testing UnicodeConverter::setMissingUnicodeAction Roundtrip...");
        myConverter->setMissingUnicodeAction(MIA1, MIA1Context, &fromUAction, &fromUContext, err);
        if (U_FAILURE(err) || fromUAction != otherUnicodeAction(MIA1) || fromUContext != &BOM)
        {
            errln ("FAILURE! " + (UnicodeString)myErrorName(err));
        }
        myConverter->getMissingUnicodeAction(&MIA1_2, &MIA1Context2);
        if (MIA1_2 != MIA1 || MIA1Context2 != MIA1Context)
        {
            errln("Missing action failed");
        }
        else
        {
            logln("Missing action ok");
        }

        /*setMissingCharAction*/
        logln("\n---Testing UnicodeConverter::setMissingCharAction...");
        myConverter->setMissingCharAction(otherCharAction(MIA2), &BOM, &toUAction, &toUContext, err);
        if (U_FAILURE(err) || toUAction != MIA2 || toUContext != MIA2Context)
        {
            errln ("FAILURE! " + (UnicodeString)myErrorName(err));
        }
        myConverter->getMissingCharAction(&MIA2_2, &MIA2Context2);
        if (MIA2_2 != otherCharAction(MIA2) || MIA2Context2 != &BOM)
        {
            errln("Missing action failed");
        }
        else
        {
            logln("Missing action ok");
        }

        logln("\n---Testing UnicodeConverter::setMissingCharAction Roundtrip...");
        myConverter->setMissingCharAction(MIA2, MIA2Context, &toUAction, &toUContext, err);
        if (U_FAILURE(err) || toUAction != otherCharAction(MIA2) || toUContext != &BOM)
        {
            errln ("FAILURE! " + (UnicodeString)myErrorName(err));
        }
        myConverter->getMissingCharAction(&MIA2_2, &MIA2Context2);
        if (MIA2_2 != MIA2 || MIA2Context2 != MIA2Context)
        {
            errln("Missing action failed");
        }
        else
        {
            logln("Missing action ok");
        }


        /*getCodepage*/
        logln("\n---Testing UnicodeConverter::getCodepage...");
        cp = myConverter->getCodepage(err);
        if (U_FAILURE(err)) errln ("FAILURE! " + (UnicodeString)myErrorName(err));    
        if (cp != CodePageNumberToTest[codepage_index])
            errln("Codepage number test failed");
        else
            logln("Codepage number test OK");


        /*getCodepagePlatform*/
        logln("\n---Testing UnicodeConverter::getCodepagePlatform ...");
        if (CodePagesPlatform[codepage_index]!=myConverter->getCodepagePlatform(err))
            errln("Platform codepage test failed");
        else
            logln("Platform codepage test ok");
        if (U_FAILURE(err))
            errln ("FAILURE! " + (UnicodeString)myErrorName(err));  


        /*Reads the BOM*/
        fread(&BOM, sizeof(UChar), 1, ucs_file_in);
        if (BOM!=0xFEFF && BOM!=0xFFFE) 
        {
            errln("File Missing BOM...Bailing!");
            return;
        }


        /*Reads in the file*/
        while(!feof(ucs_file_in)&&(i+=fread(ucs_file_buffer+i, sizeof(UChar), 1, ucs_file_in)))
        {
            myUChar = ucs_file_buffer[i-1];
            
            ucs_file_buffer[i-1] = (UChar)((BOM==0xFEFF)?myUChar:((myUChar >> 8) | (myUChar << 8))); /*adjust if BIG_ENDIAN*/
        }

        myUChar = ucs_file_buffer[i-1];
        ucs_file_buffer[i-1] = (UChar)((BOM==0xFEFF)?myUChar:((myUChar >> 8) | (myUChar << 8))); /*adjust if BIG_ENDIAN Corner Case*/

        UnicodeString* uniString  = new UnicodeString(ucs_file_buffer,i);
        UnicodeString* uniString3 = new UnicodeString(ucs_file_buffer,i);
        UnicodeString* uniString2 = new UnicodeString();


        /*Calls the Conversion Routine*/
        
        testLong1 = MAX_FILE_LEN;

        logln("\n---Testing UnicodeConverter::fromUnicodeString");
        myConverter->fromUnicodeString(output_cp_buffer, testLong1, *uniString, err);
        if (U_FAILURE(err))   logln("\nFAILURE...");




        /******************************************************************
                                Checking ksc -> Unicode
        ******************************************************************/

        /*Clean up re-usable vars*/


        /*Calls the Conversion Routine*/
        /*Uni1 ----ToUnicodeString----> Cp1 ----FromUnicodeString---->Uni2*/

        logln("\n---Testing UnicodeConverter::toUnicodeString");
        myConverter->toUnicodeString(*uniString2 , output_cp_buffer, testLong1,  err);

        if (U_FAILURE(err))
            logln ("FAILURE! " + (UnicodeString)myErrorName(err));



        logln("\n---Testing UnicodeString RoundTrip ...");

        /*check if Uni1 == Uni2*/
        if (uniString->compare(*uniString2))
            errln("Equality check test failed");
        else
            logln("Equality check test ok");

        /*AIX Compiler hacks*/
        const UChar* tmp_ucs_buf = ucs_file_buffer_use; 
        //const UChar* tmp_consumedUni = NULL;

        myConverter->fromUnicode(mytarget_1,
                     mytarget + MAX_FILE_LEN,
                     tmp_ucs_buf,
                     ucs_file_buffer_use+i,
                     NULL,
                     true,
                     err);
        //    consumedUni = (UChar*)tmp_consumedUni;

        if (U_FAILURE(err))
            errln ("FAILURE! " + (UnicodeString)myErrorName(err));

        /*Uni1 ----ToUnicode----> Cp2 ----FromUnicode---->Uni3*/

        /*AIX Compiler hacks*/
        const char* tmp_mytarget_use = mytarget_use;
        const char* tmp_consumed = consumed;
        myConverter->toUnicode(my_ucs_file_buffer_1,
                   my_ucs_file_buffer + MAX_FILE_LEN,
                   tmp_mytarget_use,
                   mytarget_use+strlen((char*)mytarget_use),
                   NULL,
                   false,
                   err);
        consumed = (char*)tmp_consumed;
        if (U_FAILURE(err))
            errln ("FAILURE! " + (UnicodeString)myErrorName(err));


        logln("\n---Testing UChar* RoundTrip ...");
        //uniString3->remove();
        uniString3->replace(0, uniString3->length(), my_ucs_file_buffer, i);
        //uniString3 = new UnicodeString(my_ucs_file_buffer,i);

        /*checks if Uni1 == Uni3*/ 
        if (uniString->compare(*uniString3))
            errln("Equality test failed line "  + UnicodeString() + 500);
        else
            logln("Equality test ok");

        /*checks if Uni2 == Uni3 This is a sanity check for the consistency of the
        UnicodeString and Unicode Convters*/ 
        logln("\n---Testing Consistency between UChar* and UnicodeString Conversion...");
        if (uniString2->compare(*uniString3))
            errln("Equality test failed line "  + UnicodeString() + 506);
        else
            logln("Equality test ok");

        logln("\n---Testing Regression 1100057 ...");
        const uint8_t mySJIS[12] = {0xFA, 0X51, 0XB8, 0XDB, 0XBD, 0XCB, 0XDB, 0XCC, 0XDE, 0XD0 , 0XFA, 0X50};


        UnicodeConverter SJIS(943, UCNV_IBM, err);
        UnicodeString myString;


        SJIS.toUnicodeString(myString, (const char *)mySJIS, 12, err);
        if (U_FAILURE(err)||(myString.length()!=10))
            errln("toUnicodeString test failed");
        else
            logln("toUnicodeString test ok");

        fclose(ucs_file_in);    
        delete myConverter;
        delete uniString;
        delete uniString2;
        delete uniString3;
        delete testUnistr;

    }
    /******* testing for Bug 778 *******************/
    {
        logln("\n---Testing Jitterbug 778 ...");
        UErrorCode err = U_ZERO_ERROR;
        UBool passed = true;
        UnicodeString unicode_string((UChar)0x592a);
        UErrorCode error = U_ZERO_ERROR;
        UnicodeConverter cnv( "iso-2022-jp-2", error );
        if( U_FAILURE(err) ) { 
            errln(UnicodeString("Error Creating iso-2022-jp-2 converter. Reason: ") + myErrorName(error));
        }
        char dest[256];
        int32_t dest_bytes_used = 256;
        cnv.fromUnicodeString( dest, dest_bytes_used, unicode_string, error );
        if( U_FAILURE(err) ) { 
            errln( UnicodeString("Error converting to iso-2022-jp-2 stream:" )+ myErrorName(error) );
        }
        const char* expected ="\x1b\x24\x42\x42\x40";
        char* got = &dest[0];
        while(*expected!='\0'){
            if(*got!=*expected){
                errln(UnicodeString("Error while testing jitterbug 778. Expected: ") + (char) *expected + " got: " + (char) *got);
                passed =false;
            }
            got++;
            expected++;
        }
        if(passed){
            logln("jitterbug 778 test ok");
        }
    }
    delete []mytarget;
    delete []output_cp_buffer;
    delete []ucs_file_buffer;
    delete []my_ucs_file_buffer;
}

void WriteToFile(const UnicodeString *a, FILE *myfile)
{
    uint32_t size = a->length();
    uint16_t i    = 0;
    UChar    b    = 0xFEFF;

    /*Writes the BOM*/
    fwrite(&b, sizeof(UChar), 1, myfile);
    for (i=0; i< size; i++)
    {
        b = (*a)[i];
        fwrite(&b, sizeof(UChar), 1, myfile);
    }
}

int32_t strCaseIcmp(const char* a1, const char * a2)
{
    int32_t i=0, ret=0;
    while(a1[i]&&a2[i]) 
    {
        ret += tolower(a1[i])-tolower(a2[i]); 
        i++;
    }
    return ret;
}

UConverterFromUCallback otherUnicodeAction(UConverterFromUCallback MIA)
{
    return (MIA==(UConverterFromUCallback)UCNV_FROM_U_CALLBACK_STOP)?(UConverterFromUCallback)UCNV_FROM_U_CALLBACK_SUBSTITUTE:(UConverterFromUCallback)UCNV_FROM_U_CALLBACK_STOP;
}

UConverterToUCallback otherCharAction(UConverterToUCallback MIA)
{
    return (MIA==(UConverterToUCallback)UCNV_TO_U_CALLBACK_STOP)?(UConverterToUCallback)UCNV_TO_U_CALLBACK_SUBSTITUTE:(UConverterToUCallback)UCNV_TO_U_CALLBACK_STOP;
}

/* Test is also located in nucnvtst.c */
void ConvertTest::TestAmbiguous() 
{
    UErrorCode status = U_ZERO_ERROR;
    UnicodeConverter *ascii_cnv = 0, *sjis_cnv = 0;
    const char target[] = {
        /* "\\usr\\local\\share\\data\\icutest.txt" */
        0x5c, 0x75, 0x73, 0x72,
        0x5c, 0x6c, 0x6f, 0x63, 0x61, 0x6c,
        0x5c, 0x73, 0x68, 0x61, 0x72, 0x65,
        0x5c, 0x64, 0x61, 0x74, 0x61,
        0x5c, 0x69, 0x63, 0x75, 0x74, 0x65, 0x73, 0x74, 0x2e, 0x74, 0x78, 0x74,
        0
    };
    UnicodeString asciiResult, sjisResult;
    
    sjis_cnv = new UnicodeConverter("ibm-943", status);
    if (U_FAILURE(status))
    {
        errln("Failed to create a SJIS converter\n");
        return;
    }
    ascii_cnv = new UnicodeConverter("LATIN-1", status);
    if (U_FAILURE(status))
    {
        errln("Failed to create a SJIS converter\n");
        delete sjis_cnv;
        return;
    }
    /* convert target from SJIS to Unicode */
    sjis_cnv->toUnicodeString(sjisResult, target, (int32_t)uprv_strlen(target), status);
    if (U_FAILURE(status))
    {
        errln("Failed to convert the SJIS string.\n");
        delete sjis_cnv;
        delete ascii_cnv;
        return;
    }

    /* convert target from Latin-1 to Unicode */
    ascii_cnv->toUnicodeString(asciiResult, target, (int32_t)uprv_strlen(target), status);
    if (U_FAILURE(status))
    {
        errln("Failed to convert the Latin-1 string.\n");
        delete sjis_cnv;
        delete ascii_cnv;
        return;
    }
    
    if (!sjis_cnv->isAmbiguous())
    {
        errln("SJIS converter should contain ambiguous character mappings.\n");
        delete sjis_cnv;
        delete ascii_cnv;
        return;
    }
    if (sjisResult == asciiResult)
    {
        errln("File separators for SJIS don't need to be fixed.\n");
    }
    sjis_cnv->fixFileSeparator(sjisResult);
    if (sjisResult != asciiResult)
    {
        errln("Fixing file separator for SJIS failed.\n");
    }
    delete sjis_cnv;
    delete ascii_cnv;

}
#endif
