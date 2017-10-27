#include "Device.h"



Device::Device()
{

}
Device::~Device()
{
	if (m_Context)
		m_Context->Release();
	if (m_Device)
		m_Device->Release();
}
HRESULT Device::CreateDeviceContext()
{
#if defined(_DEBUG)

	D3D11_CREATE_DEVICE_FLAG creationFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};
	D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE;

	HRESULT hr = D3D11CreateDevice(
		nullptr, driverType, 0,
		creationFlags, featureLevels,
		ARRAYSIZE(featureLevels),
		D3D11_SDK_VERSION, &m_Device,
		nullptr, &m_Context
	);
	return hr;
}