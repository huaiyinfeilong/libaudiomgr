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
#include <windows.foundation.h>
#include <wrl/client.h>
#include "AudioPolicyConfig.h"


//#define LIBAUDIOMGR_DEBUG

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


// ��ʼ��
void AudioManager::Initialize()
{
	this->Uninitialize();
	CoInitializeEx(NULL, NULL);
	this->GetDevices(eRender, eMultimedia, this->_listPlaybackDevice);
	this->GetDevices(eCapture, eCommunications, this->_listRecordingDevice);
	this->GetAllSession();
}

// ���ʼ��
void AudioManager::Uninitialize()
{
	this->_listPlaybackDevice.clear();
	this->_listRecordingDevice.clear();
	this->_listSession.clear();
	// ��һЩ�����У��������CoUninitialize�����������쳣
	// CoUninitialize();
}


// ��ȡ�ط��豸����
DWORD AudioManager::GetPlaybackDeviceCount()
{
	return (DWORD)this->_listPlaybackDevice.size();
}


// ��ȡ�ط��豸
AUDIO_CONTROL_DEVICE_ENTITY AudioManager::GetPlaybackDevice(DWORD nIndex)
{
	return this->_listPlaybackDevice[nIndex];
}


// ��ȡ¼���豸����
DWORD AudioManager::GetRecordingDeviceCount()
{
	return (DWORD)this->_listRecordingDevice.size();
}


// ��ȡ¼���豸
AUDIO_CONTROL_DEVICE_ENTITY AudioManager::GetRecordingDevice(DWORD nIndex)
{
	return this->_listRecordingDevice[nIndex];
}


// ��ȡ�Ự����
DWORD AudioManager::GetSessionCount()
{
	return (DWORD)_listSession.size();
}


// ��ȡ�Ự
AUDIO_CONTROL_SESSION_ENTITY AudioManager::GetSession(DWORD nIndex) const
{
	return _listSession[nIndex];
}


