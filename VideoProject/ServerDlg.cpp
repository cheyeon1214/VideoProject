// ServerDlg.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "ServerDlg.h"

#include "VideoProjectDlg.h"

#include "ConfigSvrDlg.h"

enum MenuMsg
{
	IDM_SETTING = 20000,
};

// CServerDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CServerDlg, CDialog)

CServerDlg::CServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CServerDlg::IDD, pParent)
{
	m_bInitial = FALSE;
	m_pMainDlg = NULL;

	m_pdx = NULL;
	m_pCamDrv = NULL;

	m_bufVid = NULL;
	m_encVid = NULL;

	m_sockSvr = INVALID_SOCKET;

	m_bAcpt = FALSE;
	m_hAcptThread = NULL;
	m_hAcptThreadEnd = CreateEvent(NULL, FALSE, FALSE, NULL);
}

CServerDlg::~CServerDlg()
{
	if(m_hAcptThreadEnd)
	{
		CloseHandle(m_hAcptThreadEnd);
		m_hAcptThreadEnd = NULL;
	}
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CServerDlg, CDialog)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()


// CServerDlg 메시지 처리기입니다.

BOOL CServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	if(Initialize(TRUE) == FALSE)
	{
		Initialize(FALSE);
		return FALSE;
	}

	m_bInitial = TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CServerDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	m_bInitial = FALSE;

	Initialize(FALSE);
}

void CServerDlg::OnPaint()
{
	CPaintDC dc(this);
	
	if(m_bInitial)
	{
		if(m_pdx)
		{
			m_pdx->Send_Refresh();
		}
	}
}

void CServerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(!m_bInitial)
		return;
	if(nType == SIZE_RESTORED)
	{
		if(m_pdx)
		{
			m_pdx->ChangeSplit();
		}
	}
}

void CServerDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	CDialog::OnRButtonUp(nFlags, point);

	if(m_bInitial)
	{
		CMenu menu;
		menu.CreatePopupMenu();
		UINT nPos = 0;

		menu.InsertMenu(nPos++, MF_BYPOSITION, IDM_SETTING, L"Settings");

		CPoint pt(point);
		ClientToScreen(&pt);
		menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);
	}
}

BOOL CServerDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message==WM_KEYDOWN)
	{
		switch(pMsg->wParam)
		{
		case VK_ESCAPE:
		case VK_RETURN:
		case VK_SPACE: return TRUE; break;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}
BOOL CServerDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	WORD wNotifyCode = HIWORD(wParam);

	if(wNotifyCode == BN_CLICKED)
	{
		int nID = (int)LOWORD(wParam);
		switch(nID)
		{
		case IDM_SETTING:
			{
				ConfigSvr cfg; cfg = *GetCfg();

				CConfigSvrDlg dlg; dlg.Init(&cfg);

				if(dlg.DoModal() == IDOK)
				{
					cfg = *dlg.GetCfg();
					if(m_cfg != cfg)
					{
						if(SetCfg(&cfg)) m_cfg = cfg;
					}
				}
			}
			break;
		}
	}

	return CDialog::OnCommand(wParam, lParam);
}

BOOL CServerDlg::Initialize(BOOL fInitialize)
{
	if(fInitialize)
	{
		m_pdx = new dxs_control;
		if(m_pdx->Init(this))
		{
			m_pdx->SendCleanMsg(SCR_IMG_LOGO);
		}
		else
		{
			m_pdx->Exit();
			delete m_pdx;
			m_pdx = NULL;
		}

		SetCfg(&m_cfg);
	}
	else
	{
		CLOSE();

		if(m_pCamDrv)
		{
			m_pCamDrv->Exit();
			delete m_pCamDrv;
			m_pCamDrv = NULL;
		}
		if(m_encVid != NULL)
		{
			m_encVid->Exit();
			delete m_encVid;
			m_encVid = NULL;
		}
		if(m_pdx)
		{
			m_pdx->Exit();
			delete m_pdx;
			m_pdx = NULL;
		}
		SAFE_DELETEA(m_bufVid);
	}
	return TRUE;
}

BOOL CServerDlg::Create(CVideoProjectDlg* pParent)
{
	m_pMainDlg = pParent;
	return CDialog::Create(CServerDlg::IDD, pParent);
}

