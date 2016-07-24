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

class CAboutDlg : public CDialog//关于对话框类
{
public:
	CAboutDlg();//构造函数
// Dialog Data
	enum { IDD = IDD_ABOUTBOX };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);// DDX/DDV support？
// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)//关于对话框构造函数
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

BOOL CmymfcDlg::OnInitDialog()//初始化对话框
{
	CDialog::OnInitDialog();//构造函数
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
	
	CvSize ImgSize;// 初始化 创建 TheImage
	ImgSize.height = IMAGE_HEIGHT;
	ImgSize.width = IMAGE_WIDTH;
	TheImage = cvCreateImage( ImgSize, IPL_DEPTH_8U, IMAGE_CHANNELS );
	
	m_run = 0;// 初始化视频、摄像头显示循环的执行和中断（m_run）
	
	int cam_count = CCameraDS::CameraCount();// 获取摄像头数目
	// 在组合框CamList中添加摄像头名称的字符串
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
			AfxMessageBox(_T("不能获取摄像头的名称"));
	}
	UpdateData( FALSE );
	// 使视频播放和暂停按钮失效
	GetDlgItem( IDC_RunCam )->EnableWindow( FALSE );
	GetDlgItem( IDC_StopCam )->EnableWindow( FALSE );
	
	CString mPath="logo.jpg";
	IplImage* ipl = cvLoadImage( mPath, 1 );	// 读取图片、缓存到一个局部变量 ipl 中
	if( !ipl )									// 判断是否成功读取图片
		return FALSE;
	if( TheImage )								// 对上一幅显示的图片数据清零
		cvZero( TheImage );

	ResizeImage( ipl );							// 对读入的图片进行缩放，再复制到 TheImage 中
	ShowImage( TheImage, IDC_ShowImg );			// 调用显示图片函数	
	cvReleaseImage( &ipl );						// 释放 ipl 占用的内存

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

void CmymfcDlg::OnPaint()//最小化所需的重绘功能
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
		CDialog::OnPaint();						// 重绘对话框
		CDialog::UpdateWindow();				// 更新windows窗口，重要
		ShowImage( TheImage, IDC_ShowImg );		// 重绘图片函数
	}
}
// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CmymfcDlg::OnQueryDragIcon()//拖动窗口功能
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CmymfcDlg::ShowImage( IplImage* img, UINT ID )//ID是Picture Control控件的ID号
{
	CDC* pDC = GetDlgItem( ID ) ->GetDC();		// 获得显示控件的 DC
	HDC hDC = pDC ->GetSafeHdc();				// 获取 HDC(设备句柄) 来进行绘图操作

	CRect rect;
	GetDlgItem(ID) ->GetClientRect( &rect );
	int rw = rect.right - rect.left;			// 求出图片控件的宽和高
	int rh = rect.bottom - rect.top;
	int iw = img->width;						// 读取图片的宽和高
	int ih = img->height;
	int tx = (int)(rw - iw)/2;					// 使图片的显示位置正好在控件的正中
	int ty = (int)(rh - ih)/2;
	SetRect( rect, tx, ty, tx+iw, ty+ih );

	CvvImage cimg;
	cimg.CopyOf( img );							// 复制图片
	cimg.DrawToHDC( hDC, &rect );				// 将图片绘制到显示控件的指定区域内

	ReleaseDC( pDC );
}

void CmymfcDlg::ResizeImage(IplImage* img)//重要重绘调整图片（帧）
{
	// 读取图片的宽和高
    int w = img->width;
	int h = img->height;
	int max = (w > h)? w: h;// 找出宽和高中的较大值者
	float scale = (float) ( (float) max / 301.0f );// 计算将图片缩放到TheImage区域所需的比例因子
	int nw = (int)( w/scale );// 缩放后图片的宽和高
	int nh = (int)( h/scale );
	int tlx = (nw > nh)? 0: (int)(300-nw)/2;//为将缩放后图片存入TheImage 的正中部，需计算图片在TheImage左上角的期望坐标值
	int tly = (nw > nh)? (int)(226-nh)/2: 0;
	cvSetImageROI( TheImage, cvRect( tlx, tly, nw, nh) );// 设置 TheImage 的 ROI 区域，用来存入图片 img
	cvResize( img, TheImage );// 对图片 img 进行缩放，并存入到 TheImage 中
	cvResetImageROI( TheImage );// 重置 TheImage 的 ROI 准备读入下一幅图片
}
/*===============================================================================================================
			                                    摄像头处理区
=================================================================================================================*/
void CmymfcDlg::OnCbnSelchangeCamlist()//摄像头列表按钮
{
	// TODO: Add your control notification handler code here
	mCamIndex = m_camList.GetCurSel();	
	GetDlgItem( IDC_RunCam )->EnableWindow( TRUE );
	return;
}

void CmymfcDlg::OnBnClickedStopcam()//关闭摄像头按钮
{
	// TODO: Add your control notification handler code here
	m_run = 0;
	return;
}