// �������ͻ�ȡ�豸�б�
void AudioManager::GetDevices(EDataFlow dataFlow, ERole eRole, std::vector<AUDIO_CONTROL_DEVICE_ENTITY>& listDevice)
{
	listDevice.clear();
	CComPtr<IMMDeviceEnumerator> spEnumerator;
	HRESULT hr = spEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
	if (FAILED(hr))
	{
		LOG(_T("��ȡIMMDeviceEnumerator�ӿ�ʧ�ܡ�"));
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
		LOG(_T("��ȡ��0���豸��"));
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
			LOG(_T("��ȡIPropertyStore�ӿ�ʧ�ܡ�"));
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
			LOG(_T("PropertyVariant��vt����VT_LPWSTR"));
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


// ����Ĭ�ϲ��š�¼���豸
void AudioManager::SetDefaultDevice(LPCTSTR lpDeviceId, ERole eRole)
{
	CComPtr<IPolicyConfig> spConfig;
	HRESULT hr = spConfig.CoCreateInstance(__uuidof(CPolicyConfigClient));
	if (FAILED(hr))
	{
		CString strError = _T("");
		strError.Format(_T("��ȡIpolicyConfig�ӿ�ʧ�ܣ� hr=0x%x, error code=0x%x"), hr, GetLastError());
		LOG((LPCTSTR)strError);
		return;
	}
	spConfig->SetDefaultEndpoint(lpDeviceId, eRole);
}


// ��ȡĬ�ϲ��š�¼���豸
DWORD AudioManager::GetDefaultDevice(EDataFlow eDataFlow, ERole eRole)
{
	// ��ȡIMMDeviceEnumerator�ӿ�
	CComPtr<IMMDeviceEnumerator> spEnumerator;
	HRESULT hr = spEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
	if (FAILED(hr))
	{
		LOG(_T("��ȡIMMDeviceEnumerator�ӿ�ʧ�ܡ�"));
		return -1;
	}
	// ��ȡĬ���豸
	CComPtr<IMMDevice> spDevice;
	hr = spEnumerator->GetDefaultAudioEndpoint(eDataFlow, eRole, &spDevice);
	if (FAILED(hr))
	{
		LOG(_T("��ȡIMMDevice�ӿ�ʧ�ܡ�"));
		return -1;
	}
	// ��ȡ�豸ID
	LPWSTR lpDeviceId = nullptr;
	hr = spDevice->GetId(&lpDeviceId);
	if (FAILED(hr))
	{
		LOG(_T("��ȡ�豸IDʧ�ܡ�"));
		return -1;
	}
	// �ҳ�Ĭ���豸���б��е������±겢����
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


// ���ûỰ���š�¼���豸
void AudioManager::SetSessionDevice(DWORD dwIndex, CComPtr<IMMDevice>& spDevice)
{
	AUDIO_CONTROL_SESSION_ENTITY entity = this->GetSession(dwIndex);
	CComPtr<IAudioSessionControl>& spSession = entity.spObject;
	if (!spSession)
	{
		LOG(_T("IAudioSessionControl����Ϊ�ա�"));
		return;
	}
	CComPtr<IAudioSessionControl2> spSession2;
	HRESULT hr = spSession->QueryInterface(__uuidof(IAudioSessionControl2), reinterpret_cast<void**>(&spSession2));
	if (FAILED(hr))
	{
		LOG(_T("��ȡIAudioSessionControl2�ӿ�ʧ�ܡ�"));
		return;
	}
	CComPtr<IAudioSessionManager> spManager;
	hr = spSession2->QueryInterface(__uuidof(IAudioSessionManager), reinterpret_cast<void**>(&spManager));
	if (FAILED(hr))
	{
		LOG(_T("��ȡIAudioSessionManager�ӿ�ʧ�ܡ�"));
		return;
	}
	CComPtr<IAudioSessionManager2> spManager2;
	hr = spManager->QueryInterface(__uuidof(IAudioSessionManager2), reinterpret_cast<void**>(&spManager2));
	if (FAILED(hr))
	{
		LOG(_T("��ȡIAudioSessionManager2�ӿ�ʧ�ܡ�"));
		return;
	}

}


// ��ȡ���лỰ
void AudioManager::GetAllSession()
{
	// ��ŻỰ���̵��Ự�����map���Թ���ͬ���Ҳ��ڵ�ǰĬ���豸�ĻỰ
	std::map<DWORD, AUDIO_CONTROL_SESSION_ENTITY> mapSession;
	// ��ȡ����session
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
			// �����ͬ���̵ĻỰ�Ѿ�����������
			if (mapSession.find(dwProcessId) != mapSession.end())
			{
				continue;
			}
			AUDIO_CONTROL_SESSION_ENTITY entity;
			if (spSession2->IsSystemSoundsSession() != S_FALSE)
			{
				entity.strName = _T("ϵͳ����");
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
			entity.dwProcessId = dwProcessId;
			entity.spObject = spSession;
			mapSession[dwProcessId] = entity;
		}
	}
	for (auto it = mapSession.begin(); it != mapSession.end(); it++)
	{
		this->_listSession.push_back(it->second);
	}
}


// ����������Ƶ�Ự������豸ID
CString AudioManager::GenerateDeviceId(EDataFlow dataFlow, CString strDeviceId)
{
	static LPCWSTR DEVINTERFACE_AUDIO_RENDER = L"#{e6327cad-dcec-4949-ae8a-991e976a79d2}";
	static LPCWSTR DEVINTERFACE_AUDIO_CAPTURE = L"#{2eef81be-33fa-4800-9670-1cd474972c3f}";
	static LPCWSTR MMDEVAPI_TOKEN = L"\\\\?\\SWD#MMDEVAPI#";
	CString strFullDeviceId = MMDEVAPI_TOKEN;
	strFullDeviceId += strDeviceId;
	if (dataFlow == eRender)
	{
		strFullDeviceId += DEVINTERFACE_AUDIO_RENDER;
	}
	else if (dataFlow == eCapture)
	{
		strFullDeviceId += DEVINTERFACE_AUDIO_CAPTURE;
	}
	else
	{
		return CString(_T(""));
	}
	return strFullDeviceId;
}


// ��ȡ�Ự����
BOOL AudioManager::GetSessionDisplayName(CComPtr<IAudioSessionControl2>& spSession, CString& strDisplayName)
{
	LPTSTR lpDisplayName = nullptr;
	HRESULT hr = spSession->GetDisplayName(&lpDisplayName);
	if (FAILED(hr))
	{
		LOG(_T("IAudioSessionController2::GetDisplayName failed."));
	}
	// �ɹ���ȡ������
	if (*lpDisplayName)
	{
		LOG(lpDisplayName);
		strDisplayName = lpDisplayName;
		CoTaskMemFree(lpDisplayName);
		return TRUE;
	}
	// ��ȡ����ʧ�ܣ����Ի�ȡexe�ļ������ֶ�
	// ����ͨ������ID��ȡ��������·��
	// Ȼ��ͨ����������·���õ��ļ���Ϣ�е��ļ������ֶ�����
	CoTaskMemFree(lpDisplayName);
	// ��ȡ����ID
	DWORD dwProcessId = 0;
	hr = spSession->GetProcessId(&dwProcessId);
	if (FAILED(hr))
	{
		LOG(_T("Get process ID of the session failed."));
		return FALSE;
	}
	// ͨ������ID�õ�����·��
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
	if (hProcess == NULL)
	{
		LOG(_T("OpenProcess()ִ��ʧ�ܡ�"));
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
	if (dwSize != 0)
	{
		LPVOID lpBuffer = new BYTE[dwSize];
		ZeroMemory(lpBuffer, dwSize);
		if (!GetFileVersionInfo(szExePath, dwHandle, dwSize, lpBuffer))
		{
			delete[]lpBuffer;
			LOG(_T("GetFileVersionInfo failed."));
			return FALSE;
		}
		dwLength = 0;
		// ��ȡ���Ժ��ַ�������
		LPVOID lpLangCode = nullptr;
		if (VerQueryValue(lpBuffer, TEXT("\\VarFileInfo\\Translation"), &lpLangCode, (PUINT)&dwLength))
		{
			// ������Ʒ���Ʋ�ѯ�ַ���
			TCHAR szQuery[MAX_PATH] = { 0 };
			_stprintf_s(szQuery, MAX_PATH, TEXT("\\StringFileInfo\\%.4x%.4x\\ProductName"), LOWORD((DWORD)(*(LPDWORD)lpLangCode)), HIWORD((DWORD)(*(LPDWORD)lpLangCode)));
			// ��ȡ��Ʒ����
			LPVOID lpProductName = nullptr;
			if (VerQueryValue(lpBuffer, szQuery, &lpProductName, (PUINT)&dwLength))
			{
				strDisplayName = (LPTSTR)lpProductName;
				delete[]lpBuffer;
				return TRUE;
			}
			// �����ļ�������ѯ�ַ���
			_stprintf_s(szQuery, MAX_PATH, TEXT("\\StringFileInfo\\%.4x%.4x\\FileDescription"), LOWORD((DWORD)(*(LPDWORD)lpLangCode)), HIWORD((DWORD)(*(LPDWORD)lpLangCode)));
			// ��ȡ�ļ�����
			LPVOID lpFileDescription = nullptr;
			if (VerQueryValue(lpBuffer, szQuery, &lpFileDescription, (PUINT)&dwLength))
			{
				strDisplayName = (LPTSTR)lpFileDescription;
				delete[]lpBuffer;
				return TRUE;
			}
			delete[]lpBuffer;
		}
	}
	CString strExePath = szExePath;
	DWORD dwLeft = strExePath.ReverseFind(TEXT('\\'));
	DWORD dwRight = strExePath.ReverseFind(TEXT('.'));
	strExePath = strExePath.Mid(dwLeft, (dwRight - dwLeft));
	strDisplayName = strExePath;

	return TRUE;
}


// ��ȡ�ط��豸����
FLOAT AudioManager::GetPlaybackDeviceVolume(DWORD nIndex)
{
	FLOAT fVolume = 0.00f;
	AUDIO_CONTROL_DEVICE_ENTITY entity = this->GetPlaybackDevice(nIndex);
	CComPtr<IMMDevice> spDevice = entity.spObject;
	if (!spDevice)
	{
		LOG(_T("û���ҵ�ָ�����豸��"));
		return fVolume;
	}

	// ��ȡIAudioEndpointVolume�ӿ�
	CComPtr<IAudioEndpointVolume> spVolume;
	HRESULT hr = spDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, reinterpret_cast<void**>(&spVolume));
	if (FAILED(hr))
	{
		LOG(_T("��ȡIAudioEndpointVolume�ӿ�ʧ�ܡ�"));
		return fVolume;
	}

	// ��ȡ����
	spVolume->GetMasterVolumeLevelScalar(&fVolume);

	return fVolume;
}


// ���ûط��豸����
void AudioManager::SetPlaybackDeviceVolume(DWORD nIndex, FLOAT fVolume)
{
	AUDIO_CONTROL_DEVICE_ENTITY entity = this->GetPlaybackDevice(nIndex);
	CComPtr<IMMDevice> spDevice = entity.spObject;
	if (!spDevice)
	{
		LOG(_T("û���ҵ�ָ�����豸��"));
		return;
	}

	// ��ȡIAudioEndpointVolume�ӿ�
	CComPtr<IAudioEndpointVolume> spVolume;
	HRESULT hr = spDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, reinterpret_cast<void**>(&spVolume));
	if (FAILED(hr))
	{
		LOG(_T("��ȡIAudioEndpointVolume�ӿ�ʧ�ܡ�"));
		return;
	}

	// ��������
	spVolume->SetMasterVolumeLevelScalar(fVolume, nullptr);
}


// �����ط��豸
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
		LOG(_T("��ȡIAudioEndpointVolume�ӿ�ʧ�ܡ�"));
		return;
	}
	spVolume->SetMute(bMute, nullptr);
}


