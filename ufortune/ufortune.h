// ComboList.h : main header file for the COMBOLIST application
//

#if !defined(AFX_COMBOLIST_H__FCBC7733_458C_11D2_8236_00A02401A257__INCLUDED_)
#define AFX_COMBOLIST_H__FCBC7733_458C_11D2_8236_00A02401A257__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CComboListApp:
// See ComboList.cpp for the implementation of this class
//

class CComboListApp : public CWinApp
{
public:
	CComboListApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComboListApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CComboListApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMBOLIST_H__FCBC7733_458C_11D2_8236_00A02401A257__INCLUDED_)
