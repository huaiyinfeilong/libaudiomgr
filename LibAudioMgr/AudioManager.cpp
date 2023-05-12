#include "pch.h"
#include "AudioManager.h"
#include <windowsx.h>
#include <oleacc.h>
#include <endpointvolume.h>
#include <audiopolicy.h>
#include "PolicyConfig.h"
#include <fstream>
#include<Audioclient.h>
#include <log4cxx/logger.h>


#define LIBAUDIOMGR_DEBUG

#define LOG(msg) write_log(_T("D:\\log.txt"), _T(__FILE__), __LINE__, _T(__FUNCTION__), msg)

log4cxx::LoggerPtr logger = log4cxx::Logger::getRootLogger();

void write_log(const wchar_t* logfilename, const wchar_t* filename, size_t line, const wchar_t* function, const wchar_t* message)
{
#ifndef LIBAUDIOMGR_DEBUG
	return;
#endif
	try
	{
		std::wstringstream stream;
		stream << "File: " << filename << std::endl
			<< "Line: " << line << std::endl
			<< "Function: " << function << std::endl
			<< "Message: " << message << std::endl;
		LOG4CXX_INFO(logger, stream.str().c_str());
	}
	catch (std::exception&)
	{
	}
}


void MsgBox(LPCTSTR lpText)
{
	MessageBox(GetForegroundWindow(), lpText, TEXT(""), MB_OK);
}


// 初始化
void AudioManager::Initialize()
{
	this->Uninitialize();
	CoInitializeEx(NULL, NULL);
	this->GetDevices(eRender, eMultimedia, this->_listPlaybackDevice);
	this->GetDevices(eCapture, eCommunications, this->_listRecordingDevice);
	this->GetAllSession();
}

// 逆初始化
void AudioManager::Uninitialize()
{
	this->_listPlaybackDevice.clear();
	this->_listRecordingDevice.clear();
	this->_listSession.clear();
	// 在一些程序中，如果调用CoUninitialize（）将导致异常
	// CoUninitialize();
}


// 获取回放设备数量
DWORD AudioManager::GetPlaybackDeviceCount()
{
	return (DWORD)this->_listPlaybackDevice.size();
}


// 获取回放设备
AUDIO_CONTROL_DEVICE_ENTITY AudioManager::GetPlaybackDevice(DWORD nIndex)
{
	return this->_listPlaybackDevice[nIndex];
}


// 获取录音设备数量
DWORD AudioManager::GetRecordingDeviceCount()
{
	return (DWORD)this->_listRecordingDevice.size();
}


// 获取录音设备
AUDIO_CONTROL_DEVICE_ENTITY AudioManager::GetRecordingDevice(DWORD nIndex)
{
	return this->_listRecordingDevice[nIndex];
}


// 获取会话数量
DWORD AudioManager::GetSessionCount()
{
	return (DWORD)_listSession.size();
}


// 获取会话
AUDIO_CONTROL_SESSION_ENTITY AudioManager::GetSession(DWORD nIndex) const
{
	return _listSession[nIndex];
}


