// COM32Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "COM32.h"
#include "COM32Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCOM32Dlg dialog

CCOM32Dlg::CCOM32Dlg(CWnd* pParent /*=NULL*/)
: CDialog(CCOM32Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCOM32Dlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCOM32Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCOM32Dlg)
	DDX_Control(pDX, IDC_BTN_COMSTOP, m_btn_comstop);
	DDX_Control(pDX, IDC_BTN_COMSTART, m_btn_comstart);
	DDX_Control(pDX, IDC_BTN_SEND, m_btn_send);
	DDX_Control(pDX, IDC_LIST_RECV, m_list_recv);
	DDX_Control(pDX, IDC_COMLIST, m_cmb_comlist);
  DDX_Control(pDX, IDC_BUTTON1, m_btn_at);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCOM32Dlg, CDialog)
//{{AFX_MSG_MAP(CCOM32Dlg)
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_BN_CLICKED(IDC_BTN_SEND, OnBtnSend)
ON_BN_CLICKED(IDC_BTN_COMSTART, OnBtnComstart)
ON_BN_CLICKED(IDC_BTN_COMSTOP, OnBtnComstop)
//}}AFX_MSG_MAP
ON_MESSAGE(PWM_GOTCOMMDATA,OnReceiveData)
ON_BN_CLICKED(IDC_BUTTON1, &CCOM32Dlg::OnBnClickedButton1)
ON_BN_CLICKED(IDC_BUTTON2, &CCOM32Dlg::OnBnClickedButton2)
ON_BN_CLICKED(IDC_BUTTON3, &CCOM32Dlg::OnBnClickedButton3)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCOM32Dlg message handlers

BOOL CCOM32Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	com1=new CSerialCom(this->m_hWnd);
	FindCom();
	try{
		m_cmb_comlist.SetCurSel(0);
	}
	catch(...){}
	m_btn_comstart.EnableWindow(TRUE);
	m_btn_comstop.EnableWindow(FALSE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCOM32Dlg::OnPaint() 
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
HCURSOR CCOM32Dlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CCOM32Dlg::FindCom()
{
	//枚举当前系统中的串口
	LONG res = 0;
	HKEY key = NULL;
	
	res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_ALL_ACCESS, &key);
	if( res )
		return ;
	
	TCHAR keyname[200];
	TCHAR data[200];
	DWORD keynamelen = 0;
	DWORD datalen = 0;
	
	int index = 0;
	
	while(1)
	{
		keynamelen = 200;
		datalen = 200;
		res = RegEnumValue(key, index++, keyname, &keynamelen, NULL, NULL, (LPBYTE)data, &datalen);
		
		if( res )
			break;
		m_cmb_comlist.AddString(data);
	}	
	RegCloseKey(key);
}

LRESULT CCOM32Dlg::OnReceiveData(WPARAM w,LPARAM l)
{
	char * buff=(char*)l;
	DWORD bufsize=w;
	m_list_recv.AddString(buff);
  return 0;
}

void CCOM32Dlg::OnBtnSend() 
{
	// TODO: Add your control notification handler code here

	com1->WriteCommData("AT\r\n", 11);
}

void CCOM32Dlg::OnBtnComstart() 
{
	// TODO: Add your control notification handler code here
	CString strcomname;
	m_cmb_comlist.GetLBText(m_cmb_comlist.GetCurSel(),strcomname);
	com1->SetCommName(strcomname);
	if(com1->StartComm())
	{
		m_btn_comstart.EnableWindow(FALSE);
		m_btn_comstop.EnableWindow(TRUE);
	}
}

void CCOM32Dlg::OnBtnComstop() 
{
	// TODO: Add your control notification handler code here
	com1->StopComm();
	m_btn_comstart.EnableWindow(TRUE);
	m_btn_comstop.EnableWindow(FALSE);
}

void CCOM32Dlg::OnBnClickedButton1()
{
  // TODO: 在此添加控件通知处理程序代码
  com1->WriteCommData("AT\r\n", 11);
}

void CCOM32Dlg::OnBnClickedButton2()
{
  // TODO: 在此添加控件通知处理程序代码
  com1->WriteCommData("ATD10010;\r\n", 11);
}

void CCOM32Dlg::OnBnClickedButton3()
{
  // TODO: 在此添加控件通知处理程序代码
  com1->WriteCommData("ATH\r\n", 11);
}
