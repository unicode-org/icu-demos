/*
 ******************************************************************************
 * Copyright (C) 2004-2007, International Business Machines Corporation and   *
 * others. All Rights Reserved.                                               *
 ******************************************************************************
 */

#if !defined(AFX_COMBOLISTDLG_H__FCBC7735_458C_11D2_8236_00A02401A257__INCLUDED_)
#define AFX_COMBOLISTDLG_H__FCBC7735_458C_11D2_8236_00A02401A257__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CComboListDlg dialog

class CComboListDlg : public CDialog
{

// Construction
public:
	CComboListDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CComboListDlg)
	enum { IDD = IDD_COMBOLIST_DIALOG };
	CComboBox	m_comboList;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComboListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
    
	// Generated message map functions
	//{{AFX_MSG(CComboListDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnCloseupCombo();
	afx_msg void OnEditupdateCombo();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
    CString getFortune(CString& locale);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMBOLISTDLG_H__FCBC7735_458C_11D2_8236_00A02401A257__INCLUDED_)