// 根据类型获取设备列表
void AudioManager::GetDevices(EDataFlow dataFlow, ERole eRole, std::vector<AUDIO_CONTROL_DEVICE_ENTITY>& listDevice)
{
	listDevice.clear();
	CComPtr<IMMDeviceEnumerator> spEnumerator;
	HRESULT hr = spEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
	if (FAILED(hr))
	{
		LOG(_T("获取IMMDeviceEnumerator接口失败。"));
		return;
	}
	CComPtr<IMMDeviceCollection> spDevices;
	hr = spEnumerator->EnumAudioEndpoints(dataFlow, eMultimedia, &spDevices);
	if (FAILED(hr))
	{
		LOG(_T("IMMDeviceEnumerator::EnumAudioEndpoints failed."));
		return;
	}
	UINT count = 0;
	hr = spDevices->GetCount(&count);
	if (FAILED(hr) || !count)
	{
		LOG(_T("获取到0个设备。"));
		return;
	}
	for (DWORD i = 0; i < count; i++)
	{
		CComPtr<IMMDevice> spDevice;
		hr = spDevices->Item(i, &spDevice);
		if (FAILED(hr))
		{
			LOG(_T("Get device failed."));
			continue;
		}
		CComPtr<IPropertyStore> spStore;
		hr = spDevice->OpenPropertyStore(STGM_READ, &spStore);
		if (FAILED(hr))
		{
			LOG(_T("获取IPropertyStore接口失败。"));
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
			LOG(_T("PropertyVariant的vt不是VT_LPWSTR"));
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


// 设置默认播放、录音设备
void AudioManager::SetDefaultDevice(LPCTSTR lpDeviceId, ERole eRole)
{
	CComPtr<IPolicyConfig> spConfig;
	HRESULT hr = spConfig.CoCreateInstance(__uuidof(CPolicyConfigClient));
	if (FAILED(hr))
	{
		CString strError = _T("");
		strError.Format(_T("获取IpolicyConfig接口失败： hr=0x%x, error code=0x%x"), hr, GetLastError());
		LOG((LPCTSTR)strError);
		return;
	}
	spConfig->SetDefaultEndpoint(lpDeviceId, eRole);
}


// 获取默认播放、录音设备
DWORD AudioManager::GetDefaultDevice(EDataFlow eDataFlow, ERole eRole)
{
	// 获取IMMDeviceEnumerator接口
	CComPtr<IMMDeviceEnumerator> spEnumerator;
	HRESULT hr = spEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
	if (FAILED(hr))
	{
		LOG(_T("获取IMMDeviceEnumerator接口失败。"));
		return -1;
	}
	// 获取默认设备
	CComPtr<IMMDevice> spDevice;
	hr = spEnumerator->GetDefaultAudioEndpoint(eDataFlow, eRole, &spDevice);
	if (FAILED(hr))
	{
		LOG(_T("获取IMMDevice接口失败。"));
		return -1;
	}
	// 获取设备ID
	LPWSTR lpDeviceId = nullptr;
	hr = spDevice->GetId(&lpDeviceId);
	if (FAILED(hr))
	{
		LOG(_T("获取设备ID失败。"));
		return -1;
	}
	// 找出默认设备在列表中的索引下标并返回
	CString strId = lpDeviceId;
	CoTaskMemFree(lpDeviceId);
	if (eDataFlow == eRender)
	{
		for (auto it = _listPlaybackDevice.begin(); it != _listPlaybackDevice.end(); it++)
		{
			if (strId == it->strId)
			{
				return (DWORD)(std::distance(_listPlaybackDevice.begin(), it));
			}
		}
	}
	else if (eDataFlow == eCapture)
	{
		for (auto it = _listRecordingDevice.begin(); it != _listRecordingDevice.end(); it++)
		{
			if (strId == it->strId)
			{
				return (DWORD)std::distance(_listRecordingDevice.begin(), it);
			}
		}
	}

	return -1;
}


// 设置会话播放、录音设备
void AudioManager::SetSessionDevice(DWORD dwIndex, CComPtr<IMMDevice>& spDevice)
{
	AUDIO_CONTROL_SESSION_ENTITY entity = this->GetSession(dwIndex);
	CComPtr<IAudioSessionControl>& spSession = entity.spObject;
	if (!spSession)
	{
		LOG(_T("IAudioSessionControl对象为空。"));
		return;
	}
	CComPtr<IAudioSessionControl2> spSession2;
	HRESULT hr = spSession->QueryInterface(__uuidof(IAudioSessionControl2), reinterpret_cast<void**>(&spSession2));
	if (FAILED(hr))
	{
		LOG(_T("获取IAudioSessionControl2接口失败。"));
		return;
	}
	CComPtr<IAudioSessionManager> spManager;
	hr = spSession2->QueryInterface(__uuidof(IAudioSessionManager), reinterpret_cast<void**>(&spManager));
	if (FAILED(hr))
	{
		LOG(_T("获取IAudioSessionManager接口失败。"));
		return;
	}
	CComPtr<IAudioSessionManager2> spManager2;
	hr = spManager->QueryInterface(__uuidof(IAudioSessionManager2), reinterpret_cast<void**>(&spManager2));
	if (FAILED(hr))
	{
		LOG(_T("获取IAudioSessionManager2接口失败。"));
		return;
	}
	
}


// 获取所有会话
void AudioManager::GetAllSession()
{
	// 存放会话进程到会话对象的map，以过滤同名且不在当前默认设备的会话
	std::map<DWORD, AUDIO_CONTROL_SESSION_ENTITY> mapSession;
	// 获取所有session
	HRESULT hr = S_OK;
	CComPtr<IMMDeviceEnumerator> spDeviceEnumerator;
	hr = spDeviceEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
	if (FAILED(hr))
	{
		LOG(_T("Get IMMDeviceEnumerator failed."));
		return;
	}
	CComPtr<IMMDeviceCollection> spDeviceCollection;
	hr = spDeviceEnumerator->EnumAudioEndpoints(eRender, eMultimedia, &spDeviceCollection);
	if (FAILED(hr))
	{
		LOG(_T("Get IMMDeviceCollection failed."));
		return;
	}
	UINT uDeviceCount = 0;
	hr = spDeviceCollection->GetCount(&uDeviceCount);
	if (FAILED(hr) || uDeviceCount == 0)
	{
		LOG(_T("Get device count failed."));
		return;
	}
	CComPtr<IMMDevice> spDefaultDevice;
	hr = spDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &spDefaultDevice);
	if (FAILED(hr))
	{
		LOG(_T("Get default device failed."));
		return;
	}
	// 设备是否为当前默认设备
	BOOL bDefaultDevice = FALSE;
	for (DWORD i = 0; i < uDeviceCount; i++)
	{
		CComPtr<IMMDevice> spDevice;
		hr = spDeviceCollection->Item(i, &spDevice);
		if (FAILED(hr))
		{
			LOG(_T("Get device failed."));
			continue;
		}
		CComPtr<IAudioSessionManager2> spSessionManager;
		hr = spDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_INPROC_SERVER, NULL, reinterpret_cast<void**>(&spSessionManager));
		if (FAILED(hr))
		{
			LOG(_T("Get AudioSessionManager2 failed."));
			continue;
		}
		CComPtr<IAudioSessionEnumerator> spSessionEnumerator;
		hr = spSessionManager->GetSessionEnumerator(&spSessionEnumerator);
		if (FAILED(hr))
		{
			LOG(_T("Get IAudioSessionEnumerator failed."));
			continue;
		}
		int nSessionCount = 0;
		hr = spSessionEnumerator->GetCount(&nSessionCount);
		if (FAILED(hr) || nSessionCount == 0)
		{
			LOG(_T("Get session count failed."));
			continue;
		}
		for (int j = 0; j < nSessionCount; j++)
		{
			CComPtr<IAudioSessionControl> spSession;
			hr = spSessionEnumerator->GetSession(j, &spSession);
			if (FAILED(hr))
			{
				LOG(_T("Get session failed."));
				continue;
			}
			CComPtr<IAudioSessionControl2> spSession2;
			hr = spSession->QueryInterface(__uuidof(IAudioSessionControl2), reinterpret_cast<void**>(&spSession2));
			if (FAILED(hr))
			{
				LOG(_T("Get session2 failed."));
				continue;
			}
			AudioSessionState state;
			hr = spSession->GetState(&state);
			if (FAILED(hr))
			{
				LOG(_T("Get the session state failed."));
				continue;
			}
			if (state != AudioSessionStateActive && state != AudioSessionStateInactive)
			{
				LOG(_T("The session is not active and inactive."));
				continue;
			}
			DWORD dwProcessId = 0;
			hr = spSession2->GetProcessId(&dwProcessId);
			if (FAILED(hr))
			{
				LOG(_T("Get the process IDof the session is failed."));
				continue;
			}
			// 如果相同进程的会话已经存在则跳过
			if (mapSession.find(dwProcessId) != mapSession.end())
			{
				continue;
			}
			LPWSTR lpwDefaultDeviceId = nullptr;
			LPWSTR lpwCurrentDeviceId = nullptr;
			hr = spDefaultDevice->GetId(&lpwDefaultDeviceId);
			if (FAILED(hr))
			{
				LOG(_T("Get default device id failed."));
				continue;
			}
			hr = spDevice->GetId(&lpwCurrentDeviceId);
			if (FAILED(hr))
			{
				LOG(_T("Get current device id failed."));
				CoTaskMemFree(lpwDefaultDeviceId);
				continue;
			}
			if (_tcscmp(lpwDefaultDeviceId, lpwCurrentDeviceId))
			{
				bDefaultDevice = FALSE;
			}
			else
			{
				bDefaultDevice = TRUE;
			}
			CoTaskMemFree(lpwDefaultDeviceId);
			CoTaskMemFree(lpwCurrentDeviceId);
			AUDIO_CONTROL_SESSION_ENTITY entity;
			if (spSession2->IsSystemSoundsSession() != S_FALSE)
			{
				entity.strName = _T("系统声音");
			}
			else
			{
				if (!this->GetSessionDisplayName(spSession2, entity.strName))
				{
					LOG(_T("Get session display name failed."));
					continue;
				}
			}
			LPTSTR lpSessionId = nullptr;
			hr = spSession2->GetSessionInstanceIdentifier(&lpSessionId);
			if (FAILED(hr) || !lpSessionId)
			{
				LOG(_T("Get session id failed."));
				continue;
			}
			entity.strId = lpSessionId;
			CoTaskMemFree(lpSessionId);
			entity.spObject = spSession;
			mapSession[dwProcessId] = entity;
		}
	}
	for (auto it = mapSession.begin(); it != mapSession.end(); it++)
	{
		this->_listSession.push_back(it->second);
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


// 获取会话名称
BOOL AudioManager::GetSessionDisplayName(CComPtr<IAudioSessionControl2>& spSession, CString& strDisplayName)
{
	LPTSTR lpDisplayName = nullptr;
	HRESULT hr = spSession->GetDisplayName(&lpDisplayName);
	if (FAILED(hr) || lpDisplayName == nullptr)
	{
		LOG(_T("IAudioSessionController2::GetDisplayName failed."));
		return FALSE;
	}
	// 成功获取到名称
	if (*lpDisplayName)
	{
		LOG(lpDisplayName);
		strDisplayName = lpDisplayName;
		CoTaskMemFree(lpDisplayName);
		return TRUE;
	}
	// 获取名称失败，尝试获取exe文件描述字段
	// 首先通过进程ID获取进程完整路径
	// 然后通过进程完整路径得到文件信息中的文件描述字段内容
	CoTaskMemFree(lpDisplayName);
	// 获取进程ID
	DWORD dwProcessId = 0;
	hr = spSession->GetProcessId(&dwProcessId);
	if (FAILED(hr) || dwProcessId == 0)
	{
		LOG(_T("Get process ID of the session failed."));
		return FALSE;
	}
	// 通过进程ID得到进程路径
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
	if (hProcess == NULL)
	{
		LOG(_T("OpenProcess()执行失败。"));
		return FALSE;
	}
	TCHAR szExePath[MAX_PATH] = { 0 };
	DWORD dwLength = MAX_PATH;
	QueryFullProcessImageName(hProcess, 0, szExePath, &dwLength);
	if (dwLength == 0)
	{
		TCHAR szError[MAX_PATH] = { 0 };
		_stprintf_s(szError, MAX_PATH, TEXT("error code=0x%x"), GetLastError());
		LOG(szError);
		return FALSE;
	}
	DWORD dwSize = 0;
	DWORD dwHandle = 0;
	dwSize = GetFileVersionInfoSize(szExePath, &dwHandle);
	if (dwSize == 0)
	{
		LOG(_T("GetFileVersionInfoSize failed."));
		return FALSE;
	}
	LPVOID lpBuffer = new BYTE[dwSize];
	ZeroMemory(lpBuffer, dwSize);
	if (!GetFileVersionInfo(szExePath, dwHandle, dwSize, lpBuffer))
	{
		delete[]lpBuffer;
		LOG(_T("GetFileVersionInfo failed."));
		return FALSE;
	}
	dwLength = 0;
	// 获取语言和字符集代码
	LPVOID lpLangCode = nullptr;
	if (VerQueryValue(lpBuffer, TEXT("\\VarFileInfo\\Translation"), &lpLangCode, (PUINT)&dwLength))
	{
		// 构建产品名称查询字符串
		TCHAR szQuery[MAX_PATH] = { 0 };
		_stprintf_s(szQuery, MAX_PATH, TEXT("\\StringFileInfo\\%.4x%.4x\\ProductName"), LOWORD((DWORD)(*(LPDWORD)lpLangCode)), HIWORD((DWORD)(*(LPDWORD)lpLangCode)));
		// 获取产品名称
		LPVOID lpProductName = nullptr;
		if (VerQueryValue(lpBuffer, szQuery, &lpProductName, (PUINT)&dwLength))
		{
			strDisplayName = (LPTSTR)lpProductName;
			delete[]lpBuffer;
			return TRUE;
		}
		// 构建文件描述查询字符串
		_stprintf_s(szQuery, MAX_PATH, TEXT("\\StringFileInfo\\%.4x%.4x\\FileDescription"), LOWORD((DWORD)(*(LPDWORD)lpLangCode)), HIWORD((DWORD)(*(LPDWORD)lpLangCode)));
		// 获取文件描述
		LPVOID lpFileDescription = nullptr;
		if (VerQueryValue(lpBuffer, szQuery, &lpFileDescription, (PUINT)&dwLength))
		{
			strDisplayName = (LPTSTR)lpFileDescription;
			delete[]lpBuffer;
			return TRUE;
		}
		delete[]lpBuffer;
	}
	CString strExePath = szExePath;
	DWORD dwLeft = strExePath.ReverseFind(TEXT('\\'));
	DWORD dwRight = strExePath.ReverseFind(TEXT('.'));
	strExePath = strExePath.Mid(dwLeft, (dwRight - dwLeft));
	strDisplayName = strExePath;

	return TRUE;
}


// 获取回放设备音量
FLOAT AudioManager::GetPlaybackDeviceVolume(DWORD nIndex)
{
	FLOAT fVolume = 0.00f;
	AUDIO_CONTROL_DEVICE_ENTITY entity = this->GetPlaybackDevice(nIndex);
	CComPtr<IMMDevice> spDevice = entity.spObject;
	if (!spDevice)
	{
		LOG(_T("没有找到指定的设备。"));
		return fVolume;
	}

	// 获取IAudioEndpointVolume接口
	CComPtr<IAudioEndpointVolume> spVolume;
	HRESULT hr = spDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, reinterpret_cast<void**>(&spVolume));
	if (FAILED(hr))
	{
		LOG(_T("获取IAudioEndpointVolume接口失败。"));
		return fVolume;
	}

	// 获取音量
	spVolume->GetMasterVolumeLevelScalar(&fVolume);

	return fVolume;
}


// 设置回放设备音量
void AudioManager::SetPlaybackDeviceVolume(DWORD nIndex, FLOAT fVolume)
{
	AUDIO_CONTROL_DEVICE_ENTITY entity = this->GetPlaybackDevice(nIndex);
	CComPtr<IMMDevice> spDevice = entity.spObject;
	if (!spDevice)
	{
		LOG(_T("没有找到指定的设备。"));
		return;
	}

	// 获取IAudioEndpointVolume接口
	CComPtr<IAudioEndpointVolume> spVolume;
	HRESULT hr = spDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, reinterpret_cast<void**>(&spVolume));
	if (FAILED(hr))
	{
		LOG(_T("获取IAudioEndpointVolume接口失败。"));
		return;
	}

	// 设置音量
	spVolume->SetMasterVolumeLevelScalar(fVolume, nullptr);
}


// 静音回放设备
void AudioManager::SetPlaybackDeviceMute(DWORD nIndex, BOOL bMute)
{
	DWORD count = this->GetPlaybackDeviceCount();
	if (nIndex >= count)
	{
		return;
	}
	AUDIO_CONTROL_DEVICE_ENTITY entity = this->GetPlaybackDevice(nIndex);
	CComPtr<IMMDevice> spDevice = entity.spObject;
	if (!spDevice)
	{
		return;
	}
	CComPtr<IAudioEndpointVolume> spVolume;
	HRESULT hr = spDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, reinterpret_cast<void**>(&spVolume));
	if (FAILED(hr))
	{
		LOG(_T("获取IAudioEndpointVolume接口失败。"));
		return;
	}
	spVolume->SetMute(bMute, nullptr);
}


