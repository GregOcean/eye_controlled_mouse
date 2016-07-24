// mymfcDlg.cpp : implementation file
//
#include "stdafx.h"
#include "EyeCan.h"
#include "EyeCanDlg.h"
#include "CoodinateMapping.h"
#include "PupilLocating.h"
#include "canshu.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog//���ڶԻ�����
{
public:
	CAboutDlg();//���캯��
// Dialog Data
	enum { IDD = IDD_ABOUTBOX };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);// DDX/DDV support��
// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)//���ڶԻ����캯��
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)//?????
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)

END_MESSAGE_MAP()

// CEyeCanDlg dialog

CmymfcDlg::CmymfcDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CmymfcDlg::IDD, pParent)
	, TheImage(NULL)
	, vPath(_T(""))
	, pCapture(NULL)
	, m_run(0)
	, mCamIndex(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CmymfcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CamList, m_camList);
}

BEGIN_MESSAGE_MAP(CmymfcDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	ON_CBN_SELCHANGE(IDC_CamList, &CmymfcDlg::OnCbnSelchangeCamlist)

	ON_BN_CLICKED(IDOK, &CmymfcDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_StopCam, &CmymfcDlg::OnBnClickedStopcam)
	ON_BN_CLICKED(IDC_RunCam, &CmymfcDlg::OnBnClickedRuncam)
	ON_BN_CLICKED(ID_Initialize, &CmymfcDlg::OnBnClickedInitialize)

END_MESSAGE_MAP()

// CmymfcDlg message handlers

BOOL CmymfcDlg::OnInitDialog()//��ʼ���Ի���
{
	CDialog::OnInitDialog();//���캯��
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	// TODO: Add extra initialization here
	
	CvSize ImgSize;// ��ʼ�� ���� TheImage
	ImgSize.height = IMAGE_HEIGHT;
	ImgSize.width = IMAGE_WIDTH;
	TheImage = cvCreateImage( ImgSize, IPL_DEPTH_8U, IMAGE_CHANNELS );
	
	m_run = 0;// ��ʼ����Ƶ������ͷ��ʾѭ����ִ�к��жϣ�m_run��
	
	int cam_count = CCameraDS::CameraCount();// ��ȡ����ͷ��Ŀ
	// ����Ͽ�CamList���������ͷ���Ƶ��ַ���
	for(int i=0; i < cam_count; i++)
	{
		char camera_name[1024];  
		int retval = CCameraDS::CameraName(i, camera_name, sizeof(camera_name) );
		
		char istr[25];
		sprintf_s(istr, " # %d", i);
		//strcat_s( camera_name, istr );
		CString camstr;
		camstr.Format("%s%s",camera_name, istr );		
		if(retval >0)
			m_camList.AddString( camstr );
		else
			AfxMessageBox(_T("���ܻ�ȡ����ͷ������"));
	}
	UpdateData( FALSE );
	// ʹ��Ƶ���ź���ͣ��ťʧЧ
	GetDlgItem( IDC_RunCam )->EnableWindow( FALSE );
	GetDlgItem( IDC_StopCam )->EnableWindow( FALSE );
	
	CString mPath="logo.jpg";
	IplImage* ipl = cvLoadImage( mPath, 1 );	// ��ȡͼƬ�����浽һ���ֲ����� ipl ��
	if( !ipl )									// �ж��Ƿ�ɹ���ȡͼƬ
		return FALSE;
	if( TheImage )								// ����һ����ʾ��ͼƬ��������
		cvZero( TheImage );

	ResizeImage( ipl );							// �Զ����ͼƬ�������ţ��ٸ��Ƶ� TheImage ��
	ShowImage( TheImage, IDC_ShowImg );			// ������ʾͼƬ����	
	cvReleaseImage( &ipl );						// �ͷ� ipl ռ�õ��ڴ�

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CmymfcDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CmymfcDlg::OnPaint()//��С��������ػ湦��
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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
		CDialog::OnPaint();						// �ػ�Ի���
		CDialog::UpdateWindow();				// ����windows���ڣ���Ҫ
		ShowImage( TheImage, IDC_ShowImg );		// �ػ�ͼƬ����
	}
}
// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CmymfcDlg::OnQueryDragIcon()//�϶����ڹ���
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CmymfcDlg::ShowImage( IplImage* img, UINT ID )//ID��Picture Control�ؼ���ID��
{
	CDC* pDC = GetDlgItem( ID ) ->GetDC();		// �����ʾ�ؼ��� DC
	HDC hDC = pDC ->GetSafeHdc();				// ��ȡ HDC(�豸���) �����л�ͼ����

	CRect rect;
	GetDlgItem(ID) ->GetClientRect( &rect );
	int rw = rect.right - rect.left;			// ���ͼƬ�ؼ��Ŀ�͸�
	int rh = rect.bottom - rect.top;
	int iw = img->width;						// ��ȡͼƬ�Ŀ�͸�
	int ih = img->height;
	int tx = (int)(rw - iw)/2;					// ʹͼƬ����ʾλ�������ڿؼ�������
	int ty = (int)(rh - ih)/2;
	SetRect( rect, tx, ty, tx+iw, ty+ih );

	CvvImage cimg;
	cimg.CopyOf( img );							// ����ͼƬ
	cimg.DrawToHDC( hDC, &rect );				// ��ͼƬ���Ƶ���ʾ�ؼ���ָ��������

	ReleaseDC( pDC );
}

