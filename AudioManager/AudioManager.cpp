#include "AudioManager.h"


// 初始化
void AudioManager::Initialize()
{
	CoInitialize(NULL);
	this->GetDevices(eRender, _mapPlaybackDevices);
	this->GetDevices(eCapture, _mapRecordingDevices);
	this->GetAllSession();
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
		CComPtr<IAudioSessionControl2> spSession2;
		spSession->QueryInterface(__uuidof(IAudioSessionControl2), reinterpret_cast<void**>(&spSession2));
		DWORD dwProcessId = 0;
		spSession2->GetProcessId(&dwProcessId);
		wchar_t szPath[MAX_PATH] = { 0 };
		HANDLE hProcess = NULL;
		hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
		DWORD dwLength = MAX_PATH;
		QueryFullProcessImageNameW(hProcess, 0, szPath, &dwLength);
		CloseHandle(hProcess);
		std::wstring name = szPath;
		if (!name.empty())
		{
			std::wstring::size_type pos1 = name.rfind(L'\\');
			std::wstring::size_type pos2 = name.rfind(L'.');
			name = name.substr(pos1, pos2 - pos1);
			std::wcout << name << std::endl;
		}
		else
		{
			HRESULT hr = spSession2->IsSystemSoundsSession();
			if (hr != S_FALSE)
			{
				std::wcout << L"系统声音\n";
			}
			else
			{
				//std::wcout << L"不是系统声音\n";
			}
		}
	}
}


// 获取会话数量
UINT AudioManager::GetSessionCount()
{
	return (UINT)_listSession.size();
}


// 获取会话
AUDIO_CONTROL_SESSION_ENTITY AudioManager::GetSession(UINT nIndex) const
{
	return _listSession[nIndex];
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


// 获取所有会话
void AudioManager::GetAllSession()
{
	HRESULT hr = S_OK;
	CComPtr<IMMDeviceEnumerator> spDeviceEnumerator;
	hr = spDeviceEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
	if (FAILED(hr))
	{
		std::cout << "Get IMMDeviceEnumerator failed." << std::endl;
		return;
	}
	CComPtr<IMMDeviceCollection> spDeviceCollection;
	hr = spDeviceEnumerator->EnumAudioEndpoints(eRender, eMultimedia, &spDeviceCollection);
	if (FAILED(hr))
	{
		std::cout << "Get IMMDeviceCollection failed." << std::endl;
		return;
	}
	UINT uDeviceCount = 0;
	hr = spDeviceCollection->GetCount(&uDeviceCount);
	if (FAILED(hr) || uDeviceCount == 0)
	{
		std::cout << "Get device count failed." << std::endl;
		return;
	}
	for (UINT i = 0; i < uDeviceCount; i++)
	{
		CComPtr<IMMDevice> spDevice;
		hr = spDeviceCollection->Item(i, &spDevice);
		if (FAILED(hr))
		{
			std::cout << "Get device failed." << std::endl;
			return;
		}
		CComPtr<IAudioSessionManager2> spSessionManager;
		hr = spDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, NULL, reinterpret_cast<void**>(&spSessionManager));
		if (FAILED(hr))
		{
			std::cout << "Get AudioSessionManager2 failed." << std::endl;
			return;
		}
		CComPtr<IAudioSessionEnumerator> spSessionEnumerator;
		hr = spSessionManager->GetSessionEnumerator(&spSessionEnumerator);
		if (FAILED(hr))
		{
			std::cout << "Get IAudioSessionEnumerator failed." << std::endl;
			return;
		}
		int nSessionCount = 0;
		hr = spSessionEnumerator->GetCount(&nSessionCount);
		if (FAILED(hr) || nSessionCount == 0)
		{
			std::cout << "Get session count failed." << std::endl;
			return;
		}
		for (int j = 0; j < nSessionCount; j++)
		{
			CComPtr<IAudioSessionControl> spSession;
			hr = spSessionEnumerator->GetSession(j, &spSession);
			if (FAILED(hr))
			{
				std::cout << "Get session failed." << std::endl;
				continue;
			}
			CComPtr<IAudioSessionControl2> spSession2;
			hr = spSession->QueryInterface(__uuidof(IAudioSessionControl2), reinterpret_cast<void**>(&spSession2));
			if (FAILED(hr))
			{
				std::cout << "Get session2 failed." << std::endl;
				continue;
			}
			DWORD dwProcessId = 0;
			hr = spSession2->GetProcessId(&dwProcessId);
			if (FAILED(hr))
			{
				std::cout << "Get process ID failed." << std::endl;
				continue;
			}
			DWORD dwLength = MAX_PATH;
			WCHAR szImageName[MAX_PATH] = { 0 };
			HANDLE hProcess = NULL;
			hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, 0, dwProcessId);
			if (hProcess == NULL)
			{
				std::cout << "Get process handle failed." << std::endl;
				continue;
			}
			QueryFullProcessImageName(hProcess, 0, szImageName, &dwLength);
			if (!szImageName[0])
			{
				std::cout << "Get image name failed." << std::endl;
				continue;
			}
			LPTSTR left = 0;
			LPTSTR right = 0;
			left = StrRStrIW(szImageName, nullptr, L"\\");
			right = StrRStrI(left, nullptr, L".");
			if (left == nullptr || right == nullptr)
			{
				std::cout << "Join name failed." << std::endl;
				continue;
			}
			AUDIO_CONTROL_SESSION_ENTITY entity;
			_tcsncpy_s(entity.szName, MAX_PATH, (left + 1), (int)(right - left - 1));
			LPWSTR lpwSessionId = nullptr;
			spSession2->GetSessionIdentifier(&lpwSessionId);
			_tcscpy_s(entity.szId, MAX_PATH, lpwSessionId);
			CoTaskMemFree(lpwSessionId);
			entity.spObject = spSession;
			this->_listSession.push_back(entity);
		}
	}
}