// 获取回放设备静音状态
BOOL AudioManager::GetPlaybackDeviceMute(DWORD nIndex)
{
	DWORD count = this->GetPlaybackDeviceCount();
	if (nIndex >= count)
	{
		return FALSE;
	}
	AUDIO_CONTROL_DEVICE_ENTITY entity = this->GetPlaybackDevice(nIndex);
	CComPtr<IMMDevice> spDevice = entity.spObject;
	if (!spDevice)
	{
		return FALSE;
	}
	CComPtr<IAudioEndpointVolume> spVolume;
	HRESULT hr = spDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, reinterpret_cast<void**>(&spVolume));
	if (FAILED(hr))
	{
		LOG(_T("获取IAudioEndpointVolume接口失败。"));
		return FALSE;
	}
	BOOL bMute;
	spVolume->GetMute(&bMute);
	return bMute;
}


// 设置默认播放设备
void AudioManager::SetDefaultPlaybackDevice(DWORD nIndex)
{
	AUDIO_CONTROL_DEVICE_ENTITY entity = this->GetPlaybackDevice(nIndex);
	this->SetDefaultDevice(entity.strId, eConsole);
}


// 获取默认播放设备
DWORD AudioManager::GetDefaultPlaybackDevice()
{
	return this->GetDefaultDevice(eRender, eMultimedia);
}
// 设置默认录音设备
void AudioManager::SetDefaultRecordingDevice(DWORD nIndex)
{
	AUDIO_CONTROL_DEVICE_ENTITY entity = this->GetRecordingDevice(nIndex);
	this->SetDefaultDevice(entity.strId, eCommunications);
}


