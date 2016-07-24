
#include "cv.h"
#include "highgui.h"
#include "cxcore.h"

#ifndef NIHE_H
#define NIHE_H
//拟合方程Xs=a0+a1Xe+a2Xe^2+a3Xe*Ye+a4Ye+a5Ye*Ye,
//拟合类
class ZXEC
{
	double A[10][6],AT[6][10],X[6],b[10],C[36],D[36];
public:
	ZXEC();//初始化
	void nihe(double Y[],double Xe[],double Ye[],double a[]);
};


#endif //NIHE_H///:~