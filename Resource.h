#pragma once
#include "Device.h"
#include<DirectXMath.h>
using namespace DirectX;
struct MatrixSize
{
	XMUINT4  Matrix;
};
class Resource:public Device
{
public:
	Resource();
	~Resource();
	HRESULT Initialization(UINT Height,UINT Width);
	VOID MatMul(FLOAT *LeftP, UINT LeftRow, UINT LeftColumn,
		FLOAT *RightP, UINT RightRow, UINT RightColumn, FLOAT *Dest)
	{
		this->UpdataMatrix(LeftP, LeftRow, LeftColumn,
			                  RightP, RightRow, RightColumn);
		this->Compute();

		this->GetResult(Dest);
	}
private:
	VOID GetResult(FLOAT *Dest);
	VOID UpdataMatrix(FLOAT *LeftP, UINT LeftRow, UINT LeftColumn,
		FLOAT *RightP, UINT RightRow, UINT RightColumn);
	VOID Compute()
	{
		m_Context->CSSetShaderResources(0, 1, &m_Matrix_Left);
		m_Context->CSSetShaderResources(1, 1, &m_Matrix_Right);
		m_Context->CSSetConstantBuffers(0, 1, &m_Describe);
		m_Context->CSSetUnorderedAccessViews(0, 1, &m_Out,nullptr);
		m_Context->CSSetShader(m_Shader_MatMul, 0 ,0);
		m_Context->Dispatch(Width, Height,1);
	}
private:
	UINT Height;
	UINT Width;
	//Max Support 16384*16384  Size
	MatrixSize                               m_MatrixSize;
	/*For calculate*/
	ID3D11Buffer*                           m_Describe;

	ID3D11ShaderResourceView*   m_Matrix_Left;

	ID3D11ShaderResourceView*   m_Matrix_Right;

	ID3D11Texture2D*                    m_TextureLeft;

	ID3D11Texture2D*                    m_TextureRight;

	ID3D11Texture2D*                    m_TextureRead;

	ID3D11UnorderedAccessView* m_Out;

	ID3D11ComputeShader*         m_Shader_MatMul;

   



	D3D11_MAPPED_SUBRESOURCE WriteContext;
	/*For Copy to Cpu*/
	D3D11_MAPPED_SUBRESOURCE                              m_Lock;
	ID3D11Texture2D*   m_Copy;
};

