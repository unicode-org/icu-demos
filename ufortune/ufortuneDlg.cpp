/*
 ******************************************************************************
 * Copyright (C) 2004-2007, International Business Machines Corporation and   *
 * others. All Rights Reserved.                                               *
 ******************************************************************************
 */

#include "stdafx.h"
#include "ufortune.h"
#include "ufortuneDlg.h"

#include <time.h>
#include "unicode/udata.h"     /* ICU API for data handling.                 */
#include "unicode/ures.h"      /* ICU API for resource loading               */
#include "unicode/putil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CComboListDlg dialog

CComboListDlg::CComboListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CComboListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CComboListDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CComboListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CComboListDlg)
	DDX_Control(pDX, IDC_COMBO, m_comboList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CComboListDlg, CDialog)
	//{{AFX_MSG_MAP(CComboListDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_CLOSEUP(IDC_COMBO, OnCloseupCombo)
	ON_CBN_EDITUPDATE(IDC_COMBO, OnEditupdateCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComboListDlg message handlers

BOOL CComboListDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, true);			// Set big icon
	SetIcon(m_hIcon, false);		// Set small icon
	
	// TODO: Add extra initialization here
    m_comboList.AddString( L"root" );
    m_comboList.AddString( L"en" );
    m_comboList.AddString( L"te" );
    m_comboList.AddString( L"de" );
    m_comboList.AddString( L"es" );
	
	return true;  // return true  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CComboListDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CComboListDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CComboListDlg::OnCloseupCombo() 
{
	// TODO: Add your control notification handler code here
    CString     szChoice;
    CString     szResult;
    int         nChoice;

    // Get current selections from edit and list-box controls
    m_comboList.GetWindowText( szChoice );
    nChoice = m_comboList.GetCurSel();

    if( nChoice != CB_ERR )
    {
        // If a valid choice was made from the list box, fetch
        // the item's text string.
        m_comboList.GetLBText( nChoice, szChoice );
        szResult = getFortune( szChoice);
    }
    else if( szChoice.IsEmpty() == true )
    {
        // No choice was made from the list box, and the edit
        // control was empty.
        szResult = "No choice selected";
    }
    else if( m_comboList.FindStringExact(-1, szChoice) != CB_ERR )
    {
        // The string from the edit control was found in the
        // list box.
        szResult = getFortune(szChoice);
    }
    else
    {
        // The edit control contains a new string, not currently
        // in the list box. Add the string.
        m_comboList.AddString( szChoice );
        szResult = "Adding " + szChoice + " to list";
    }

    // Get a pointer to the static-text control, and display an
    // appropriate result message.
    CWnd* pWnd = GetDlgItem( IDC_RESULT );
    ASSERT( pWnd );
    if( pWnd )
        if(szChoice == "te"){
	        CFont *m_Font1 = new CFont;
	        BOOL retval = m_Font1->CreatePointFont(120, L"TeluguMT");
            if(retval==0){
                CString szMessage;
                szMessage.Format(L"Could not create the font. Return Value: %i", retval);
                AfxMessageBox( szMessage );
            }
            pWnd->SetFont(m_Font1);
        }
        pWnd->SetWindowTextW(szResult );
}

void CComboListDlg::OnEditupdateCombo() 
{
	// TODO: Add your control notification handler code here
    CString     szChoice;
    CString     szResult;

    m_comboList.GetWindowText( szChoice );
    szResult = "Choice changed to " + szChoice;

    CWnd* pWnd = GetDlgItem( IDC_RESULT );
    ASSERT( pWnd );
    if( pWnd )
        pWnd->SetWindowText( szResult );
}

void CComboListDlg::OnOK() 
{
	// TODO: Add extra validation here
    CString szCombo;

    m_comboList.GetWindowText( szCombo );
    AfxMessageBox( szCombo );

    int nChoice = m_comboList.GetCurSel();
    szCombo.Format( L"The current selection is %d", nChoice );
    AfxMessageBox( szCombo );

    CDialog::OnOK();
}


#ifndef UFORTUNE_NOSETAPPDATA
/*
 *  Resource Data Reference.  The data is packaged as a dll (or .so or
 *           whatever, depending on the platform) that exports a data
 *           symbol.  The application (that's us) references that symbol,
 *           here, and will pass the data address to ICU, which will then
 *           be able to fetch resources from the data.
 */
extern "C" {
    extern  const void U_IMPORT *ufortuneRes_dat;
}
#endif


CString CComboListDlg::getFortune(CString& locale)
{
    
    UErrorCode         err           = U_ZERO_ERROR;   /* Error return, used for most ICU     */
                                                       /*   functions.                        */

    UResourceBundle   *myResources;              /* ICU Resource "handles"                    */
    UResourceBundle   *fortunes_r;

    int32_t            numFortunes;              /* Number of fortune strings available.      */
    int                i;

    const UChar       *resString;                /* Points to strings fetched from Resources. */
    int32_t            len;

    CString szMessage;
    const char* programName ="ufortune";

#ifndef UFORTUNE_NOSETAPPDATA
    /* Tell ICU where our resource data is located in memory.
     *   The data lives in the Fortune_Resources dll, and we just
     *   pass the address of an exported symbol from that library
     *   to ICU.
     */
    udata_setAppData("ufortuneRes", &ufortuneRes_dat, &err);
    if (U_FAILURE(err)) {
        //fprintf(stderr, "%s: ures_open failed with error \"%s\"\n", programName, u_errorName(err));
        //exit(-1);
    }
#endif
    
    char loc[20] = {"\0"};
    u_UCharsToChars(locale.GetBuffer(10), (char*)loc, locale.GetLength());
    locale.ReleaseBuffer();

    /* Open our resources.
    */
    myResources = ures_open("ufortuneRes",loc, &err);
    if (U_FAILURE(err)) {
        szMessage.Format(L"%s: ures_open failed with error \"%s\"\n", programName, u_errorName(err));
        AfxMessageBox( szMessage );  
        return "";
    }



    /*
     * Open the "fortunes" resources from within the already open resources
     */
    fortunes_r = ures_getByKey(myResources, "fortunes", NULL, &err);
    if (U_FAILURE(err)) {
        szMessage.Format( L"%s: ures_getByKey(\"fortunes\") failed, %s\n", programName, u_errorName(err));
        AfxMessageBox( szMessage );  
        return "";
    }


    /*
     * Pick up and display a random fortune
     *
     */
    numFortunes = ures_countArrayItems(myResources, "fortunes", &err);
    if (U_FAILURE(err)) {
        szMessage.Format(L"%s: ures_countArrayItems(\"fortunes\") failed, %s\n", programName, u_errorName(err));
        AfxMessageBox( szMessage );  
        return "";
    }
    if (numFortunes <= 0) {
        szMessage.Format(L"%s: no fortunes found.\n");
        AfxMessageBox( szMessage );  
        return "";
    }

    i = time(NULL) % numFortunes;    /*  Use time to pick a somewhat-random fortune.  */
    resString = ures_getStringByIndex(fortunes_r, i, &len, &err);
    if (U_FAILURE(err)) {
       szMessage.Format(L"%s: ures_getStringByIndex(%d) failed, %s\n", programName, i, u_errorName(err));
        AfxMessageBox( szMessage );  
        return "";
    }

    return CString(resString);
}