void CmymfcDlg::ResizeImage(IplImage* img)//��Ҫ�ػ����ͼƬ��֡��
{
	// ��ȡͼƬ�Ŀ�͸�
    int w = img->width;
	int h = img->height;
	int max = (w > h)? w: h;// �ҳ���͸��еĽϴ�ֵ��
	float scale = (float) ( (float) max / 301.0f );// ���㽫ͼƬ���ŵ�TheImage��������ı�������
	int nw = (int)( w/scale );// ���ź�ͼƬ�Ŀ�͸�
	int nh = (int)( h/scale );
	int tlx = (nw > nh)? 0: (int)(300-nw)/2;//Ϊ�����ź�ͼƬ����TheImage �����в��������ͼƬ��TheImage���Ͻǵ���������ֵ
	int tly = (nw > nh)? (int)(226-nh)/2: 0;
	cvSetImageROI( TheImage, cvRect( tlx, tly, nw, nh) );// ���� TheImage �� ROI ������������ͼƬ img
	cvResize( img, TheImage );// ��ͼƬ img �������ţ������뵽 TheImage ��
	cvResetImageROI( TheImage );// ���� TheImage �� ROI ׼��������һ��ͼƬ
}
/*===============================================================================================================
			                                    ����ͷ������
=================================================================================================================*/
void CmymfcDlg::OnCbnSelchangeCamlist()//����ͷ�б�ť
{
	// TODO: Add your control notification handler code here
	mCamIndex = m_camList.GetCurSel();	
	GetDlgItem( IDC_RunCam )->EnableWindow( TRUE );
	return;
}

void CmymfcDlg::OnBnClickedStopcam()//�ر�����ͷ��ť
{
	// TODO: Add your control notification handler code here
	m_run = 0;
	return;
}

