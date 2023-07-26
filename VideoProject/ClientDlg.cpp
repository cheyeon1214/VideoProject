// ClientDlg.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "ClientDlg.h"

#include "VideoProjectDlg.h"

#include "ConnectDlg.h"

enum MenuMsg
{
	IDM_CONNECT = 20000,
	IDM_DISCONNECT,
};

// CClientDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CClientDlg, CDialog)

CClientDlg::CClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CClientDlg::IDD, pParent)
{
	m_bInitial = FALSE;
	m_pMainDlg = NULL;

	m_pdx = NULL;

	m_bStart = FALSE;
	m_bufVid = NULL;
	m_decVid = NULL;

	m_sockCnt = INVALID_SOCKET;
	*m_szIP = '\0';
	m_iPort = 0;

	m_bChild = FALSE;
	m_hChildThread = NULL;
	m_hChildThreadEnd = CreateEvent(NULL, FALSE, FALSE, NULL);

	m_bClient = FALSE;
	m_hClientThread = NULL;
	m_hClientThreadEnd = CreateEvent(NULL, FALSE, FALSE, NULL);
}

CClientDlg::~CClientDlg()
{
	if(m_hChildThreadEnd)
	{
		CloseHandle(m_hChildThreadEnd);
		m_hChildThreadEnd = NULL;
	}
	if(m_hClientThreadEnd)
	{
		CloseHandle(m_hClientThreadEnd);
		m_hClientThreadEnd = NULL;
	}
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CClientDlg, CDialog)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()


// CClientDlg 메시지 처리기입니다.

BOOL CClientDlg::OnInitDialog()
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

void CClientDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	m_bInitial = FALSE;

	Initialize(FALSE);
}

void CClientDlg::OnPaint()
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

void CClientDlg::OnSize(UINT nType, int cx, int cy)
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

void CClientDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	CDialog::OnRButtonUp(nFlags, point);

	if(m_bInitial)
	{
		CMenu menu;
		menu.CreatePopupMenu();
		UINT nPos = 0;

		menu.InsertMenu(nPos++, MF_BYPOSITION, IDM_CONNECT, L"Connect");
		menu.InsertMenu(nPos++, MF_BYPOSITION, IDM_DISCONNECT, L"Disconnect");

		CPoint pt(point);
		ClientToScreen(&pt);
		menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);
	}
}

BOOL CClientDlg::PreTranslateMessage(MSG* pMsg)
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
BOOL CClientDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	WORD wNotifyCode = HIWORD(wParam);

	if(wNotifyCode == BN_CLICKED)
	{
		int nID = (int)LOWORD(wParam);
		switch(nID)
		{
		case IDM_CONNECT:
			{
				CConnectDlg dlg;
				dlg.Init(L"127.0.0.1", 12000);
				if(dlg.DoModal() == IDOK)
				{
					CONNECT(dlg.GetIP(), dlg.GetPort());
				}
			}
			break;
		case IDM_DISCONNECT:
			{
				CLOSE();
			}
			break;
		}
	}

	return CDialog::OnCommand(wParam, lParam);
}

