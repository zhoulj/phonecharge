// COM32Dlg.h : header file
//

#if !defined(AFX_COM32DLG_H__828582DD_7984_4807_9233_07A7086BB8A1__INCLUDED_)
#define AFX_COM32DLG_H__828582DD_7984_4807_9233_07A7086BB8A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "SerialCom.h"
/////////////////////////////////////////////////////////////////////////////
// CCOM32Dlg dialog

class CCOM32Dlg : public CDialog
{
// Construction
public:
	void FindCom();
	CCOM32Dlg(CWnd* pParent = NULL);	// standard constructor
	CSerialCom * com1;
	LRESULT OnReceiveData(WPARAM,LPARAM);
// Dialog Data
	//{{AFX_DATA(CCOM32Dlg)
	enum { IDD = IDD_COM32_DIALOG };
	CButton	m_btn_comstop;
	CButton	m_btn_comstart;
	CButton	m_btn_send;
	CListBox	m_list_recv;
	CComboBox	m_cmb_comlist;
	HANDLE	m_hRecvMessage;
  CButton m_btn_at;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCOM32Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CCOM32Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnSend();
	afx_msg void OnBtnComstart();
	afx_msg void OnBtnComstop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedButton1();
  afx_msg void OnBnClickedButton2();
  afx_msg void OnBnClickedButton3();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COM32DLG_H__828582DD_7984_4807_9233_07A7086BB8A1__INCLUDED_)