// ��ȡ�ط��豸����״̬
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
		LOG(_T("��ȡIAudioEndpointVolume�ӿ�ʧ�ܡ�"));
		return FALSE;
	}
	BOOL bMute;
	spVolume->GetMute(&bMute);
	return bMute;
}


// ����Ĭ�ϲ����豸
void AudioManager::SetDefaultPlaybackDevice(DWORD nIndex)
{
	AUDIO_CONTROL_DEVICE_ENTITY entity = this->GetPlaybackDevice(nIndex);
	this->SetDefaultDevice(entity.strId, eConsole);
}


// ��ȡĬ�ϲ����豸
DWORD AudioManager::GetDefaultPlaybackDevice()
{
	return this->GetDefaultDevice(eRender, eMultimedia);
}
// ����Ĭ��¼���豸
void AudioManager::SetDefaultRecordingDevice(DWORD nIndex)
{
	AUDIO_CONTROL_DEVICE_ENTITY entity = this->GetRecordingDevice(nIndex);
	this->SetDefaultDevice(entity.strId, eCommunications);
}


// ��ȡĬ��¼���豸
DWORD AudioManager::GetDefaultRecordingDevice()
{
	return this->GetDefaultDevice(eCapture, eCommunications);
}


// ��ȡ¼���豸����
FLOAT AudioManager::GetRecordingDeviceVolume(DWORD nIndex)
{
	FLOAT fVolume = 0.00f;
	AUDIO_CONTROL_DEVICE_ENTITY entity = this->GetRecordingDevice(nIndex);
	CComPtr<IMMDevice> spDevice = entity.spObject;
	if (!spDevice)
	{
		LOG(_T("û���ҵ�ָ�����豸��"));
		return fVolume;
	}

	// ��ȡIAudioEndpointVolume�ӿ�
	CComPtr<IAudioEndpointVolume> spVolume;
	HRESULT hr = spDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, reinterpret_cast<void**>(&spVolume));
	if (FAILED(hr))
	{
		LOG(_T("��ȡIAudioEndpointVolume�ӿ�ʧ�ܡ�"));
		return fVolume;
	}

	// ��ȡ����
	spVolume->GetMasterVolumeLevelScalar(&fVolume);

	return fVolume;
}