// 获取默认录音设备
DWORD AudioManager::GetDefaultRecordingDevice()
{
	return this->GetDefaultDevice(eCapture, eCommunications);
}


// 获取录音设备音量
FLOAT AudioManager::GetRecordingDeviceVolume(DWORD nIndex)
{
	FLOAT fVolume = 0.00f;
	AUDIO_CONTROL_DEVICE_ENTITY entity = this->GetRecordingDevice(nIndex);
	CComPtr<IMMDevice> spDevice = entity.spObject;
	if (!spDevice)
	{
		LOG(_T("没有找到指定的设备。"));
		return fVolume;
	}

	// 获取IAudioEndpointVolume接口
	CComPtr<IAudioEndpointVolume> spVolume;
	HRESULT hr = spDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, reinterpret_cast<void**>(&spVolume));
	if (FAILED(hr))
	{
		LOG(_T("获取IAudioEndpointVolume接口失败。"));
		return fVolume;
	}

	// 获取音量
	spVolume->GetMasterVolumeLevelScalar(&fVolume);

	return fVolume;
}


// 设置录音设备音量
void AudioManager::SetRecordingDeviceVolume(DWORD nIndex, FLOAT fVolume)
{
	AUDIO_CONTROL_DEVICE_ENTITY entity = this->GetRecordingDevice(nIndex);
	CComPtr<IMMDevice> spDevice = entity.spObject;
	if (!spDevice)
	{
		LOG(_T("没有找到指定的设备。"));
		return;
	}

	// 获取IAudioEndpointVolume接口
	CComPtr<IAudioEndpointVolume> spVolume;
	HRESULT hr = spDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, reinterpret_cast<void**>(&spVolume));
	if (FAILED(hr))
	{
		LOG(_T("获取IAudioEndpointVolume接口失败。"));
		return;
	}

	// 设置音量
	spVolume->SetMasterVolumeLevelScalar(fVolume, nullptr);
}


