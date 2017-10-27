#pragma once
#include <d3d11.h>
class Device
{
public:
	  Device();
	~Device();
	HRESULT CreateDeviceContext();
protected:
	ID3D11Device*               m_Device;
	ID3D11DeviceContext* m_Context;
};

