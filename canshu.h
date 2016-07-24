#include "stdafx.h"
#include "cv.h"
#include "highgui.h"
#include "iostream"
#include "fstream"
using namespace std;
//参数设置类
#ifndef CANSHU_H
#define CANSHU_H

class Canshu{
private:
	int N, // 循环次数
		ans,//膨胀腐蚀模板系数
		ans1,//膨胀腐蚀模板系数
		iteration,//去亮斑腐蚀系数
		threshold,//二值化图像阈值
		iteration1,//膨胀系数
		iteration2;//腐蚀系数
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