// ����¼���豸����
void AudioManager::SetRecordingDeviceVolume(DWORD nIndex, FLOAT fVolume)
{
	AUDIO_CONTROL_DEVICE_ENTITY entity = this->GetRecordingDevice(nIndex);
	CComPtr<IMMDevice> spDevice = entity.spObject;
	if (!spDevice)
	{
		LOG(_T("û���ҵ�ָ�����豸��"));
		return;
	}

	// ��ȡIAudioEndpointVolume�ӿ�
	CComPtr<IAudioEndpointVolume> spVolume;
	HRESULT hr = spDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, reinterpret_cast<void**>(&spVolume));
	if (FAILED(hr))
	{
		LOG(_T("��ȡIAudioEndpointVolume�ӿ�ʧ�ܡ�"));
		return;
	}

	// ��������
	spVolume->SetMasterVolumeLevelScalar(fVolume, nullptr);
}


// ����¼���豸
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
		LOG(_T("��ȡIAudioEndpointVolume�ӿ�ʧ�ܡ�"));
		return;
	}
	spVolume->SetMute(bMute, nullptr);
}


// ��ȡ¼���豸����״̬
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
		LOG(_T("��ȡIAudioEndpointVolume�ӿ�ʧ�ܡ�"));
		return FALSE;
	}
	BOOL bMute;
	spVolume->GetMute(&bMute);
	return bMute;
}


// ��ȡ�Ự����
FLOAT AudioManager::GetSessionVolume(DWORD nIndex)
{
	FLOAT fVolume = 0.00f;
	AUDIO_CONTROL_SESSION_ENTITY entity = this->GetSession(nIndex);
	CComPtr<IAudioSessionControl> spSession = entity.spObject;
	if (!spSession)
	{
		LOG(_T("û���ҵ�ָ�����豸��"));
		return fVolume;
	}

	// ��ȡISimpleAudioVolume�ӿ�
	CComPtr<ISimpleAudioVolume> spVolume;
	HRESULT hr = spSession->QueryInterface(__uuidof(ISimpleAudioVolume), reinterpret_cast<void**>(&spVolume));
	if (FAILED(hr))
	{
		LOG(_T("��ȡISimpleAudioVolume�ӿ�ʧ�ܡ�"));
		return fVolume;
	}

	// ��ȡ����
	spVolume->GetMasterVolume(&fVolume);

	return fVolume;
}


// ���ûỰ����
void AudioManager::SetSessionVolume(DWORD nIndex, FLOAT fVolume)
{
	DWORD count = this->GetSessionCount();
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
	// ��ȡ��ǰ��Ҫ���ûỰ�����Ľ���ID
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
	// ö�����лỰ����������ָ������ID�Ự������
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
			// ��ȡISimpleAudioVolume�ӿ�
			CComPtr<ISimpleAudioVolume> spVolume;
			hr = spSession->QueryInterface(__uuidof(ISimpleAudioVolume), reinterpret_cast<void**>(&spVolume));
			if (FAILED(hr))
			{
				LOG(_T("Get ISimpleAudioVolume failed."));
				continue;
			}
			// ��������
			hr = spVolume->SetMasterVolume(fVolume, nullptr);
			if (FAILED(hr))
			{
				LOG(_T("Set the session volume failed."));
				continue;
			}
		}
	}
}

// �����Ự
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
	// ��ȡ��ǰ��Ҫ�����Ự�Ľ���ID
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
	// ö�����лỰ����������ָ������ID�ĻỰ
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
			// ��ȡISimpleAudioVolume�ӿ�
			CComPtr<ISimpleAudioVolume> spVolume;
			hr = spSession->QueryInterface(__uuidof(ISimpleAudioVolume), reinterpret_cast<void**>(&spVolume));
			if (FAILED(hr))
			{
				LOG(_T("Get ISimpleAudioVolume failed."));
				continue;
			}
			// �����Ự
			hr = spVolume->SetMute(bMute, nullptr);
			if (FAILED(hr))
			{
				LOG(_T("Mute the session failed."));
				continue;
			}
		}
	}
}


// ��ȡ�Ự����״̬
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

	// ��ȡISimpleAudioVolume�ӿ�
	CComPtr<ISimpleAudioVolume> spVolume;
	HRESULT hr = spSession->QueryInterface(__uuidof(ISimpleAudioVolume), reinterpret_cast<void**>(&spVolume));
	if (FAILED(hr))
	{
		LOG(_T("Get ISimpleAudioVolume failed."));
		return FALSE;
	}

	// ��ȡ�Ự����״̬
	BOOL bMute;
	hr = spVolume->GetMute(&bMute);
	if (FAILED(hr))
	{
		LOG(_T("Get mute state failed."));
		return FALSE;
	}
	return bMute;
}