void CmymfcDlg::OnBnClickedRuncam()//��������ͷ��ť
{
	// TODO: Add your control notification handler code here
	CString camName;
	camName.Format("%s%d","Camera # ", mCamIndex );
	UpdateData( FALSE );
	
	CCameraDS camera;

	if( (! camera.OpenCamera(mCamIndex, false,320,240) ) ) //�򿪵�һ������ͷ�ô����ƶ�ͼ���͸�
	{
		AfxMessageBox(_T("Can not open camera.\n"));
		return;
	}
	
	GetDlgItem( IDC_StopCam )->EnableWindow( TRUE );// ʹ�ر�����ͷ��ť��Ч
	
	IplImage* cFrame;// ����Iplͼ��ָ���ȡ����ͷ��֡����
	
	cvNamedWindow("camera");// ����һ����Ƶ���Ŵ���
	
	HWND hWnd = (HWND) cvGetWindowHandle("camera");		// ��ȡ���ھ��
	HWND hParent = ::GetParent(hWnd);// ��ȡ�ô��ڵĸ����ھ��	
	HWND hwnd1=::FindWindow("CmymfcDlg","mymfc");// ��ȡmymfc��GUI���ھ��	
	::SetParent(hWnd, hwnd1);// ��GUI��������Ϊ��Ƶ���Ŵ��ڵĸ����ڣ�ʹ��Ƶ�� GUI ��ָ�����򲥷�
	// ��������������Ƶ���Ŵ��ڣ�
	::ShowWindow(hParent, SW_HIDE);
	::ShowWindow(hWnd, SW_HIDE);
	
	double xe=0,ye=0,x=0,y=0,xe1=0,ye1=0,x1=0,y1=0;//����ӳ��ģ�������
	int w=0,h=0,flag=0;
	PupilLocating Pupil;
	
	if( TheImage ) cvZero( TheImage );// ��ͼ����������

	m_run = 1;	
	while( m_run )//��ȡһ֡
	{
		cFrame = camera.QueryFrame();//cFrame���ܱ�ֱ�Ӵ���
		IplImage* newframe = cvCloneImage(cFrame);//��¡��newframe,����newframe
		ResizeImage( newframe );//������С
		
		Pupil.detectEyes(TheImage,p);  //ȷ��ͫ��λ�ò���ʾ
		//
		for(int ii = 0; ii < 2; ii++)//��ͫ��λ�û���ʮ��
		{
			cvLine(TheImage,cvPoint(p[ii].x-5,p[ii].y),cvPoint(p[ii].x+5,p[ii].y),CV_RGB(255, 0, 0));
			cvLine(TheImage,cvPoint(p[ii].x,p[ii].y-5),cvPoint(p[ii].x,p[ii].y+5),CV_RGB(255, 0, 0));
		}
		ShowImage( TheImage, IDC_ShowImg );//��ʾ��picture�ؼ���
		//����һ���ֻ��xe��ye�����Ӧ��
		xe=p[0].x; ye=p[0].y;
		xe1=p[1].x; ye1=p[1].y;
		x=X.B(xe,ye);
		y=Y.B(xe,ye);
		//���±�ע����Ϊ���۱궨
// 		x1=X1.B(xe1,ye1);
// 		y1=Y1.B(xe1,ye1);
// 		w=(int)((x+x1)/2.0);
// 		h=(int)((y+y1)/2.0);
		
		flag=SetCursorPos((int)x,(int)y);//����Ϊ���۱궨�������ꡣ

		if (cvWaitKey(20) == 27)
			break;
		cvReleaseImage( &newframe );
	}

	if( TheImage )	// ��ͼ����������	
	{
		cvZero( TheImage );
		ShowImage( TheImage, IDC_ShowImg );
	}
	camera.CloseCamera();
	cvDestroyWindow("camera");

	return;
}
/*=================================================================================================================
			                                      ����ӳ����
===================================================================================================================*/
void CmymfcDlg::OnBnClickedInitialize()
{
	CString camName;
	camName.Format("%s%d","Camera # ", mCamIndex );
	UpdateData( FALSE );

	CCameraDS camera;

	if( (! camera.OpenCamera(mCamIndex, false,320,240) ) )
	{
		AfxMessageBox(_T("Can not open camera.\n"));
		return;
	}
	GetDlgItem( IDC_StopCam )->EnableWindow( TRUE );
	IplImage* cFrame;

	cvNamedWindow("camera");
	cvResizeWindow("camera",0,0);

	HWND hWnd = (HWND) cvGetWindowHandle("camera");
	HWND hParent = ::GetParent(hWnd);
	HWND hwnd1=::FindWindow("CmymfcDlg","mymfc");
	
	::SetParent(hWnd, hwnd1);
	::ShowWindow(hParent, SW_HIDE);
	::ShowWindow(hWnd, SW_HIDE);
	
	if( TheImage ) cvZero( TheImage );
		
	int i=0;
	Canshu canshu;
	int N;
	N = canshu.getN();
	double *Xs,*Ys,*Xe,*Ye,*Xe1,*Ye1;
	Xs = new double[N+1];
	Ys = new double[N+1];
	Xe = new double[N+1];
	Ye = new double[N+1];
	Xe1 = new double[N+1];
	Ye1 = new double[N+1];
	Fitting S1,S2,S3,S4;
	POINT curpos;
	PupilLocating Pupil;
	m_run = N;	
	while(m_run )//��ȡһ֡
	{ 
		--m_run;
		cFrame = camera.QueryFrame();
		IplImage* newframe = cvCloneImage(cFrame);

		ResizeImage( newframe);//������С
		Pupil.detectEyes(TheImage,p);
		cvLine(TheImage,cvPoint(p[0].x-5,p[0].y),cvPoint(p[0].x+5,p[0].y),CV_RGB(255, 0, 0));
		cvLine(TheImage,cvPoint(p[0].x,p[0].y-5),cvPoint(p[0].x,p[0].y+5),CV_RGB(255, 0, 0));
		ShowImage( TheImage, IDC_ShowImg );//��ʾ��picture�ؼ���
		if (cvWaitKey(1000) == 27)			
			break;
		if(i==0)
		{
			Xe[0]=0; Ye[0]=0;
			Xe1[0]=0; Ye1[0]=0;
		}
		else
		{
			Xe[i]=p[0].x-Xe[0]; Ye[i]=p[0].y-Ye[0];
			Xe1[i]=p[1].x-Xe1[0]; Ye1[i]=p[1].y-Ye1[0];
		}
		
		GetCursorPos(&curpos);
		Xs[i]=curpos.x;
		Ys[i]=curpos.y;
		++i;

		cvReleaseImage( &newframe );
	}

	if( TheImage )	// ��ͼ����������
	{
		cvZero( TheImage );
		ShowImage( TheImage, IDC_ShowImg );
	}

	camera.CloseCamera();
	cvDestroyWindow("camera");

	double a[6]={0},a1[6]={0};
	double b[6]={0},b1[6]={0};
	S1.Map(Xs,Xe,Ye,a);
	S2.Map(Ys,Xe,Ye,b);//����
	S3.Map(Xs,Xe1,Ye1,a1);
	S4.Map(Ys,Xe1,Ye1,b1);//����

	X.A(a);
	Y.A(b);
	X1.A(a1);
	Y1.A(b1);
}
/*=================================================================================================================
			                                      �˳�GUI����
===================================================================================================================*/
void CmymfcDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	m_run = 0;
	cvDestroyAllWindows();
	OnOK();
}