// 静音录音设备
void AudioManager::SetRecordingDeviceMute(DWORD nIndex, BOOL bMute)
{
	DWORD count = this->GetRecordingDeviceCount();
	if (nIndex >= count)
	{
		return;
	}
	AUDIO_CONTROL_DEVICE_ENTITY entity = this->GetRecordingDevice(nIndex);
	CComPtr<IMMDevice> spDevice = entity.spObject;
	if (!spDevice)
	{
		return;
	}
	CComPtr<IAudioEndpointVolume> spVolume;
	HRESULT hr = spDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, reinterpret_cast<void**>(&spVolume));
	if (FAILED(hr))
	{
		LOG(_T("获取IAudioEndpointVolume接口失败。"));
		return;
	}
	spVolume->SetMute(bMute, nullptr);
}


// 获取录音设备静音状态
BOOL AudioManager::GetRecordingDeviceMute(DWORD nIndex)
{
	DWORD count = this->GetRecordingDeviceCount();
	if (nIndex >= count)
	{
		return FALSE;
	}
	AUDIO_CONTROL_DEVICE_ENTITY entity = this->GetRecordingDevice(nIndex);
	CComPtr<IMMDevice> spDevice = entity.spObject;
	if (!spDevice)
	{
		return FALSE;
	}
	CComPtr<IAudioEndpointVolume> spVolume;
	HRESULT hr = spDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, reinterpret_cast<void**>(&spVolume));
	if (FAILED(hr))
	{
		LOG(_T("获取IAudioEndpointVolume接口失败。"));
		return FALSE;
	}
	BOOL bMute;
	spVolume->GetMute(&bMute);
	return bMute;
}


