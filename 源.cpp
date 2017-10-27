#include "Resource.h"
#include <iostream>
#include "GPUMath.h"
using namespace std;
int main()
{
	//Max Mat 256*256 
	GPUMath Math(256,256);
	GPUVariable LeftMat;
	GPUVariable RightMat;
	GPUVariable Result;
	LeftMat.GPUarange(100, Shape(25, 4));
	RightMat.GPUarange(20, Shape(4, 5));
	Math.MatMul(LeftMat, RightMat, Result);
	GPUVariable A;
	A.GPUarange(100, Shape(5, 20));
	Math.MatMul(LeftMat, Result, RightMat);
	for (UINT i = 0; i < RightMat.m_Shape.Row*RightMat.m_Shape.Vertical; ++i)
	{
		cout << RightMat.m_pData[i] << endl;
	}
	system("pause");
    return 0;
}