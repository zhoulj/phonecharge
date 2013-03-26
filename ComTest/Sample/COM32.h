// COM32.h : main header file for the COM32 application
//

#if !defined(AFX_COM32_H__5F8821A9_8A0A_4080_8EC7_74E34EB85210__INCLUDED_)
#define AFX_COM32_H__5F8821A9_8A0A_4080_8EC7_74E34EB85210__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCOM32App:
// See COM32.cpp for the implementation of this class
//

class CCOM32App : public CWinApp
{
public:
	CCOM32App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCOM32App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CCOM32App)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COM32_H__5F8821A9_8A0A_4080_8EC7_74E34EB85210__INCLUDED_)