// ��ȡ�Ự��ǰ�����豸
DWORD AudioManager::GetSessionPlaybackDevice(DWORD dwIndex)
{
	// �������
	if (dwIndex < 0 || dwIndex >= this->GetSessionCount())
	{
		LOG(_T("�������󡣸��������������˻Ự�б��±귶Χ��"));
		return -1;
	}
	// ͨ��IAudioSessionControl��ȡIAudioSessionControl2�ӿ�
	CComPtr<IAudioSessionControl> spSession;
	AUDIO_CONTROL_SESSION_ENTITY entity = this->GetSession(dwIndex);
	spSession = entity.spObject;
	CComPtr<IAudioSessionControl2> spSession2;
	HRESULT hr = spSession->QueryInterface(__uuidof(IAudioSessionControl2), reinterpret_cast<void**>(&spSession2));
	if (FAILED(hr))
	{
		LOG(_T("��ȡIAudioSessionControl2�ӿ�ʧ��"));
		return -1;
	}
	// ��ȡ�Ự����ID
	DWORD dwProcessId = 0;
	hr = spSession2->GetProcessId(&dwProcessId);
	if (FAILED(hr) || dwProcessId == 0)
	{
		LOG(_T("��ȡ����IDʧ�ܡ�"));
		return -1;
	}
	// ��ȡIActivationFactory�ӿ�
	IActivationFactory* pFactory = nullptr;
	HSTRING CLSID_AudioPolicyConfig;
	LPCWSTR szClsid = L"Windows.Media.Internal.AudioPolicyConfig";
	hr = WindowsCreateString(szClsid, (UINT)wcslen(szClsid), &CLSID_AudioPolicyConfig);
	if (FAILED(hr))
	{
		LOG(_T("����CLSID_AudioPolicyConfig�ַ���ʧ�ܡ�"));
		return -1;
	}
	hr = RoGetActivationFactory(CLSID_AudioPolicyConfig, __uuidof(IActivationFactory), reinterpret_cast<void**>(&pFactory));
	if (FAILED(hr))
	{
		LOG(_T("��ȡIActivationFactory�ӿ�ʧ�ܡ�"));
		WindowsDeleteString(CLSID_AudioPolicyConfig);
		return -1;
	}
	// ��ȡIAudioPolicyConfig2�ӿ�
	IAudioPolicyConfig2* pAudioPolicyConfig = nullptr;
	hr = pFactory->QueryInterface(__uuidof(IAudioPolicyConfig2), reinterpret_cast<void**>(&pAudioPolicyConfig));
	if (FAILED(hr))
	{
		LOG(_T("��ȡIAudioPolicyConfig2�ӿ�ʧ�ܣ����Ի�ȡIAudioPolicyConfig�ӿڡ�"));
		hr = pFactory->QueryInterface(__uuidof(IAudioPolicyConfig), reinterpret_cast<void**>(&pAudioPolicyConfig));
		if (FAILED(hr))
		{
			LOG(_T("��ȡIAudioPolicyConfig�ӿ�ʧ�ܡ�"));
			WindowsDeleteString(CLSID_AudioPolicyConfig);
			pFactory->Release();
			return -1;
		}
	}
	// ��ȡ��ǰ�����豸ID
	HSTRING deviceId = nullptr;
	hr = pAudioPolicyConfig->GetPersistedDefaultAudioEndpoint(dwProcessId, eRender, eMultimedia, &deviceId);
	if (FAILED(hr) || !deviceId)
	{
		LOG(_T("��ȡ��ǰ�����豸IDʧ�ܡ�"));
		WindowsDeleteString(CLSID_AudioPolicyConfig);
		pAudioPolicyConfig->Release();
		pFactory->Release();
		return -1;
	}
	CString strDeviceId(WindowsGetStringRawBuffer(deviceId, nullptr), WindowsGetStringLen(deviceId));
	WindowsDeleteString(deviceId);
	for (auto it = this->_listPlaybackDevice.begin(); it != this->_listPlaybackDevice.end(); it++)
	{
		if (strDeviceId.Find(it->strId) >= 0)
		{
			LOG(_T("�ҵ���ǰ�����豸��"));
			pAudioPolicyConfig->Release();
			pFactory->Release();
			return static_cast<DWORD>(std::distance(this->_listPlaybackDevice.begin(), it));
		}
	}
	pAudioPolicyConfig->Release();
	pFactory->Release();

	return -1;
}