// 获取会话音量
FLOAT AudioManager::GetSessionVolume(DWORD nIndex)
{
	FLOAT fVolume = 0.00f;
	AUDIO_CONTROL_SESSION_ENTITY entity = this->GetSession(nIndex);
	CComPtr<IAudioSessionControl> spSession = entity.spObject;
	if (!spSession)
	{
		LOG(_T("没有找到指定的设备。"));
		return fVolume;
	}

	// 获取ISimpleAudioVolume接口
	CComPtr<ISimpleAudioVolume> spVolume;
	HRESULT hr = spSession->QueryInterface(__uuidof(ISimpleAudioVolume), reinterpret_cast<void**>(&spVolume));
	if (FAILED(hr))
	{
		LOG(_T("获取ISimpleAudioVolume接口失败。"));
		return fVolume;
	}

	// 获取音量
	spVolume->GetMasterVolume(&fVolume);

	return fVolume;
}


// 设置会话音量
void AudioManager::SetSessionVolume(DWORD nIndex, FLOAT fVolume)
{
	DWORD count = this->GetPlaybackDeviceCount();
	if (nIndex >= count)
	{
		LOG(_T("Parameter error."));
		return;
	}
	AUDIO_CONTROL_SESSION_ENTITY entity = this->GetSession(nIndex);
	CComPtr<IAudioSessionControl> spSession = entity.spObject;
	if (!spSession)
	{
		LOG(_T("The session is not exists."));
		return;
	}
	// 获取当前需要设置会话音量的进程ID
	DWORD dwProcessId = 0;
	CComPtr<IAudioSessionControl2> spSession2;
	HRESULT hr = spSession->QueryInterface(__uuidof(IAudioSessionControl2), reinterpret_cast<void**>(&spSession2));
	if (FAILED(hr))
	{
		LOG(_T("Get IAudioSessionControl2 failed."));
		return;
	}
	hr = spSession2->GetProcessId(&dwProcessId);
	if (FAILED(hr))
	{
		LOG(_T("Get the process ID of the session is failed."));
		return;
	}
	// 枚举所有会话，设置所有指定进程ID会话的音量
	spSession = nullptr;
	spSession2 = nullptr;
	CComPtr<IMMDeviceEnumerator> spDeviceEnumerator;
	hr = spDeviceEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
	if (FAILED(hr))
	{
		LOG(_T("Get IMMDeviceEnumerator failed."));
		return;
	}
	CComPtr<IMMDeviceCollection> spDeviceCollection;
	hr = spDeviceEnumerator->EnumAudioEndpoints(eRender, eMultimedia, &spDeviceCollection);
	if (FAILED(hr))
	{
		LOG(_T("Get IMMDeviceCollection failed."));
		return;
	}
	UINT uDeviceCount = 0;
	hr = spDeviceCollection->GetCount(&uDeviceCount);
	if (FAILED(hr))
	{
		LOG(_T("Get device count failed."));
		return;
	}
	for (UINT i = 0; i < uDeviceCount; i++)
	{
		CComPtr<IMMDevice> spDevice;
		hr = spDeviceCollection->Item(i, &spDevice);
		if (FAILED(hr))
		{
			LOG(_T("Get device failed."));
			continue;
		}
		CComPtr<IAudioSessionManager2> spSessionManager2;
		hr = spDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_INPROC_SERVER, nullptr, reinterpret_cast<void**>(&spSessionManager2));
		if (FAILED(hr))
		{
			LOG(_T("Get IAudioSessionManager2 failed."));
			continue;
		}
		CComPtr<IAudioSessionEnumerator> spSessionEnumerator;
		hr = spSessionManager2->GetSessionEnumerator(&spSessionEnumerator);
		if (FAILED(hr))
		{
			LOG(_T("Get IAudioSessionEnumerator failed."));
			continue;
		}
		int nSessionCount = 0;
		hr = spSessionEnumerator->GetCount(&nSessionCount);
		if (FAILED(hr))
		{
			LOG(_T("Get session count failed."));
			continue;
		}
		for (int j = 0; j < nSessionCount; j++)
		{
			CComPtr<IAudioSessionControl> spSession;
			hr = spSessionEnumerator->GetSession(j, &spSession);
			if (FAILED(hr))
			{
				LOG(_T("Get session failed."));
				continue;
			}
			CComPtr<IAudioSessionControl2> spSession2;
			hr = spSession->QueryInterface(__uuidof(IAudioSessionControl2), reinterpret_cast<void**>(&spSession2));
			if (FAILED(hr))
			{
				LOG(_T("Get IAudioSessionControl2 failed."));
				continue;
			}
			AudioSessionState state;
			hr = spSession->GetState(&state);
			if (FAILED(hr))
			{
				LOG(_T("Get session state failed."));
				continue;
			}
			if (state != AudioSessionStateActive && state != AudioSessionStateInactive)
			{
				LOG(_T("The session is expired."));
				continue;
			}
			DWORD dwSessionProcessId = 0;
			hr = spSession2->GetProcessId(&dwSessionProcessId);
			if (FAILED(hr))
			{
				LOG(_T("Get session process ID failed."));
				continue;
			}
			if (dwProcessId != dwSessionProcessId)
			{
				continue;
			}
			// 获取ISimpleAudioVolume接口
			CComPtr<ISimpleAudioVolume> spVolume;
			hr = spSession->QueryInterface(__uuidof(ISimpleAudioVolume), reinterpret_cast<void**>(&spVolume));
			if (FAILED(hr))
			{
				LOG(_T("Get ISimpleAudioVolume failed."));
				continue;
			}
			// 设置音量
			hr = spVolume->SetMasterVolume(fVolume, nullptr);
			if (FAILED(hr))
			{
				LOG(_T("Set the session volume failed."));
				continue;
			}
		}
	}
}