void CServerDlg::SendRawImgSrc(VID_IMGSRC* pVid_img)
{
	if(!m_bInitial)
		return;
	if(m_pdx)
	{
		m_pdx->SendImageSrc(pVid_img);
	}
	if(m_bufVid)
	{
		AVS_PACKET* pAVS_dat = (AVS_PACKET*)m_bufVid;
		if(m_encVid)
		{
			int nOutSize = MAX_VIDEO_SIZE; BOOL bRecvI = FALSE;
			if(m_encVid->Encode(pVid_img, pAVS_dat->Payload, nOutSize, bRecvI))
			{
				pAVS_dat->SetType(CD_VIDEO);
				pAVS_dat->SetSize(nOutSize);
				pAVS_dat->SetInfo(0, bRecvI ? I_TYPE : P_TYPE);

				if(m_pMainDlg)
				{
					m_pMainDlg->SendMpgDatSrc(pAVS_dat);
				}
			}
		}
	}
}
void CServerDlg::CLOSE()
{
	if(m_bAcpt)
	{
		m_bAcpt = FALSE;
		SAFE_CLOSESOCK(m_sockSvr);
		if(m_hAcptThread)
		{
			WaitForSingleObject(m_hAcptThreadEnd, 1000);
			if(m_hAcptThread)
			{
				DWORD ExitCode = 0;
				if(::GetExitCodeThread(m_hAcptThread, &ExitCode))
				{
					TerminateThread(m_hAcptThread, ExitCode);
					CloseHandle(m_hAcptThread);
				}
				m_hAcptThread = NULL;
			}
		}
	}
}

BOOL CServerDlg::SetCfg(ConfigSvr* pCfg)
{
	CLOSE();
	if(m_pCamDrv)
	{
		m_pCamDrv->Exit();
		delete m_pCamDrv;
		m_pCamDrv = NULL;
	}
	if(m_encVid)
	{
		m_encVid->Exit();
		delete m_encVid;
		m_encVid = NULL;
	}
	SAFE_DELETEA(m_bufVid);

	if(m_pdx)
	{
		m_pdx->SendCleanMsg(SCR_IMG_LOGO);
	}

	if(pCfg->bUseCam)
	{
		m_encVid = new H264ENC;
		if(!m_encVid->Init(pCfg->hsize, pCfg->vsize, 30, 4000000))
		{
			m_encVid->Exit();
			delete m_encVid;
			m_encVid = NULL;
		}
		m_bufVid = new uint8[sizeof(AVS_HDR) + MAX_VIDEO_SIZE];

		m_pCamDrv = new VinputControl(this);
		if(!m_pCamDrv->Init(pCfg->index, pCfg->hsize, pCfg->vsize))
		{
			m_pCamDrv->Exit();
			delete m_pCamDrv;
			m_pCamDrv = NULL;
		}
	}
	if(pCfg->bUseSvr)
	{
		m_sockSvr = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		if(m_sockSvr != INVALID_SOCKET)
		{
			char szIP[64];
			WideCharToMultiByte(CP_ACP, 0, pCfg->szIP, -1, szIP, 64, NULL, NULL);

			if(!tcp_listen(m_sockSvr, szIP, pCfg->iPort))
				SAFE_CLOSESOCK(m_sockSvr);
		}
		if(m_sockSvr != INVALID_SOCKET)
		{
			m_bAcpt = TRUE;
			ResetEvent(m_hAcptThreadEnd);
			m_hAcptThread = (HANDLE)_beginthread(::AcceptThread, 0, this);
		}
	}
	return TRUE;
}

static void AcceptThread(void* Ptr)
{
	((CServerDlg*)Ptr)->AcceptThread();
}
void CServerDlg::AcceptThread()
{
	if(!m_bAcpt) // if close
	{
		m_hAcptThread = NULL;
		SetEvent(m_hAcptThreadEnd);
		return;
	}

	SOCKET sdAccept = INVALID_SOCKET;
	SOCKADDR_IN	addrFrom;
	int nAddFromLen = sizeof(addrFrom);

	while(m_bAcpt)
	{
		nAddFromLen = sizeof(addrFrom);
		memset(&addrFrom, 0, sizeof(addrFrom));
		sdAccept = accept(m_sockSvr, (LPSOCKADDR)&addrFrom, &nAddFromLen);
		if(m_bAcpt && sdAccept != INVALID_SOCKET)
		{
			bool b_accept = false;
			if(m_pMainDlg)
			{
				VMS_header hdr;
				if(m_pMainDlg->ACCEPT(sdAccept))
				{
					b_accept = true;
					hdr.cmd = CMD_OK;
					tcp_send(sdAccept, (char*)&hdr, sizeof(VMS_header));
				}
				else
				{
					hdr.cmd = CMD_FAIL;
					tcp_send(sdAccept, (char*)&hdr, sizeof(VMS_header));
				}
			}
			if(!b_accept)
			{
				shutdown(sdAccept, SD_SEND);
				SAFE_CLOSESOCK(sdAccept);
			}
		}
	}

	SAFE_CLOSESOCK(m_sockSvr);

	m_hAcptThread = NULL;
	if(m_bAcpt)
		m_bAcpt = FALSE;
	else SetEvent(m_hAcptThreadEnd);
}