// ���ûỰ��ǰ�����豸
void AudioManager::SetSessionPlaybackDevice(DWORD dwSessionIndex, DWORD dwDeviceIndex)
{
	// �������
	if (dwSessionIndex < 0 || dwSessionIndex >= this->GetSessionCount()
		|| dwDeviceIndex < 0 || dwDeviceIndex >= this->GetPlaybackDeviceCount())
	{
		LOG(_T("�������󡣸��������������˻Ự�б��±귶Χ��"));
		return;
	}
	// ��ȡ�����豸ID
	AUDIO_CONTROL_DEVICE_ENTITY deviceEntity = this->GetPlaybackDevice(dwDeviceIndex);
	CString strDeviceId = deviceEntity.strId;
	// ͨ��IAudioSessionControl��ȡIAudioSessionControl2�ӿ�
	CComPtr<IAudioSessionControl> spSession;
	AUDIO_CONTROL_SESSION_ENTITY sessionEntity = this->GetSession(dwSessionIndex);
	spSession = sessionEntity.spObject;
	CComPtr<IAudioSessionControl2> spSession2;
	HRESULT hr = spSession->QueryInterface(__uuidof(IAudioSessionControl2), reinterpret_cast<void**>(&spSession2));
	if (FAILED(hr))
	{
		LOG(_T("��ȡIAudioSessionControl2�ӿ�ʧ��"));
		return;
	}
	// ��ȡ�Ự����ID
	DWORD dwProcessId = 0;
	hr = spSession2->GetProcessId(&dwProcessId);
	if (FAILED(hr) || dwProcessId == 0)
	{
		LOG(_T("��ȡ����IDʧ�ܡ�"));
		return;
	}
	// ��ȡIActivationFactory�ӿ�
	IActivationFactory* pFactory = nullptr;
	HSTRING CLSID_AudioPolicyConfig;
	LPCWSTR szClsid = L"Windows.Media.Internal.AudioPolicyConfig";
	hr = WindowsCreateString(szClsid, (UINT)wcslen(szClsid), &CLSID_AudioPolicyConfig);
	if (FAILED(hr))
	{
		LOG(_T("����CLSID_AudioPolicyConfig�ַ���ʧ�ܡ�"));
		return;
	}
	hr = RoGetActivationFactory(CLSID_AudioPolicyConfig, __uuidof(IActivationFactory), reinterpret_cast<void**>(&pFactory));
	if (FAILED(hr))
	{
		LOG(_T("��ȡIActivationFactory�ӿ�ʧ�ܡ�"));
		WindowsDeleteString(CLSID_AudioPolicyConfig);
		return;
	}
	// ��ȡIAudioPolicyConfig2�ӿ�
	IAudioPolicyConfig2* pAudioPolicyConfig = nullptr;
	hr = pFactory->QueryInterface(__uuidof(IAudioPolicyConfig2), reinterpret_cast<void**>(&pAudioPolicyConfig));
	if (FAILED(hr))
	{
		LOG(_T("��ȡIAudioPolicyConfig2�ӿ�ʧ�ܣ����Ի�ȡIAudioPolicyConfig�ӿڡ�"));
		hr = pFactory->QueryInterface(__uuidof(IAudioPolicyConfig), reinterpret_cast<void**>(&pAudioPolicyConfig));
		if (FAILED(hr))
		{
			LOG(_T("��ȡIAudioPolicyConfig�ӿ�ʧ�ܡ�"));
			WindowsDeleteString(CLSID_AudioPolicyConfig);
			pFactory->Release();
			return;
		}
	}
	// ���������豸ID
	HSTRING deviceId = nullptr;
	CString strFullDeviceId = this->GenerateDeviceId(eRender, strDeviceId);
	hr = WindowsCreateString(strFullDeviceId, strFullDeviceId.GetLength(), &deviceId);
	if (FAILED(hr) || !deviceId)
	{
		LOG(_T("�����豸ID�ַ���ʧ�ܡ�"));
		WindowsDeleteString(CLSID_AudioPolicyConfig);
		pAudioPolicyConfig->Release();
		pFactory->Release();
		return;
	}
	// ���ûỰĬ�ϲ����豸
	hr = pAudioPolicyConfig->SetPersistedDefaultAudioEndpoint(dwProcessId, eRender, eMultimedia, deviceId);
	if (FAILED(hr) || !deviceId)
	{
		LOG(_T("���õ�ǰ�����豸IDʧ�ܡ�"));
		WindowsDeleteString(CLSID_AudioPolicyConfig);
		WindowsDeleteString(deviceId);
		pAudioPolicyConfig->Release();
		pFactory->Release();
		return;
	}
	hr = pAudioPolicyConfig->SetPersistedDefaultAudioEndpoint(dwProcessId, eRender, eConsole, deviceId);
	if (FAILED(hr) || !deviceId)
	{
		LOG(_T("���õ�ǰ�����豸IDʧ�ܡ�"));
		WindowsDeleteString(CLSID_AudioPolicyConfig);
		WindowsDeleteString(deviceId);
		pAudioPolicyConfig->Release();
		pFactory->Release();
		return;
	}
	WindowsDeleteString(deviceId);
	hr = pAudioPolicyConfig->SetPersistedDefaultAudioEndpoint(dwProcessId, eRender, eCommunications, deviceId);
	if (FAILED(hr) || !deviceId)
	{
		LOG(_T("���õ�ǰ�����豸IDʧ�ܡ�"));
		WindowsDeleteString(CLSID_AudioPolicyConfig);
		WindowsDeleteString(deviceId);
		pAudioPolicyConfig->Release();
		pFactory->Release();
		return;
	}
	pAudioPolicyConfig->Release();
	pFactory->Release();
}


// �������лỰ�����豸
void AudioManager::ResetAllSessionDevice()
{
	// ��ȡIActivationFactory�ӿ�
	HRESULT hr;
	IActivationFactory* pFactory = nullptr;
	HSTRING CLSID_AudioPolicyConfig;
	LPCWSTR szClsid = L"Windows.Media.Internal.AudioPolicyConfig";
	hr = WindowsCreateString(szClsid, (UINT)wcslen(szClsid), &CLSID_AudioPolicyConfig);
	if (FAILED(hr))
	{
		LOG(_T("����CLSID_AudioPolicyConfig�ַ���ʧ�ܡ�"));
		return;
	}
	hr = RoGetActivationFactory(CLSID_AudioPolicyConfig, __uuidof(IActivationFactory), reinterpret_cast<void**>(&pFactory));
	if (FAILED(hr))
	{
		LOG(_T("��ȡIActivationFactory�ӿ�ʧ�ܡ�"));
		WindowsDeleteString(CLSID_AudioPolicyConfig);
		return;
	}
	// ��ȡIAudioPolicyConfig2�ӿ�
	IAudioPolicyConfig2* pAudioPolicyConfig = nullptr;
	hr = pFactory->QueryInterface(__uuidof(IAudioPolicyConfig2), reinterpret_cast<void**>(&pAudioPolicyConfig));
	if (FAILED(hr))
	{
		LOG(_T("��ȡIAudioPolicyConfig2�ӿ�ʧ�ܣ����Ի�ȡIAudioPolicyConfig�ӿڡ�"));
		hr = pFactory->QueryInterface(__uuidof(IAudioPolicyConfig), reinterpret_cast<void**>(&pAudioPolicyConfig));
		if (FAILED(hr))
		{
			LOG(_T("��ȡIAudioPolicyConfig�ӿ�ʧ�ܡ�"));
			WindowsDeleteString(CLSID_AudioPolicyConfig);
			pFactory->Release();
			return;
		}
	}
	hr = pAudioPolicyConfig->ClearAllPersistedApplicationDefaultEndpoints();
	if (FAILED(hr))
	{
		LOG(_T("ִ��IAudioPolicyConfig2::ClearAllPersistedApplicationDefaultEndpoints()ʧ�ܡ�"));
	}
	WindowsDeleteString(CLSID_AudioPolicyConfig);
	pAudioPolicyConfig->Release();
	pFactory->Release();
}


