#include "stdafx.h"
#include "cv.h"
#include "highgui.h"
#include "iostream"
#include "fstream"
using namespace std;
//����������
#ifndef CANSHU_H
#define CANSHU_H

class Canshu{
private:
	int N, // ѭ������
		ans,//���͸�ʴģ��ϵ��
		ans1,//���͸�ʴģ��ϵ��
		iteration,//ȥ���߸�ʴϵ��
		threshold,//��ֵ��ͼ����ֵ
		iteration1,//����ϵ��
		iteration2;//��ʴϵ��
public:
	int getN(){return N;};
	int getans(){return ans;};
	int getans1(){return ans1;};
	int getiteration(){return iteration;};
	int getiteration1(){return iteration1;};
	int getiteration2(){return iteration2;};
	int getthreshold(){return threshold;};
	Canshu(){
		ifstream in("in.txt");
		in >>  N >> ans >> ans1 >> iteration >> threshold >> iteration1 >> iteration2;
	}
};

#endif //CANSHU_H///:~