#ifndef  _VINPUT_CONTROL_H_
#define  _VINPUT_CONTROL_H_

using namespace cv;
using namespace std;

class CServerDlg;
class VinputControl
{
private:
	int m_index;
	int m_hsize, m_vsize;

public:
	BOOL Init(int index, int hsize, int vsize);
	void Exit();

private:
	friend static void DevThread(void* Ptr);
	void DevThread();

	BOOL m_bDev;
	HANDLE m_hDevThread;
	HANDLE m_hDevThreadEnd;

public:
	CServerDlg* m_pDlg;
	VinputControl(CServerDlg* pDlg);
	virtual ~VinputControl();
};

#endif