// ���ûỰ¼���豸
void AudioManager::SetSessionRecordingDevice(DWORD dwSessionIndex, DWORD dwDeviceIndex)
{
	// �������
	if (dwSessionIndex < 0 || dwSessionIndex >= this->GetSessionCount()
		|| dwDeviceIndex < 0 || dwDeviceIndex >= this->GetRecordingDeviceCount())
	{
		LOG(_T("�������󡣸��������������˻Ự�б��±귶Χ��"));
		return;
	}
	// ��ȡ�����豸ID
	AUDIO_CONTROL_DEVICE_ENTITY deviceEntity = this->GetRecordingDevice(dwDeviceIndex);
	CString strDeviceId = deviceEntity.strId;
	CComPtr<IAudioSessionControl> spSession;
	AUDIO_CONTROL_SESSION_ENTITY sessionEntity = this->GetSession(dwSessionIndex);
	DWORD dwProcessId = sessionEntity.dwProcessId;
	if (dwProcessId == 0)
	{
		LOG(_T("��ȡ����IDʧ�ܡ�"));
		return;
	}
	// ��ȡIActivationFactory�ӿ�
	IActivationFactory* pFactory = nullptr;
	HSTRING CLSID_AudioPolicyConfig;
	LPCWSTR szClsid = L"Windows.Media.Internal.AudioPolicyConfig";
	HRESULT hr = WindowsCreateString(szClsid, (UINT)wcslen(szClsid), &CLSID_AudioPolicyConfig);
	if (FAILED(hr))
	{
		LOG(_T("����CLSID_AudioPolicyConfig�ַ���ʧ�ܡ�"));
		return;
	}
	hr = RoGetActivationFactory(CLSID_AudioPolicyConfig, __uuidof(IActivationFactory), reinterpret_cast<void**>(&pFactory));
	if (FAILED(hr))
	{
		LOG(_T("��ȡIActivationFactory�ӿ�ʧ�ܡ�"));
		WindowsDeleteString(CLSID_AudioPolicyConfig);
		return;
	}
	// ��ȡIAudioPolicyConfig2�ӿ�
	IAudioPolicyConfig2* pAudioPolicyConfig = nullptr;
	hr = pFactory->QueryInterface(__uuidof(IAudioPolicyConfig2), reinterpret_cast<void**>(&pAudioPolicyConfig));
	if (FAILED(hr))
	{
		LOG(_T("��ȡIAudioPolicyConfig2�ӿ�ʧ�ܣ����Ի�ȡIAudioPolicyConfig�ӿڡ�"));
		hr = pFactory->QueryInterface(__uuidof(IAudioPolicyConfig), reinterpret_cast<void**>(&pAudioPolicyConfig));
		if (FAILED(hr))
		{
			LOG(_T("��ȡIAudioPolicyConfig�ӿ�ʧ�ܡ�"));
			WindowsDeleteString(CLSID_AudioPolicyConfig);
			pFactory->Release();
			return;
		}
	}
	// ����¼���豸ID
	HSTRING deviceId = nullptr;
	CString strFullDeviceId = this->GenerateDeviceId(eCapture, strDeviceId);
	hr = WindowsCreateString(strFullDeviceId, strFullDeviceId.GetLength(), &deviceId);
	if (FAILED(hr) || !deviceId)
	{
		LOG(_T("�����豸ID�ַ���ʧ�ܡ�"));
		WindowsDeleteString(CLSID_AudioPolicyConfig);
		pAudioPolicyConfig->Release();
		pFactory->Release();
		return;
	}
	// ���ûỰĬ��¼���豸
	hr = pAudioPolicyConfig->SetPersistedDefaultAudioEndpoint(dwProcessId, eCapture, eMultimedia, deviceId);
	if (FAILED(hr))
	{
		CString strMessage;
		strMessage.Format(_T("���ûỰ¼���豸ʧ�ܡ�������=0x%x"), hr);
		LOG(strMessage);
	}
	hr = pAudioPolicyConfig->SetPersistedDefaultAudioEndpoint(dwProcessId, eCapture, eCommunications, deviceId);
	if (FAILED(hr))
	{
		CString strMessage;
		strMessage.Format(_T("���ûỰ¼���豸ʧ�ܡ�������=0x%x"), hr);
		LOG(strMessage);
	}
	hr = pAudioPolicyConfig->SetPersistedDefaultAudioEndpoint(dwProcessId, eCapture, eConsole, deviceId);
	if (FAILED(hr))
	{
		CString strMessage;
		strMessage.Format(_T("���ûỰ¼���豸ʧ�ܡ�������=0x%x"), hr);
		LOG(strMessage);
	}
	// �Ự¼���豸������ɣ��ͷ���Դ
	WindowsDeleteString(deviceId);
	pAudioPolicyConfig->Release();
	pFactory->Release();
}