void CmymfcDlg::OnBnClickedRuncam()//开启摄像头按钮
{
	// TODO: Add your control notification handler code here
	CString camName;
	camName.Format("%s%d","Camera # ", mCamIndex );
	UpdateData( FALSE );
	
	CCameraDS camera;

	if( (! camera.OpenCamera(mCamIndex, false,320,240) ) ) //打开第一个摄像头用代码制定图像宽和高
	{
		AfxMessageBox(_T("Can not open camera.\n"));
		return;
	}
	
	GetDlgItem( IDC_StopCam )->EnableWindow( TRUE );// 使关闭摄像头按钮生效
	
	IplImage* cFrame;// 创建Ipl图像指针读取摄像头的帧画面
	
	cvNamedWindow("camera");// 创建一个视频播放窗口
	
	HWND hWnd = (HWND) cvGetWindowHandle("camera");		// 获取窗口句柄
	HWND hParent = ::GetParent(hWnd);// 获取该窗口的父窗口句柄	
	HWND hwnd1=::FindWindow("CmymfcDlg","mymfc");// 获取mymfc的GUI窗口句柄	
	::SetParent(hWnd, hwnd1);// 将GUI窗口设置为视频播放窗口的父窗口，使视频在 GUI 的指定区域播放
	// 隐藏所创建的视频播放窗口：
	::ShowWindow(hParent, SW_HIDE);
	::ShowWindow(hWnd, SW_HIDE);
	
	double xe=0,ye=0,x=0,y=0,xe1=0,ye1=0,x1=0,y1=0;//坐标映射模块变量：
	int w=0,h=0,flag=0;
	PupilLocating Pupil;
	
	if( TheImage ) cvZero( TheImage );// 对图像数据清零

	m_run = 1;	
	while( m_run )//获取一帧
	{
		cFrame = camera.QueryFrame();//cFrame不能被直接处理
		IplImage* newframe = cvCloneImage(cFrame);//克隆到newframe,处理newframe
		ResizeImage( newframe );//调整大小
		
		Pupil.detectEyes(TheImage,p);  //确定瞳孔位置并显示
		//
		for(int ii = 0; ii < 2; ii++)//在瞳孔位置画出十字
		{
			cvLine(TheImage,cvPoint(p[ii].x-5,p[ii].y),cvPoint(p[ii].x+5,p[ii].y),CV_RGB(255, 0, 0));
			cvLine(TheImage,cvPoint(p[ii].x,p[ii].y-5),cvPoint(p[ii].x,p[ii].y+5),CV_RGB(255, 0, 0));
		}
		ShowImage( TheImage, IDC_ShowImg );//显示到picture控件中
		//从上一部分获得xe，ye计算对应点
		xe=p[0].x; ye=p[0].y;
		xe1=p[1].x; ye1=p[1].y;
		x=X.B(xe,ye);
		y=Y.B(xe,ye);
		//以下被注释则为单眼标定
// 		x1=X1.B(xe1,ye1);
// 		y1=Y1.B(xe1,ye1);
// 		w=(int)((x+x1)/2.0);
// 		h=(int)((y+y1)/2.0);
		
		flag=SetCursorPos((int)x,(int)y);//参数为单眼标定所得坐标。

		if (cvWaitKey(20) == 27)
			break;
		cvReleaseImage( &newframe );
	}

	if( TheImage )	// 对图像数据清零	
	{
		cvZero( TheImage );
		ShowImage( TheImage, IDC_ShowImg );
	}
	camera.CloseCamera();
	cvDestroyWindow("camera");

	return;
}
/*=================================================================================================================
			                                      坐标映射区
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
	while(m_run )//获取一帧
	{ 
		--m_run;
		cFrame = camera.QueryFrame();
		IplImage* newframe = cvCloneImage(cFrame);

		ResizeImage( newframe);//调整大小
		Pupil.detectEyes(TheImage,p);
		cvLine(TheImage,cvPoint(p[0].x-5,p[0].y),cvPoint(p[0].x+5,p[0].y),CV_RGB(255, 0, 0));
		cvLine(TheImage,cvPoint(p[0].x,p[0].y-5),cvPoint(p[0].x,p[0].y+5),CV_RGB(255, 0, 0));
		ShowImage( TheImage, IDC_ShowImg );//显示到picture控件中
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

	if( TheImage )	// 对图像数据清零
	{
		cvZero( TheImage );
		ShowImage( TheImage, IDC_ShowImg );
	}

	camera.CloseCamera();
	cvDestroyWindow("camera");

	double a[6]={0},a1[6]={0};
	double b[6]={0},b1[6]={0};
	S1.Map(Xs,Xe,Ye,a);
	S2.Map(Ys,Xe,Ye,b);//左眼
	S3.Map(Xs,Xe1,Ye1,a1);
	S4.Map(Ys,Xe1,Ye1,b1);//右眼

	X.A(a);
	Y.A(b);
	X1.A(a1);
	Y1.A(b1);
}
/*=================================================================================================================
			                                      退出GUI窗口
===================================================================================================================*/
void CmymfcDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	m_run = 0;
	cvDestroyAllWindows();
	OnOK();
}