BOOL CClientDlg::Initialize(BOOL fInitialize)
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

		m_decVid = new H264DEC;
		if(!m_decVid->Init())
		{
			m_decVid->Exit();
			delete m_decVid;
			m_decVid = NULL;
		}
		m_bufVid = new uint8[sizeof(AVS_HDR) + MAX_VIDEO_SIZE];
	}
	else
	{
		CLOSE();

		if(m_decVid != NULL)
		{
			m_decVid->Exit();
			delete m_decVid;
			m_decVid = NULL;
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

BOOL CClientDlg::Create(CVideoProjectDlg* pParent)
{
	m_pMainDlg = pParent;
	return CDialog::Create(CClientDlg::IDD, pParent);
}

void CClientDlg::CLOSE()
{
	if(m_bChild)
	{
		m_bChild = FALSE;
		if(m_sockCnt != INVALID_SOCKET)
		{
			VMS_header hdr;
			hdr.cmd = CMD_DISCONNECT;
			tcp_send(m_sockCnt, (char*)&hdr, sizeof(VMS_header));

			shutdown(m_sockCnt, SD_SEND);
			SAFE_CLOSESOCK(m_sockCnt);
		}
		if(m_hChildThread)
		{
			WaitForSingleObject(m_hChildThreadEnd, 1000);
			if(m_hChildThread)
			{
				DWORD ExitCode = 0;
				if(::GetExitCodeThread(m_hChildThread, &ExitCode))
				{
					TerminateThread(m_hChildThread, ExitCode);
					CloseHandle(m_hChildThread);
				}
				m_hChildThread = NULL;
			}
		}
	}
	if(m_bClient)
	{
		m_bClient = FALSE;
		if(m_sockCnt != INVALID_SOCKET)
		{
			VMS_header hdr;
			hdr.cmd = CMD_DISCONNECT;
			tcp_send(m_sockCnt, (char*)&hdr, sizeof(VMS_header));

			shutdown(m_sockCnt, SD_SEND);
			SAFE_CLOSESOCK(m_sockCnt);
		}
		if(m_hClientThread)
		{
			WaitForSingleObject(m_hClientThreadEnd, 1000);
			if(m_hClientThread)
			{
				DWORD ExitCode = 0;
				if(::GetExitCodeThread(m_hClientThread, &ExitCode))
				{
					TerminateThread(m_hClientThread, ExitCode);
					CloseHandle(m_hClientThread);
				}
				m_hClientThread = NULL;
			}
		}
	}
	m_bStart = FALSE;
	*m_szIP = '\0';
	m_iPort = 0;
}
BOOL CClientDlg::ACCEPT(SOCKET sdAccept)
{
	if(m_sockCnt != INVALID_SOCKET)
		return FALSE;

	m_sockCnt = sdAccept;

	m_bChild = TRUE;
	ResetEvent(m_hChildThreadEnd);
	m_hChildThread = (HANDLE)_beginthread(::ChildThread, 0, this);

	return TRUE;
}
BOOL CClientDlg::CONNECT(WCHAR* szIP, int iPort)
{
	if(m_sockCnt != INVALID_SOCKET)
		return FALSE;

	m_sockCnt = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if(m_sockCnt == INVALID_SOCKET) return FALSE;

	WideCharToMultiByte(CP_ACP, 0, szIP, -1, m_szIP, 64, NULL, NULL);
	m_iPort = iPort;

	m_bClient = TRUE;
	ResetEvent(m_hClientThreadEnd);
	m_hClientThread = (HANDLE)_beginthread(::ClientThread, 0, this);

	return TRUE;
}
void CClientDlg::SendMpgDatSrc(AVS_PACKET* pAVS_dat)
{
	if(!m_bChild && !m_bClient)
		return;
	if(m_sockCnt == INVALID_SOCKET)
		return;
	if(pAVS_dat->GetInfo(0) == I_TYPE)
		m_bStart = TRUE;
	if(m_bStart)
	{
		VMS_header hdr;
		hdr.cmd = CMD_SEND_STREAM;
		hdr.size = pAVS_dat->GetPacketLen();
		tcp_send(m_sockCnt, (char*)&hdr, sizeof(VMS_header));
		tcp_send(m_sockCnt, (char*)pAVS_dat, pAVS_dat->GetPacketLen());
	}
}

static void ChildThread(void* Ptr)
{
	((CClientDlg*)Ptr)->ChildThread();
}
void CClientDlg::ChildThread()
{
	if(!m_bufVid)
	{
		m_hChildThread = NULL;
		m_bChild = FALSE;
		return;
	}
	while(m_bChild)
	{
		VMS_header hdr;
		if(tcp_recv(m_sockCnt, (char*)&hdr, sizeof(VMS_header)) <= 0)
			break;
		if(strcmp(hdr.marker, "vms") != 0)
		{
			recv(m_sockCnt, (char*)m_bufVid, MAX_VIDEO_SIZE, 0);
			continue;
		}
		if(hdr.size)
		{
			if(tcp_recv(m_sockCnt, (char*)m_bufVid, hdr.size) <= 0)
				break;
		}
		if(hdr.cmd == CMD_SEND_STREAM)
		{
			AVS_PACKET* pAVS_dat = (AVS_PACKET*)m_bufVid;

			VID_IMGSRC stVid_img;
			if(m_decVid)
			{
				if(!m_decVid->Decode(pAVS_dat->Payload, pAVS_dat->GetSize(), &stVid_img))
					return;
			}
			if(stVid_img.IsImage())
			{
				if(m_pdx)
				{
					m_pdx->SendImageSrc(&stVid_img);
				}
			}
		}
		else if(hdr.cmd == CMD_DISCONNECT) break;
	}

	SAFE_CLOSESOCK(m_sockCnt);

	if(m_pdx)
	{
		m_pdx->SendCleanMsg(SCR_IMG_LOGO);
	}
	m_hChildThread = NULL;
	if(m_bChild)
		m_bChild = FALSE;
	else SetEvent(m_hChildThreadEnd);
}

static void ClientThread(void* Ptr)
{
	((CClientDlg*)Ptr)->ClientThread();
}
void CClientDlg::ClientThread()
{
	BOOL bNetCon = FALSE;
	if(tcp_connect(m_sockCnt, m_szIP, m_iPort))
		bNetCon = TRUE;
	if(!m_bClient) // if close
	{
		m_hClientThread = NULL;
		SetEvent(m_hClientThreadEnd);
		return;
	}
	if(!bNetCon)
	{
		SAFE_CLOSESOCK(m_sockCnt);
		m_hClientThread = NULL;
		m_bClient = FALSE;
		return;
	}

	bool b_result = true;
	if(b_result)
	{
		VMS_header hdr;
		if(tcp_recv(m_sockCnt, (char*)&hdr, sizeof(VMS_header)) <= 0)
			b_result = false;
		if(hdr.cmd == CMD_FAIL) b_result = false;
	}
	if(!b_result)
	{
		SAFE_CLOSESOCK(m_sockCnt);
		m_hClientThread = NULL;
		m_bClient = FALSE;
		return;
	}
	while(m_bClient)
	{
		VMS_header hdr;
		if(tcp_recv(m_sockCnt, (char*)&hdr, sizeof(VMS_header)) <= 0)
			break;
		if(strcmp(hdr.marker, "vms") != 0)
		{
			recv(m_sockCnt, (char*)m_bufVid, MAX_VIDEO_SIZE, 0);
			continue;
		}
		if(hdr.size)
		{
			if(tcp_recv(m_sockCnt, (char*)m_bufVid, hdr.size) <= 0)
				break;
		}
		if(hdr.cmd == CMD_SEND_STREAM)
		{
			AVS_PACKET* pAVS_dat = (AVS_PACKET*)m_bufVid;

			VID_IMGSRC stVid_img;
			if(m_decVid)
			{
				if(!m_decVid->Decode(pAVS_dat->Payload, pAVS_dat->GetSize(), &stVid_img))
					return;
			}
			if(stVid_img.IsImage())
			{
				if(m_pdx)
				{
					m_pdx->SendImageSrc(&stVid_img);
				}
			}
		}
		else if(hdr.cmd == CMD_DISCONNECT) break;
	}

	SAFE_CLOSESOCK(m_sockCnt);

	if(m_pdx)
	{
		m_pdx->SendCleanMsg(SCR_IMG_LOGO);
	}
	m_hClientThread = NULL;
	if(m_bClient)
		m_bClient = FALSE;
	else SetEvent(m_hClientThreadEnd);
}