// 静音会话
void AudioManager::SetSessionMute(DWORD nIndex, BOOL bMute)
{
	DWORD count = this->GetSessionCount();
	if (nIndex >= count)
	{
		LOG(_T("Parameters are invalid."));
		return;
	}
	AUDIO_CONTROL_SESSION_ENTITY entity = this->GetSession(nIndex);
	CComPtr<IAudioSessionControl> spSession = entity.spObject;
	if (!spSession)
	{
		LOG(_T("The session is not exists."));
		return;
	}
	// 获取当前需要静音会话的进程ID
	DWORD dwProcessId = 0;
	CComPtr<IAudioSessionControl2> spSession2;
	HRESULT hr = spSession->QueryInterface(__uuidof(IAudioSessionControl2), reinterpret_cast<void**>(&spSession2));
	if (FAILED(hr))
	{
		LOG(_T("Get IAudioSessionControl2 failed."));
		return;
	}
	hr = spSession2->GetProcessId(&dwProcessId);
	if (FAILED(hr))
	{
		LOG(_T("Get the process ID of the session is failed."));
		return;
	}
	// 枚举所有会话，静音所有指定进程ID的会话
	spSession = nullptr;
	spSession2 = nullptr;
	CComPtr<IMMDeviceEnumerator> spDeviceEnumerator;
	hr = spDeviceEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
	if (FAILED(hr))
	{
		LOG(_T("Get IMMDeviceEnumerator failed."));
		return;
	}
	CComPtr<IMMDeviceCollection> spDeviceCollection;
	hr = spDeviceEnumerator->EnumAudioEndpoints(eRender, eMultimedia, &spDeviceCollection);
	if (FAILED(hr))
	{
		LOG(_T("Get IMMDeviceCollection failed."));
		return;
	}
	UINT uDeviceCount = 0;
	hr = spDeviceCollection->GetCount(&uDeviceCount);
	if (FAILED(hr))
	{
		LOG(_T("Get device count failed."));
		return;
	}
	for (UINT i = 0; i < uDeviceCount; i++)
	{
		CComPtr<IMMDevice> spDevice;
		hr = spDeviceCollection->Item(i, &spDevice);
		if (FAILED(hr))
		{
			LOG(_T("Get device failed."));
			continue;
		}
		CComPtr<IAudioSessionManager2> spSessionManager2;
		hr = spDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_INPROC_SERVER, nullptr, reinterpret_cast<void**>(&spSessionManager2));
		if (FAILED(hr))
		{
			LOG(_T("Get IAudioSessionManager2 failed."));
			continue;
		}
		CComPtr<IAudioSessionEnumerator> spSessionEnumerator;
		hr = spSessionManager2->GetSessionEnumerator(&spSessionEnumerator);
		if (FAILED(hr))
		{
			LOG(_T("Get IAudioSessionEnumerator failed."));
			continue;
		}
		int nSessionCount = 0;
		hr = spSessionEnumerator->GetCount(&nSessionCount);
		if (FAILED(hr))
		{
			LOG(_T("Get session count failed."));
			continue;
		}
		for (int j = 0; j < nSessionCount; j++)
		{
			CComPtr<IAudioSessionControl> spSession;
			hr = spSessionEnumerator->GetSession(j, &spSession);
			if (FAILED(hr))
			{
				LOG(_T("Get session failed."));
				continue;
			}
			CComPtr<IAudioSessionControl2> spSession2;
			hr = spSession->QueryInterface(__uuidof(IAudioSessionControl2), reinterpret_cast<void**>(&spSession2));
			if (FAILED(hr))
			{
				LOG(_T("Get IAudioSessionControl2 failed."));
				continue;
			}
			AudioSessionState state;
			hr = spSession->GetState(&state);
			if (FAILED(hr))
			{
				LOG(_T("Get session state failed."));
				continue;
			}
			if (state != AudioSessionStateActive && state != AudioSessionStateInactive)
			{
				LOG(_T("The session is expired."));
				continue;
			}
			DWORD dwSessionProcessId = 0;
			hr = spSession2->GetProcessId(&dwSessionProcessId);
			if (FAILED(hr))
			{
				LOG(_T("Get session process ID failed."));
				continue;
			}
			if (dwProcessId != dwSessionProcessId)
			{
				continue;
			}
			// 获取ISimpleAudioVolume接口
			CComPtr<ISimpleAudioVolume> spVolume;
			hr = spSession->QueryInterface(__uuidof(ISimpleAudioVolume), reinterpret_cast<void**>(&spVolume));
			if (FAILED(hr))
			{
				LOG(_T("Get ISimpleAudioVolume failed."));
				continue;
			}
			// 静音会话
			hr = spVolume->SetMute(bMute, nullptr);
			if (FAILED(hr))
			{
				LOG(_T("Mute the session failed."));
				continue;
			}
		}
	}
}


// 获取会话静音状态
BOOL AudioManager::GetSessionMute(DWORD nIndex)
{
	DWORD count = this->GetSessionCount();
	if (nIndex >= count)
	{
		LOG(_T("Parameters are invalid."));
		return FALSE;
	}
	AUDIO_CONTROL_SESSION_ENTITY entity = this->GetSession(nIndex);
	CComPtr<IAudioSessionControl> spSession = entity.spObject;
	if (!spSession)
	{
		LOG(_T("Invalid session"));
		return FALSE;
	}

	// 获取ISimpleAudioVolume接口
	CComPtr<ISimpleAudioVolume> spVolume;
	HRESULT hr = spSession->QueryInterface(__uuidof(ISimpleAudioVolume), reinterpret_cast<void**>(&spVolume));
	if (FAILED(hr))
	{
		LOG(_T("Get ISimpleAudioVolume failed."));
		return FALSE;
	}

	// 获取会话静音状态
	BOOL bMute;
	hr = spVolume->GetMute(&bMute);
	if (FAILED(hr))
	{
		LOG(_T("Get mute state failed."));
		return FALSE;
	}
	return bMute;
}
