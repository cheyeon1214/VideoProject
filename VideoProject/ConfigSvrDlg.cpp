// ConfigSvrDlg.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "ConfigSvrDlg.h"

// CConfigSvrDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CConfigSvrDlg, CDialog)
CConfigSvrDlg::CConfigSvrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigSvrDlg::IDD, pParent)
{
	m_hIcon = NULL;
	m_hIcon2 = NULL;
}

CConfigSvrDlg::~CConfigSvrDlg()
{
}

void CConfigSvrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_CHECK_USE_CAMERA, m_UseCam_btn);
	DDX_Control(pDX, IDC_EDIT_INDEX, m_Index_edt);
	DDX_Control(pDX, IDC_EDIT_WIDTH, m_Width_edt);
	DDX_Control(pDX, IDC_EDIT_HEIGHT, m_Height_edt);
	DDX_Control(pDX, IDC_CHECK_USE_SERVER, m_UseSvr_btn);
	DDX_Control(pDX, IDC_EDIT_IP, m_IP_edt);
	DDX_Control(pDX, IDC_EDIT_PORT, m_Port_edt);
	DDX_Control(pDX, IDC_BTN_OK, m_OK_btn);
	DDX_Control(pDX, IDC_BTN_CANCEL, m_Cancel_btn);
}


BEGIN_MESSAGE_MAP(CConfigSvrDlg, CDialog)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_OK, OnBnClickedBtnOK)
	ON_BN_CLICKED(IDC_BTN_CANCEL, OnBnClickedBtnCancel)
END_MESSAGE_MAP()


// CConfigSvrDlg 메시지 처리기입니다.

BOOL CConfigSvrDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	if(Initialize(TRUE) == FALSE)
	{
		Initialize(FALSE);
		return FALSE;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CConfigSvrDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	Initialize(FALSE);
}

BOOL CConfigSvrDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message==WM_KEYDOWN)
	{
		switch(pMsg->wParam)
		{
		case VK_ESCAPE:
		case VK_RETURN: return TRUE; break;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CConfigSvrDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	WORD wNotifyCode = HIWORD(wParam);

	if(wNotifyCode == BN_CLICKED)
	{
		int nID = (int)LOWORD(wParam);
	}

	return CDialog::OnCommand(wParam, lParam);
}

BOOL CConfigSvrDlg::Initialize(BOOL fInitialize)
{
	if(fInitialize)
	{
		WCHAR szPath[MAX_PATH];
		swprintf_s(szPath, L"%s\\Dlg_Setting.ico", _cmn_IcoPath());
		m_hIcon = (HICON)::LoadImage(NULL, szPath, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
		m_hIcon2 = (HICON)::LoadImage(NULL, szPath, IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
		SetIcon(m_hIcon, TRUE);
		SetIcon(m_hIcon2, FALSE);

		WCHAR szCfg[64];

		m_UseCam_btn.SetCheck(m_cfg.bUseCam);
		swprintf_s(szCfg, L"%d", m_cfg.index);
		m_Index_edt.SetWindowText(szCfg);
		swprintf_s(szCfg, L"%d", m_cfg.hsize);
		m_Width_edt.SetWindowText(szCfg);
		swprintf_s(szCfg, L"%d", m_cfg.vsize);
		m_Height_edt.SetWindowText(szCfg);

		m_UseSvr_btn.SetCheck(m_cfg.bUseSvr);
		m_IP_edt.SetWindowText(m_cfg.szIP);
		swprintf_s(szCfg, L"%d", m_cfg.iPort);
		m_Port_edt.SetWindowText(szCfg);
	}
	else
	{
		if(m_hIcon) DestroyIcon(m_hIcon);
		if(m_hIcon2) DestroyIcon(m_hIcon2);
	}

	return TRUE;
}

void CConfigSvrDlg::OnBnClickedBtnOK()
{
	WCHAR szCfg[64];

	m_cfg.bUseCam = m_UseCam_btn.GetCheck();
	swprintf_s(szCfg, L"%d", m_cfg.index);
	m_Index_edt.GetWindowText(szCfg, 64);
	m_cfg.index = _wtoi(szCfg);
	m_Width_edt.GetWindowText(szCfg, 64);
	m_cfg.hsize = _wtoi(szCfg);
	m_Height_edt.GetWindowText(szCfg, 64);
	m_cfg.vsize = _wtoi(szCfg);

	m_cfg.bUseSvr = m_UseSvr_btn.GetCheck();
	m_IP_edt.GetWindowText(m_cfg.szIP, 64);
	m_Port_edt.GetWindowText(szCfg, 64);
	m_cfg.iPort = _wtoi(szCfg);

	EndDialog(IDOK);
}
void CConfigSvrDlg::OnBnClickedBtnCancel()
{
	EndDialog(IDCANCEL);
}
