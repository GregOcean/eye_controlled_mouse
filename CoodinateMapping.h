#include "cv.h"
#include "highgui.h"
#include "cxcore.h"
//ӳ����
#ifndef YINGSHE_H
#define YINGSHE_H

class Mapping{
private:
	double a1,a2,a3,a4,a5,a6;
public:
	void A(double b[]);
	double B(double xe,double ye);
};

#endif //YINGSHE_H///:~

//�����
#ifndef NIHE_H
#define NIHE_H

class Fitting//��С���˷� least squares techniques
{
private:
	double **A,**AT,X[6],b[10],C[36],D[36];
	int N;
public:
	Fitting();//��ʼ��
	void Map(double Y[],double Xe[],double Ye[],double a[]);
};

#endif //NIHE_H///:~

//��Ϸ���Xs=a0+a1Xe+a2Xe^2+a3Xe*Ye+a4Ye+a5Ye*Ye,