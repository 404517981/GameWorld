#include "Resource.h"
#include <d3dcompiler.h>
#include<fstream>
using namespace std;
#define Release(X)      if(X){\
                                           X->Release(); \
                                           X = nullptr;\
                                          }
Resource::Resource()
{
}


Resource::~Resource()
{
	/*For calculate*/
	Release(m_Describe) ;

	Release(m_Matrix_Left);

	Release(m_Matrix_Right);

	Release(m_TextureLeft);

	Release(m_TextureRight);

	Release(m_TextureRead);

	Release(m_Out);

	Release(m_Copy);

	Release(m_Shader_MatMul);
}
HRESULT Resource::Initialization(UINT Height, UINT Width)
{
	this->Height = Height;
	this->Width = Width;
	D3D11_TEXTURE2D_DESC Desc;
	Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Desc.Format = DXGI_FORMAT_R32_FLOAT;
	Desc.ArraySize = 1;
	Desc.Height = Height;
	Desc.Width = Width;
	Desc.MiscFlags = 0;
	Desc.SampleDesc.Quality = 0;
	Desc.SampleDesc.Count = 1;
	Desc.Usage = D3D11_USAGE_DYNAMIC;
	Desc.MipLevels = 1;
	HRESULT hr = m_Device->CreateTexture2D(&Desc,nullptr,&m_TextureLeft);
	if (FAILED(hr)) 
		return E_FAIL;
	  hr = m_Device->CreateTexture2D(&Desc, nullptr, &m_TextureRight);
	if (FAILED(hr))
		return E_FAIL;
	D3D11_SHADER_RESOURCE_VIEW_DESC ResourceDesc;
	ResourceDesc.Texture2D.MipLevels = 1;
	ResourceDesc.Texture2D.MostDetailedMip = 0;
	ResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	ResourceDesc.Format = DXGI_FORMAT_R32_FLOAT;

	 hr = m_Device->CreateShaderResourceView(m_TextureLeft, &ResourceDesc, &m_Matrix_Left);
	if (FAILED(hr))
		return E_FAIL;

	 hr = m_Device->CreateShaderResourceView(m_TextureRight, &ResourceDesc, &m_Matrix_Right);
	if (FAILED(hr))
		return E_FAIL;
	//Copy
	Desc.Usage = D3D11_USAGE_STAGING;
	Desc.BindFlags = 0;
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	 hr = m_Device->CreateTexture2D(&Desc, nullptr, &m_Copy);
	if (FAILED(hr))
		return E_FAIL;
	//UnorderAccess
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	Desc.CPUAccessFlags = 0;
	hr = m_Device->CreateTexture2D(&Desc, nullptr, &m_TextureRead);
	if (FAILED(hr))
		return E_FAIL;
	D3D11_UNORDERED_ACCESS_VIEW_DESC UnorderDesc;
	UnorderDesc.Format = DXGI_FORMAT_R32_FLOAT;
	UnorderDesc.Texture2D.MipSlice = 0;
	UnorderDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	 hr = m_Device->CreateUnorderedAccessView(m_TextureRead, &UnorderDesc, &m_Out);
	if (FAILED(hr))
		return E_FAIL;
	D3D11_BUFFER_DESC BufferDesc;
	BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	BufferDesc.ByteWidth = sizeof(MatrixSize);
	BufferDesc.StructureByteStride = sizeof(MatrixSize);
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	BufferDesc.MiscFlags = 0;
	hr = m_Device->CreateBuffer(&BufferDesc,nullptr, &m_Describe);
	if (FAILED(hr))
		return E_FAIL;
	std::fstream  Filer;
	Filer.open("C:\\Users\\e\\Desktop\\Tensorflow\\Debug\\Compute.cso", ios::binary | ios::in);
	Filer.seekg(0, ios_base::beg);
	Filer.seekg(0, ios_base::end);
	streampos  Length = Filer.tellg();
	Filer.seekg(0, ios_base::beg);
	CHAR* Data = new CHAR[static_cast<long>(Length)];
	Filer.read(Data, Length);
	hr = m_Device->CreateComputeShader(Data, Length, nullptr, &m_Shader_MatMul);
	if (FAILED(hr))
		return E_FAIL;
	delete[]Data;
	Filer.close();
	return S_OK;
}
VOID Resource::UpdataMatrix(FLOAT *LeftP, UINT LeftRow, UINT LeftColumn,
	FLOAT *RightP, UINT RightRow, UINT RightColumn)
{
	m_MatrixSize.Matrix.x = LeftRow;
	m_MatrixSize.Matrix.y = LeftColumn;
	m_MatrixSize.Matrix.z = RightRow;
	m_MatrixSize.Matrix.w = RightColumn;
	//Updata left matrix
	m_Context->Map(m_TextureLeft, 0, D3D11_MAP_WRITE_DISCARD, 0, &WriteContext);
	FLOAT* Write = reinterpret_cast<FLOAT*>(WriteContext.pData);
	for (UINT i = 0; i < LeftRow; ++i)
	{
			for (UINT j = 0; j < LeftColumn; ++j)
			{
				Write[i*Width + j] = LeftP[i*LeftColumn + j];
			}
	}
	m_Context->Unmap(m_TextureLeft, 0);
	m_Context->Map(m_TextureRight, 0, D3D11_MAP_WRITE_DISCARD, 0, &WriteContext);
	Write = reinterpret_cast<FLOAT*>(WriteContext.pData);
	for (UINT i = 0; i < RightRow; ++i)
	{
		for (UINT j = 0; j < RightColumn; ++j)
		{
			Write[i*Width+j] = RightP[i*RightColumn+j];
		}
	}
	m_Context->Unmap(m_TextureRight, 0);
	m_Context->Map(m_Describe, 0, D3D11_MAP_WRITE_DISCARD, 0, &WriteContext);
	MatrixSize *Size = reinterpret_cast<MatrixSize*>(WriteContext.pData);
	Size->Matrix.x = LeftRow;
	Size->Matrix.y = LeftColumn;
	Size->Matrix.z = RightRow;
	Size->Matrix.w = RightColumn;
	m_Context->Unmap(m_Describe, 0);
 
}
VOID Resource::GetResult(FLOAT *Dest)
{
	m_Context->CopyResource(m_Copy, m_TextureRead);
	m_Context->Map(m_Copy, 0, D3D11_MAP_READ, 0, &WriteContext);
	FLOAT *Data = reinterpret_cast<FLOAT *>(WriteContext.pData);
	for (UINT i = 0; i < m_MatrixSize.Matrix.x;++i)
	{
		for (UINT j = 0; j < m_MatrixSize.Matrix.w; ++j)
		{
			Dest[i*m_MatrixSize.Matrix.w +j] = Data[i*Width + j];
		}
	}
}