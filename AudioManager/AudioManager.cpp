#include "AudioManager.h"
#include <functiondiscoverykeys_devpkey.h>
#include <audiopolicy.h>


// ��ʼ��
void AudioManager::Initialize()
{
	CoInitialize(NULL);
	GetDevices(eRender, _mapPlaybackDevices);
	GetDevices(eCapture, _mapRecordingDevices);
}

// ���ʼ��
void AudioManager::Uninitialize()
{
	_mapPlaybackDevices.clear();
	_mapRecordingDevices.clear();
	CoUninitialize();
}


// ��ȡ�ط��豸����
UINT AudioManager::GetPlaybackDeviceCount()
{
	return (UINT)_mapPlaybackDevices.size();
}


// ��ȡ�ط��豸
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


// ��ȡ¼���豸����
UINT AudioManager::GetRecordingDeviceCount()
{
	return (UINT)_mapRecordingDevices.size();
}


// ��ȡ¼���豸
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


// ��ȡͨ��
void AudioManager::GetChannels()
{
	CComPtr<IMMDeviceEnumerator> spEnumerator;
	spEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
	if (!spEnumerator)
	{
		std::cout << "��ȡIMMDeviceEnumerator�ӿ�ʧ�ܡ�" << std::endl;
		return;
		std::cout << "��ȡIMMDeviceEnumerator�ӿ�ʧ�ܡ�" << std::endl;
	}
	CComPtr<IMMDeviceCollection> spDevices;
	spEnumerator->EnumAudioEndpoints(eRender, eMultimedia, &spDevices);
	if (!spDevices)
	{
		std::cout << "��ȡIMMDeviceCollection�ӿ�ʧ�ܡ�" << std::endl;
		return;
	}
	CComPtr<IMMDevice> spDevice;
	spEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &spDevice);
	if (!spDevice)
	{
		std::cout << "��ȡIMMDeviceʧ�ܡ�" << std::endl;
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
		std::wcout << "�Ự���ƣ�" << pwszDisplayName << std::endl;
	}
}


// �������ͻ�ȡ�豸�б�
void AudioManager::GetDevices(EDataFlow dataFlow, std::map <std::wstring, CComPtr<IMMDevice>>& mapDevices)
{
	mapDevices.clear();
	CComPtr<IMMDeviceEnumerator> spEnumerator;
	spEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
	if (!spEnumerator)
	{
		std::cout << "��ȡIMMDeviceEnumerator�ӿ�ʧ�ܡ�" << std::endl;
		return;
		std::cout << "��ȡIMMDeviceEnumerator�ӿ�ʧ�ܡ�" << std::endl;
	}
	CComPtr<IMMDeviceCollection> spDevices;
	spEnumerator->EnumAudioEndpoints(dataFlow, eMultimedia, &spDevices);
	if (!spDevices)
	{
		std::cout << "��ȡIMMDeviceCollection�ӿ�ʧ�ܡ�" << std::endl;
		return;
	}
	UINT count = 0;
	spDevices->GetCount(&count);
	if (!count)
	{
		std::cout << "��ȡ��0���豸��" << std::endl;
		return;
	}
	for (UINT i = 0; i < count; i++)
	{
		CComPtr<IMMDevice> spDevice;
		spDevices->Item(i, &spDevice);
		if (!spDevice)
		{
			std::cout << "��ȡIMMDevice�ӿ�ʧ�ܡ�" << std::endl;
			continue;
		}
		CComPtr<IPropertyStore> spStore;
		spDevice->OpenPropertyStore(STGM_READ, &spStore);
		if (!spStore)
		{
			std::cout << "��ȡIPropertyStore�ӿ�ʧ�ܡ�" << std::endl;
			continue;
		}
		PROPVARIANT pv;
		spStore->GetValue(PKEY_Device_FriendlyName, &pv);
		if (pv.vt == VT_LPWSTR)
		{
			mapDevices[pv.pwszVal] = spDevice;
		}
		else {
			std::cout << "PropertyVariant��vt����VT_LPWSTR" << std::endl;
		}
		PropVariantClear(&pv);
	}
}
