#include "AudioManager.h"
#include <windowsx.h>
#include <oleacc.h>


// 初始化
void AudioManager::Initialize()
{
	CoInitialize(NULL);
	this->GetDevices(eRender, this->_listPlaybackDevice);
	this->GetDevices(eCapture, this->_listRecordingDevice);
	this->GetAllSession();
}

// 逆初始化
void AudioManager::Uninitialize()
{
	this->_listPlaybackDevice.clear();
	this->_listRecordingDevice.clear();
	this->_listSession.clear();
	CoUninitialize();
}


// 获取回放设备数量
UINT AudioManager::GetPlaybackDeviceCount()
{
	return (UINT)this->_listPlaybackDevice.size();
}


// 获取回放设备
AUDIO_CONTROL_DEVICE_ENTITY AudioManager::GetPlaybackDevice(UINT nIndex)
{
	return this->_listPlaybackDevice[nIndex];
}


// 获取录音设备数量
UINT AudioManager::GetRecordingDeviceCount()
{
	return (UINT)this->_listRecordingDevice.size();
}


// 获取录音设备
AUDIO_CONTROL_DEVICE_ENTITY AudioManager::GetRecordingDevice(UINT nIndex)
{
	return this->_listRecordingDevice[nIndex];
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
void AudioManager::GetDevices(EDataFlow dataFlow, std::vector<AUDIO_CONTROL_DEVICE_ENTITY>& listDevice)
{
	listDevice.clear();
	CComPtr<IMMDeviceEnumerator> spEnumerator;
	HRESULT hr = spEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
	if (FAILED(hr))
	{
		std::cout << "获取IMMDeviceEnumerator接口失败。" << std::endl;
		return;
	}
	CComPtr<IMMDeviceCollection> spDevices;
	hr = spEnumerator->EnumAudioEndpoints(dataFlow, eMultimedia, &spDevices);
	if (FAILED(hr))
	{
		std::cout << "获取IMMDeviceCollection接口失败。" << std::endl;
		return;
	}
	UINT count = 0;
	hr = spDevices->GetCount(&count);
	if (FAILED(hr) || !count)
	{
		std::cout << "获取到0个设备。" << std::endl;
		return;
	}
	for (UINT i = 0; i < count; i++)
	{
		CComPtr<IMMDevice> spDevice;
		hr = spDevices->Item(i, &spDevice);
		if (FAILED(hr))
		{
			std::cout << "获取IMMDevice接口失败。" << std::endl;
			continue;
		}
		CComPtr<IPropertyStore> spStore;
		hr = spDevice->OpenPropertyStore(STGM_READ, &spStore);
		if (FAILED(hr))
		{
			std::cout << "获取IPropertyStore接口失败。" << std::endl;
			continue;
		}
		PROPVARIANT pv;
		hr = spStore->GetValue(PKEY_Device_FriendlyName, &pv);
		if (FAILED(hr))
		{
			continue;
		}
		AUDIO_CONTROL_DEVICE_ENTITY entity;
		if (pv.vt == VT_LPWSTR)
		{
			entity.strName = pv.pwszVal;
		}
		else
		{
			std::cout << "PropertyVariant的vt不是VT_LPWSTR" << std::endl;
		}
		PropVariantClear(&pv);
		LPTSTR lpDeviceId = nullptr;
		hr = spDevice->GetId(&lpDeviceId);
		if (FAILED(hr))
		{
			continue;
		}
		entity.strId = lpDeviceId;
		CoTaskMemFree(lpDeviceId);
		entity.spObject = spDevice;
		listDevice.push_back(entity);
	}
}


// 获取所有会话
void AudioManager::GetAllSession()
{
	// 获取当前所有窗口及其进程ID
	std::map<DWORD, CString> context;
	GetWindows(context);
	// 获取所有session
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
	CComPtr<IMMDevice> spDefaultDevice;
	hr = spDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &spDefaultDevice);
	if (FAILED(hr))
	{
		std::cout << "Get default device failed." << std::endl;
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
		hr = spDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_INPROC_SERVER, NULL, reinterpret_cast<void**>(&spSessionManager));
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
			if (spSession2->IsSystemSoundsSession() != S_FALSE)
			{
				LPWSTR lpwDefaultDeviceId = nullptr;
				LPWSTR lpwCurrentDeviceId = nullptr;
				hr = spDefaultDevice->GetId(&lpwDefaultDeviceId);
				if (FAILED(hr))
				{
					std::cout << "Get default device id failed." << std::endl;
					continue;
				}
				hr = spDevice->GetId(&lpwCurrentDeviceId);
				if (FAILED(hr))
				{
					std::cout << "Get current device id failed." << std::endl;
					CoTaskMemFree(lpwDefaultDeviceId);
					continue;
				}
				if (_tcscmp(lpwDefaultDeviceId, lpwCurrentDeviceId))
				{
					CoTaskMemFree(lpwDefaultDeviceId);
					CoTaskMemFree(lpwCurrentDeviceId);
					continue;
				}
				CoTaskMemFree(lpwDefaultDeviceId);
				CoTaskMemFree(lpwCurrentDeviceId);
				AUDIO_CONTROL_SESSION_ENTITY entity;
				LPWSTR lpwSessionId = nullptr;
				hr = spSession2->GetSessionInstanceIdentifier(&lpwSessionId);
				if (FAILED(hr) || lpwSessionId == nullptr)
				{
					continue;
				}
				entity.strId = lpwSessionId;
				entity.strName = TEXT("系统音量");
				entity.spObject = spSession;
				_listSession.push_back(entity);
				CoTaskMemFree(lpwSessionId);
				continue;
			}
			DWORD dwProcessId = 0;
			hr = spSession2->GetProcessId(&dwProcessId);
			if (FAILED(hr))
			{
				std::cout << "Get process ID failed." << std::endl;
				continue;
			}
			/*
						if (context.find(dwProcessId) != context.end() && !context[dwProcessId].IsEmpty())
						{
							AUDIO_CONTROL_SESSION_ENTITY entity;
							LPWSTR lpwSessionId = nullptr;
							spSession2->GetSessionInstanceIdentifier(&lpwSessionId);
							if (lpwSessionId)
							{
								entity.strId = lpwSessionId;
								CoTaskMemFree(lpwSessionId);
							}
							entity.strName = context[dwProcessId];
							entity.spObject = spSession;
							this->_listSession.push_back(entity);
							continue;
						}
			*/
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
			LPWSTR lpwDisplayName = nullptr;
			hr = spSession2->GetDisplayName(&lpwDisplayName);
			if (FAILED(hr) || lpwDisplayName == nullptr || !*lpwDisplayName)
			{
				std::cout << "Get display name failed." << std::endl;
				left[right - left] = TEXT('\0');
				entity.strName = (left + 1);
			}
			else
			{
				entity.strName = lpwDisplayName;
				CoTaskMemFree(lpwDisplayName);
			}
			LPWSTR lpwSessionId = nullptr;
			hr = spSession2->GetIconPath(&lpwSessionId);
			if (FAILED(hr))
			{
				std::cout << "Get session id failed." << std::endl;
				continue;
			}
			entity.strId = lpwSessionId;
			CoTaskMemFree(lpwSessionId);
			entity.spObject = spSession;
			this->_listSession.push_back(entity);
		}
	}
}


// 通过进程ID获取窗口标题
BOOL AudioManager::GetWindows(std::map<DWORD, CString>& context)
{
	DWORD dwProcessId = 0;
	HWND hWnd = FindWindowEx(NULL, NULL, NULL, NULL);
	while (hWnd != NULL)
	{
		DWORD dwStyle = GetWindowStyle(hWnd);
		if (!(dwStyle & (WS_OVERLAPPED | WS_POPUP)) || dwStyle & WS_DISABLED)
		{
			hWnd = FindWindowEx(NULL, hWnd, NULL, NULL);
			continue;
		}
		GetWindowThreadProcessId(hWnd, &dwProcessId);
		if (dwProcessId == 0)
		{
			hWnd = FindWindowEx(NULL, hWnd, NULL, NULL);
			continue;
		}
		TCHAR szTitle[MAX_PATH] = { 0 };
		GetWindowText(hWnd, szTitle, MAX_PATH);
		if (!*szTitle)
		{
			hWnd = FindWindowEx(NULL, hWnd, NULL, NULL);
			continue;
		}
		context.insert(std::pair<DWORD, CString>(dwProcessId, szTitle));
		hWnd = FindWindowEx(NULL, hWnd, NULL, NULL);
	}

	return TRUE;
}
