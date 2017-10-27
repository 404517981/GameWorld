#pragma once
#include"Resource.h"
struct Shape
{
	Shape(UINT _Row, UINT _Vertical):Row(_Row), Vertical(_Vertical)
	{}
	Shape()
	{}
	UINT     Row;
	UINT     Vertical;
};
struct GPUVariable
{
	GPUVariable(GPUVariable& Var)
	{   
		m_pData = new FLOAT[Var.m_Shape.Row*Var.m_Shape.Vertical];
		memcpy(m_pData, Var.m_pData, Var.m_Shape.Row*Var.m_Shape.Vertical);
		m_Shape.Row = Var.m_Shape.Row;
		m_Shape.Vertical = Var.m_Shape.Vertical;
	}
	GPUVariable():m_pData(nullptr),
		                   m_Shape(0,0)
	{
	}
	VOID GPUarange(UINT _Size, Shape &_Shape)
	{
		if (m_pData)
			delete[]m_pData;
		assert(_Size == _Shape.Row*_Shape.Vertical);
		m_pData = new FLOAT[_Shape.Row*_Shape.Vertical];
		for (UINT i = 0; i < _Size; ++i)
		{
			m_pData[i] = static_cast<FLOAT>(i);
		}
		m_Shape = _Shape;
	}
	FLOAT*  m_pData;
	Shape    m_Shape;
};
class GPUMath
{
public:
	GPUMath(UINT MaxRow,UINT MaxVertical)
	{
		GPU.CreateDeviceContext();
		GPU.Initialization(MaxRow, MaxVertical);
	}
	~GPUMath()
	{

	}
	VOID MatMul(GPUVariable &Left, GPUVariable&Right, GPUVariable& Dest)
	{
		if (Dest.m_pData)
			delete[] Dest.m_pData;
		Dest.m_pData = new FLOAT[Left.m_Shape.Row*Right.m_Shape.Vertical];
		Dest.m_Shape.Row = Left.m_Shape.Row;
		Dest.m_Shape.Vertical = Right.m_Shape.Vertical;
		GPU.MatMul(Left.m_pData, Left.m_Shape.Row, Left.m_Shape.Vertical, Right.m_pData, Right.m_Shape.Row, Right.m_Shape.Vertical, Dest.m_pData);

	}
private:Resource  GPU;
};

