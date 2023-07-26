// VinputControl.cpp
//

#include "pch.h"
#include "VinputControl.h"

#include "ServerDlg.h"

VinputControl::VinputControl(CServerDlg* pDlg)
{
	m_pDlg = pDlg;

	m_index = 0;
	m_hsize = 0;
	m_vsize = 0;

	m_bDev = FALSE;
	m_hDevThread = NULL;
	m_hDevThreadEnd = CreateEvent(NULL, FALSE, FALSE, NULL);
}
VinputControl::~VinputControl()
{
	if(m_hDevThreadEnd)
	{
		CloseHandle(m_hDevThreadEnd);
		m_hDevThreadEnd = NULL;
	}
}

BOOL VinputControl::Init(int index, int hsize, int vsize)
{
	m_index = index;
	m_hsize = hsize;
	m_vsize = vsize;

	m_bDev = TRUE;
	m_hDevThread = (HANDLE)_beginthread(::DevThread, 0, this);

	return TRUE;
}
void VinputControl::Exit()
{
	if(m_bDev)
	{
		m_bDev = FALSE;
		if(m_hDevThread)
		{
			WaitForSingleObject(m_hDevThreadEnd, 1000);
			if(m_hDevThread)
			{
				DWORD ExitCode = 0;
				if(::GetExitCodeThread(m_hDevThread, &ExitCode))
				{
					TerminateThread(m_hDevThread, ExitCode);
					CloseHandle(m_hDevThread);
				}
				m_hDevThread = NULL;
			}
		}
	}
}

static void DevThread(void* Ptr)
{
	((VinputControl *) Ptr)->DevThread();
}
void VinputControl::DevThread()
{
	VideoCapture cap(m_index);
	cap.set(CAP_PROP_FRAME_WIDTH, m_hsize);
	cap.set(CAP_PROP_FRAME_HEIGHT, m_vsize);

	if(!cap.isOpened())
	{
		m_hDevThread = NULL;
		m_bDev = FALSE;
		return;
	}

	Mat bgrImg, yuvImg;
	while(m_bDev)
	{
		cap.read(bgrImg);
		if(!m_bDev) break;

		cvtColor(bgrImg, yuvImg, COLOR_BGR2YUV_I420);

		VID_IMGSRC stVid_img;
		stVid_img.f_col = FOURCC_YV12;
		stVid_img.hsize = m_hsize;
		stVid_img.vsize = m_vsize;
		stVid_img.y_src = yuvImg.data;
		stVid_img.y_stride = (int)yuvImg.step;
		stVid_img.u_src = stVid_img.y_src + (stVid_img.y_stride * stVid_img.vsize);
		stVid_img.v_src = stVid_img.u_src + (stVid_img.y_stride * stVid_img.vsize / 4);
		stVid_img.uv_stride = stVid_img.y_stride / 2;

		if(m_pDlg)
		{
			m_pDlg->SendRawImgSrc(&stVid_img);
		}
	}

	m_hDevThread = NULL;
	if(m_bDev)
		m_bDev = FALSE;
	else SetEvent(m_hDevThreadEnd);
}