// ��ȡ�Ự¼���豸
DWORD AudioManager::GetSessionRecordingDevice(DWORD dwIndex)
{
	// �������
	if (dwIndex < 0 || dwIndex >= this->GetSessionCount())
	{
		LOG(_T("�������󡣸��������������˻Ự�б��±귶Χ��"));
		return -1;
	}
	// ��ȡ�Ự����ID
	AUDIO_CONTROL_SESSION_ENTITY entity = this->GetSession(dwIndex);
	DWORD dwProcessId = entity.dwProcessId;
	if (dwProcessId == 0)
	{
		LOG(_T("��ȡ����IDʧ�ܡ�"));
		return -1;
	}
	// ��ȡIActivationFactory�ӿ�
	IActivationFactory* pFactory = nullptr;
	HSTRING CLSID_AudioPolicyConfig;
	LPCWSTR szClsid = L"Windows.Media.Internal.AudioPolicyConfig";
	HRESULT hr = WindowsCreateString(szClsid, (UINT)wcslen(szClsid), &CLSID_AudioPolicyConfig);
	if (FAILED(hr))
	{
		LOG(_T("����CLSID_AudioPolicyConfig�ַ���ʧ�ܡ�"));
		return -1;
	}
	hr = RoGetActivationFactory(CLSID_AudioPolicyConfig, __uuidof(IActivationFactory), reinterpret_cast<void**>(&pFactory));
	if (FAILED(hr))
	{
		LOG(_T("��ȡIActivationFactory�ӿ�ʧ�ܡ�"));
		WindowsDeleteString(CLSID_AudioPolicyConfig);
		return -1;
	}
	// ��ȡIAudioPolicyConfig2�ӿ�
	IAudioPolicyConfig2* pAudioPolicyConfig = nullptr;
	hr = pFactory->QueryInterface(__uuidof(IAudioPolicyConfig2), reinterpret_cast<void**>(&pAudioPolicyConfig));
	if (FAILED(hr))
	{
		LOG(_T("��ȡIAudioPolicyConfig2�ӿ�ʧ�ܣ����Ի�ȡIAudioPolicyConfig�ӿڡ�"));
		hr = pFactory->QueryInterface(__uuidof(IAudioPolicyConfig), reinterpret_cast<void**>(&pAudioPolicyConfig));
		if (FAILED(hr))
		{
			LOG(_T("��ȡIAudioPolicyConfig�ӿ�ʧ�ܡ�"));
			WindowsDeleteString(CLSID_AudioPolicyConfig);
			pFactory->Release();
			return -1;
		}
	}
	// ��ȡ��ǰ¼���豸ID
	HSTRING deviceId = nullptr;
	hr = pAudioPolicyConfig->GetPersistedDefaultAudioEndpoint(dwProcessId, eCapture, eMultimedia, &deviceId);
	if (FAILED(hr) || !deviceId)
	{
		LOG(_T("��ȡ�Ự¼���豸IDʧ�ܡ�"));
		WindowsDeleteString(CLSID_AudioPolicyConfig);
		pAudioPolicyConfig->Release();
		pFactory->Release();
		return -1;
	}
	CString strDeviceId(WindowsGetStringRawBuffer(deviceId, nullptr), WindowsGetStringLen(deviceId));
	WindowsDeleteString(deviceId);
	CString strMessage;
	strMessage.Format(_T("��ǰ�Ự¼���豸ID=%s"), strDeviceId);
	LOG(strMessage);
	for (auto it = this->_listRecordingDevice.begin(); it != this->_listRecordingDevice.end(); it++)
	{
		if (strDeviceId.Find(it->strId) >= 0)
		{
			LOG(_T("�ҵ���ǰ¼���豸��"));
			pAudioPolicyConfig->Release();
			pFactory->Release();
			return static_cast<DWORD>(std::distance(this->_listRecordingDevice.begin(), it));
		}
	}
	pAudioPolicyConfig->Release();
	pFactory->Release();

	return -1;
}

// ���ô��ھ���
void AudioManager::SetWindowMute(HWND hWnd, BOOL bMute)
{
	// ͨ�����ھ���õ�����ID
	DWORD dwProcessId = 0;
	GetWindowThreadProcessId(hWnd, &dwProcessId);
	if (dwProcessId == 0)
	{
		LOG(_T("ִ��GetWindowThreadProcessId()ʧ�ܡ�"));
		return;
	}
	// ͨ������ID�ҵ��Ự����
	for (auto it = this->_listSession.begin(); it != this->_listSession.end(); it++)
	{
		if (it->dwProcessId == dwProcessId)
		{
			DWORD dwIndex = static_cast<DWORD>(std::distance(this->_listSession.begin(), it));
			this->SetSessionMute(dwIndex, bMute);
			return;
		}
	}
	LOG(_T("û���ҵ��Ự��"));
}


// ��ȡ���ھ���״̬
BOOL AudioManager::GetWindowMute(HWND hWnd)
{
	// ͨ�����ھ���õ�����ID
	DWORD dwProcessId = 0;
	GetWindowThreadProcessId(hWnd, &dwProcessId);
	if (dwProcessId == 0)
	{
		LOG(_T("ִ��GetWindowThreadProcessId()ʧ�ܡ�"));
		return FALSE;
	}
	// ͨ������ID�ҵ��Ự����
	for (auto it = this->_listSession.begin(); it != this->_listSession.end(); it++)
	{
		if (it->dwProcessId == dwProcessId)
		{
			DWORD dwIndex = static_cast<DWORD>(std::distance(this->_listSession.begin(), it));
			return this->GetSessionMute(dwIndex);
		}
	}
	return FALSE;
}
