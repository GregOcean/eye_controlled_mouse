#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>

#include "cv.h"
#include "highgui.h"
#include "PupilLocating.h"
#include "canshu.h"
// void PupilLocating::count(IplImage*gray,IplImage *img,CvPoint p1,CvPoint p2,CvPoint& p3)
// {
// 	int i,j,x=0,y=0,sum,min;
// 	unsigned char * ptr;
// 	min=999999999;
// 	for(i=p1.y;i<=p2.y;i++)
// 	{ 
// 		ptr=(unsigned char *)(gray->imageData+i*(gray->widthStep));
// 		sum=0;
// 		for(j=p1.x;j<=p2.x;j++)
// 		{ 
// 			sum=sum+ptr[j];
// 		}
// 		if(sum<min)
// 		{
// 			min=sum;
// 			y=i;
// 		}
// 	}
// 	min=999999999;
// 	ptr=(unsigned char *)(gray->imageData);
// 	for(i=p1.x;i<=p2.x;i++)
// 	{		
// 		sum=0;
// 		for(j=p1.y;j<=p2.y;j++)
// 			sum=sum+ptr[j*(gray->widthStep)+i];
// 		if(sum<min)
// 		{
// 			min=sum;
// 			x=i;
// 		}
// 	}
// 	p3.x=x;
// 	p3.y=y;
// }

// void PupilLocating::count(IplImage*&gray,CvPoint p1,CvPoint p2,CvPoint& p3)
// {
// 	cvSetImageROI(gray,cvRect(p1.x,p1.y,p2.x-p1.x,p2.y-p1.y));
// 	IplImage *edge=cvCreateImage(cvSize(p2.x-p1.x,p2.y-p1.y),IPL_DEPTH_8U, 1);
// 	//cvSmooth(gray,edge,CV_GAUSSIAN,3,3,0,0);    //高斯滤波
// 	cvCanny(gray,edge,50,25,3);				  //Canny边缘检测
// 	printf("%d %d\n",edge->width,edge->height);
// 	cvResetImageROI(gray);
// 	CvMemStorage*storage_circle=cvCreateMemStorage(0);
// 	//cvHoughCircles函数需要估计每一个像素梯度的方向
// 	//因此会在内部自动调用cvSobel,而二值边缘图像的处理是比较难的
// 	CvSeq* circles=cvHoughCircles(edge,storage_circle,CV_HOUGH_GRADIENT,2.0,edge->width/1,100,50);
// 	for(int i=0;i<circles->total;i++)
// 	{
// 		float* p = ( float* )cvGetSeqElem( circles, i );
// 		p3.x=cvRound(p[0]+p1.x);					//坐标变换。
// 		p3.y=cvRound(p[1]+p1.y);
// 	}
// 	cvReleaseImage(&edge);
// 	cvReleaseMemStorage(&storage_circle);
// }
void PupilLocating::Coordinate(IplImage* dst,int& width,int& height,int& row,int &col)
{
	int Sum_row=0,Sum_col=0,sumnum=0;
	unsigned char * ptr=(unsigned char *)(dst->imageData);
	for(int i=0;i<height;i++)
	{ 
		for(int j=0;j<width;j++)
		{ 
			if (ptr[i*(dst->widthStep)+j]==255)
			{
				Sum_row+=i;
				Sum_col+=j;
				sumnum++;
			}
		}
	}
	if (sumnum==0)
	{
		return;
	}
	row=Sum_row/sumnum;
	col=Sum_col/sumnum;
}
void PupilLocating::count(IplImage*&gray,CvPoint p1,CvPoint p2,CvPoint& p3)
{
	Canshu canshu;
	IplImage* src =0;
	IplImage* dst = 0;
	IplConvKernel* element = 0;

	cvSetImageROI(gray,cvRect(p1.x,p1.y,p2.x-p1.x,p2.y-p1.y));
	IplImage *edge=cvCreateImage(cvSize(p2.x-p1.x,p2.y-p1.y),IPL_DEPTH_8U, 1);
	cvCopy(gray,edge);
	src=cvCloneImage(edge);
	dst=cvCloneImage(src); 
	int element_shape = CV_SHAPE_RECT;
	int ans = canshu.getans();
	element = cvCreateStructuringElementEx(2*ans, 2*ans, ans, ans,element_shape,0);
	//cvDilate(src,dst,element,1);
	cvErode(dst,dst,element,1);

	//二值化图像
	IplImage *Im_bin=cvCreateImage(cvSize(edge->width,edge->height),8,1);
	//int threshold=otsuThreshold(gray);
	cvThreshold(dst,Im_bin,/*threshold*/canshu.getthreshold(),255,CV_THRESH_BINARY_INV);

	//膨胀腐蚀
	IplImage* src1 =0;
	IplImage* dst1 = 0;
	IplConvKernel* element1 = 0;

	src1=cvCloneImage(Im_bin);
	dst1 = cvCloneImage(src1); 

	int element_shape1 = CV_SHAPE_RECT;
	int ans1=canshu.getans1();
	element = cvCreateStructuringElementEx( 2*ans1, 2*ans1,ans1 ,ans1 , element_shape1, 0 );

	cvDilate(src1,dst1,element1,canshu.getiteration1());//膨胀
	cvErode(dst1,dst1,element1,canshu.getiteration2());//腐蚀
	int row,col;
	Coordinate(dst1,dst1->width,dst1->height,row,col);
	//
	p3.x=col+p1.x;
	p3.y=row+p1.y;

	cvReleaseImage(&src);
	cvReleaseImage(&src1);
	cvReleaseImage(&dst);
	cvReleaseImage(&dst1);
}

