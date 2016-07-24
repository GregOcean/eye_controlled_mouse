#include "stdafx.h"
#include"CoodinateMapping.h"
#include "canshu.h"
void Mapping::A(double b[])
{
	a1=b[0]; a2=b[1]; a3=b[2]; a4=b[3]; a5=b[4]; a6=b[5];
}

double Mapping::B(double x,double y)
{
	return a1+a2*x+a3*x*x+a4*x*y+a5*y+a6*y*y;
}

Fitting::Fitting()
{
	Canshu canshu;
	int i;
	N = canshu.getN();
	A = new double *[N+1];
	for(i = 0;i < N;i++) A[i] = new double [7];
	for(i = 0;i<N;i++)
	{
		A[i][0]=1;
	}
	AT = new double *[7];
	for(i = 0;i < 7;i++) AT[i] = new double [N+1];
}

void Fitting::Map(double Y[],double Xe[],double Ye[],double a[])//最小二乘法 least squares techniques(映射)
{
	int i,j,k;
	double max=0.0;
	double **E = new double *[7];
	for(i = 0;i < 7;i++) E[i] = new double [N+1];
	for(i = 0;i < 7;i++)
		for(j = 0;j < N;j++)
			E[i][j] = 0;
	for(i=0;i<N;i++)
	{
		A[i][1]=Xe[i];
		A[i][2]=Xe[i]*Xe[i];
		A[i][3]=Xe[i]*Ye[i];
		A[i][4]=Ye[i];
		A[i][5]=Ye[i]*Ye[i];
	}//初始化A
	for(i=0;i<10;i++)
	{
		for(j=0;j<6;j++)
		{
			AT[j][i]=A[i][j];
		}
	}//求A转置
	for(i=0;i<6;i++)
	{
		for(j=0;j<6;j++)
		{
			max=0;
			for(k=0;k<N;k++)
			{
				max=max+AT[i][k]*A[k][j];
			}
			C[i*6+j]=max;
		}
	}//求AT*A，存入C
	CvMat Ma=cvMat(6,6,CV_64FC1,C);
	CvMat Mb=cvMat(6,6,CV_64FC1,D);
	cvInvert(&Ma,&Mb,CV_LU);
	if(1)
	{
		for(i=0;i<6;i++)
		{
			for(j=0;j<N;j++)
			{
				E[i][j]=0;
				for(k=0;k<6;k++)
				{
					E[i][j]=E[i][j]+D[i*6+k]*AT[k][j];
				}
			}
		}//求A*AT的逆矩阵*AT存入E中
		for(j=0;j<6;j++)
		{
			a[j]=0;
			for(k=0;k<N;k++)
			{
				a[j]=a[j]+E[j][k]*Y[k];
			}
		}
	}
}