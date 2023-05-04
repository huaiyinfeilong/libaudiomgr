#include "AudioManager.h"
#include <functiondiscoverykeys_devpkey.h>
#include <audiopolicy.h>


// 初始化
void AudioManager::Initialize()
{
	CoInitialize(NULL);
	GetDevices(eRender, _mapPlaybackDevices);
	GetDevices(eCapture, _mapRecordingDevices);
}

// 逆初始化
void AudioManager::Uninitialize()
{
	_mapPlaybackDevices.clear();
	_mapRecordingDevices.clear();
	CoUninitialize();
}


// 获取回放设备数量
UINT AudioManager::GetPlaybackDeviceCount()
{
	return (UINT)_mapPlaybackDevices.size();
}


// 获取回放设备
LPCWSTR AudioManager::GetPlaybackDevice(UINT nIndex)
{
	UINT i = 0;
	UINT count = GetPlaybackDeviceCount();
	for (auto it = _mapPlaybackDevices.begin(); i < count && it != _mapPlaybackDevices.end(); it++, i++)
	{
		if (i == nIndex)
		{
			return it->first.c_str();
		}
	}
	return nullptr;
}


// 获取录音设备数量
UINT AudioManager::GetRecordingDeviceCount()
{
	return (UINT)_mapRecordingDevices.size();
}


// 获取录音设备
LPCWSTR AudioManager::GetRecordingDevice(UINT nIndex)
{
	UINT i = 0;
	UINT count = GetRecordingDeviceCount();
	for (auto it = _mapRecordingDevices.begin(); i < count && it != _mapRecordingDevices.end(); it++, i++)
	{
		if (i == nIndex)
		{
			return it->first.c_str();
		}
	}
	return nullptr;
}


// 获取通道
void AudioManager::GetChannels()
{
	CComPtr<IMMDeviceEnumerator> spEnumerator;
	spEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
	if (!spEnumerator)
	{
		std::cout << "获取IMMDeviceEnumerator接口失败。" << std::endl;
		return;
		std::cout << "获取IMMDeviceEnumerator接口失败。" << std::endl;
	}
	CComPtr<IMMDeviceCollection> spDevices;
	spEnumerator->EnumAudioEndpoints(eRender, eMultimedia, &spDevices);
	if (!spDevices)
	{
		std::cout << "获取IMMDeviceCollection接口失败。" << std::endl;
		return;
	}
	CComPtr<IMMDevice> spDevice;
	spEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &spDevice);
	if (!spDevice)
	{
		std::cout << "获取IMMDevice失败。" << std::endl;
		return;
	}
	CComPtr<IAudioSessionManager2> spSessionManager;
	spDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_INPROC_SERVER, nullptr, reinterpret_cast<void**>(&spSessionManager));
	CComPtr<IAudioSessionEnumerator> spSessionEnumerator;
	spSessionManager->GetSessionEnumerator(&spSessionEnumerator);
	int count = 0;
	spSessionEnumerator->GetCount(&count);
	for (int i = 0; i < count; i++)
	{
		CComPtr<IAudioSessionControl> spSession;
		spSessionEnumerator->GetSession(i, &spSession);
		LPWSTR pwszDisplayName = nullptr;
		spSession->GetDisplayName(&pwszDisplayName);
		std::wcout << "会话名称：" << pwszDisplayName << std::endl;
	}
}


// 根据类型获取设备列表
void AudioManager::GetDevices(EDataFlow dataFlow, std::map <std::wstring, CComPtr<IMMDevice>>& mapDevices)
{
	mapDevices.clear();
	CComPtr<IMMDeviceEnumerator> spEnumerator;
	spEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
	if (!spEnumerator)
	{
		std::cout << "获取IMMDeviceEnumerator接口失败。" << std::endl;
		return;
		std::cout << "获取IMMDeviceEnumerator接口失败。" << std::endl;
	}
	CComPtr<IMMDeviceCollection> spDevices;
	spEnumerator->EnumAudioEndpoints(dataFlow, eMultimedia, &spDevices);
	if (!spDevices)
	{
		std::cout << "获取IMMDeviceCollection接口失败。" << std::endl;
		return;
	}
	UINT count = 0;
	spDevices->GetCount(&count);
	if (!count)
	{
		std::cout << "获取到0个设备。" << std::endl;
		return;
	}
	for (UINT i = 0; i < count; i++)
	{
		CComPtr<IMMDevice> spDevice;
		spDevices->Item(i, &spDevice);
		if (!spDevice)
		{
			std::cout << "获取IMMDevice接口失败。" << std::endl;
			continue;
		}
		CComPtr<IPropertyStore> spStore;
		spDevice->OpenPropertyStore(STGM_READ, &spStore);
		if (!spStore)
		{
			std::cout << "获取IPropertyStore接口失败。" << std::endl;
			continue;
		}
		PROPVARIANT pv;
		spStore->GetValue(PKEY_Device_FriendlyName, &pv);
		if (pv.vt == VT_LPWSTR)
		{
			mapDevices[pv.pwszVal] = spDevice;
		}
		else {
			std::cout << "PropertyVariant的vt不是VT_LPWSTR" << std::endl;
		}
		PropVariantClear(&pv);
	}
}