void PupilLocating::detectEyes(IplImage *img,CvPoint  p[])//人眼瞳孔检测函数
{  
    CvMemStorage* storage = 0;  /*allocate storage*/  
    storage = cvCreateMemStorage(0) ;  
    
    CvHaarClassifierCascade* cascade_f = 0;  /*load face cascade*/  
    const char* cascade_name = "haarcascade_frontalface_alt2.xml";  
    cascade_f = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 );  
    /* detect faces */  
	double scale = 1.3;
	
	IplImage* gray = cvCreateImage( cvSize(img->width,img->height), 8, 1 );//创建头并分配数据
	IplImage* small_img = cvCreateImage( cvSize( cvRound (img->width/scale),cvRound (img->height/scale)), 8, 1 );
	cvCvtColor( img, gray, CV_BGR2GRAY );//颜色空间转换函数，可以实现rgb颜色向HSV,HSI等颜色空间的转换，也可转换为灰度图。
	cvResize( gray, small_img, CV_INTER_LINEAR );
	cvEqualizeHist( small_img, small_img );	//增加亮度和对比度

    CvSeq *faces = cvHaarDetectObjects(  
        small_img,      /* the source image */  
        cascade_f,      /* the face classifier */  
        storage,        /* memory buffer, created with cvMemStorage */  
        1.5, 3, 0,      /* special parameters, tune for your app */  
        cvSize(30, 30)  /* minimum detection scale */  
    ); 
    if (faces->total != 1)
	{
		cvResetImageROI(img);  /* reset region of interest */ 
		cvReleaseMemStorage(&storage);

		cvReleaseImage(&gray);
		cvReleaseImage(&small_img);

		cvRelease((void **)&cascade_f);
		return;   /* return if not found */ 
	}
    CvRect *face = (CvRect*)cvGetSeqElem(faces, 0);  /* get the first detected face */ 
    cvRectangle(  
        img,  
        cvPoint(face->x*scale, face->y*scale),  
        cvPoint(  
            (face->x + face->width)*scale,  
            (face->y + face->height)*scale  
        ),  
        CV_RGB(255, 0, 0),  
        1, 8, 0  
    );//在img中框出脸的部分 

    cvSetImageROI(  
       img,                /* the source image */  
       cvRect(  
       face->x*scale,      /* x = start from leftmost */  
       face->y*scale,	   /* y = a few pixels from the top */  
       face->width *scale, /* width = same width with the face */  
       face->height*scale  /* height = 1/3 of face height */  
			)  
   );//img图片只截取脸的部分
    CvHaarClassifierCascade* cascade_e = 0;  
    const char* cascade_name2 = "haarcascade_eye.xml";  
    cascade_e = (CvHaarClassifierCascade*)cvLoad( cascade_name2, 0, 0, 0 );  
    assert(cascade_e != NULL);  
	CvMemStorage* storage1 = 0;  
	storage1 = cvCreateMemStorage(0) ; 
    CvSeq *eyes = cvHaarDetectObjects(  
        img,            /* the source image, with the 
                           estimated location defined */  
        cascade_e,      /* the eye classifier */  
        storage1,       /* memory buffer */  
        1.15, 2, 0,     /* tune for your app */  
        cvSize(25, 15)  /* minimum detection scale */  
    );  
	if(eyes->total != 2)
	{
		cvResetImageROI(img);  /* reset region of interest */ 
		cvReleaseMemStorage(&storage);
		cvReleaseMemStorage(&storage1);

		cvReleaseImage(&gray);
		cvReleaseImage(&small_img);

		cvRelease((void **)&cascade_f);
		cvRelease((void **)&cascade_e);

		return;
	}
	for(int i = 0; i < (eyes ? eyes->total : 0); i++ )/* draw a rectangle for each detected eye */  
	{  
		CvRect *eye = (CvRect*)cvGetSeqElem(eyes, i);  /* get one eye */ 
		cvResetImageROI(img);
		CvPoint p1,p2;
		p1.x=eye->x+face->x*scale+eye->width*0.3;
		p1.y=eye->y+eye->height*0.3+face->y*scale;
		p2.x=p1.x+eye->width*0.5;
		p2.y=p1.y+eye->height*0.3;
		cvRectangle(  
			img,  
			cvPoint(p1.x,p1.y ),  
			cvPoint(p2.x,p2.y),  
			CV_RGB(255, 0, 0),  
			1, 8, 0);
		PupilLocating::count(gray/*,img*/,p1,p2,p[i]);
	} 
    cvResetImageROI(img);  /* reset region of interest */ 
	cvReleaseMemStorage(&storage);
	cvReleaseMemStorage(&storage1);

	cvReleaseImage(&gray);
	cvReleaseImage(&small_img);

	cvRelease((void **)&cascade_f);
	cvRelease((void **)&cascade_e);
}  