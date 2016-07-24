// mymfcDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "CoodinateMapping.h"

// CmymfcDlg dialog
class CmymfcDlg : public CDialog
{
// Construction
public:
	CmymfcDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MYMFC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	// 用于初始化bmp的显示缓存
	HICON m_hIcon;
	BITMAPINFO* bmi;
	BITMAPINFOHEADER* bmih;
	RGBQUAD* palette;
	unsigned int buffer[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256];

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void ShowImage( IplImage* img, UINT ID );	
	IplImage* TheImage;
	Mapping X,Y,X1,Y1;
	CvPoint p[3];
	void ResizeImage(IplImage* img);
	CString vPath;
	CvCapture* pCapture;
	CComboBox m_camList;
	afx_msg void OnCbnSelchangeCamlist();
	int m_run;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedStopcam();
	afx_msg void OnBnClickedRuncam();
	int mCamIndex;
	afx_msg void OnBnClickedInitialize();
};
