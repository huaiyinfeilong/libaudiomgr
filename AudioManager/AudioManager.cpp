#include "AudioManager.h"
#include <windowsx.h>
#include <oleacc.h>
#include <endpointvolume.h>


void MsgBox(LPCTSTR lpText)
{
	MessageBox(GetForegroundWindow(), lpText, TEXT(""), MB_OK);
}


// ��ʼ��
void AudioManager::Initialize()
{
	CoInitialize(NULL);
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
	CoUninitialize();
}


// ��ȡ�ط��豸����
UINT AudioManager::GetPlaybackDeviceCount()
{
	return (UINT)this->_listPlaybackDevice.size();
}


// ��ȡ�ط��豸
AUDIO_CONTROL_DEVICE_ENTITY AudioManager::GetPlaybackDevice(UINT nIndex)
{
	return this->_listPlaybackDevice[nIndex];
}


// ��ȡ¼���豸����
UINT AudioManager::GetRecordingDeviceCount()
{
	return (UINT)this->_listRecordingDevice.size();
}


// ��ȡ¼���豸
AUDIO_CONTROL_DEVICE_ENTITY AudioManager::GetRecordingDevice(UINT nIndex)
{
	return this->_listRecordingDevice[nIndex];
}


// ��ȡ�Ự����
UINT AudioManager::GetSessionCount()
{
	return (UINT)_listSession.size();
}


// ��ȡ�Ự
AUDIO_CONTROL_SESSION_ENTITY AudioManager::GetSession(UINT nIndex) const
{
	return _listSession[nIndex];
}


// �������ͻ�ȡ�豸�б�
void AudioManager::GetDevices(EDataFlow dataFlow, DWORD dwStateMask, std::vector<AUDIO_CONTROL_DEVICE_ENTITY>& listDevice)
{
	listDevice.clear();
	CComPtr<IMMDeviceEnumerator> spEnumerator;
	HRESULT hr = spEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
	if (FAILED(hr))
	{
		ATLTRACE(_T("��ȡIMMDeviceEnumerator�ӿ�ʧ�ܡ�"));
		return;
	}
	CComPtr<IMMDeviceCollection> spDevices;
	hr = spEnumerator->EnumAudioEndpoints(dataFlow, eMultimedia, &spDevices);
	if (FAILED(hr))
	{
		ATLTRACE(_T("��ȡIMMDeviceCollection�ӿ�ʧ�ܡ�"));
		return;
	}
	UINT count = 0;
	hr = spDevices->GetCount(&count);
	if (FAILED(hr) || !count)
	{
		ATLTRACE(_T("��ȡ��0���豸��"));
		return;
	}
	for (UINT i = 0; i < count; i++)
	{
		CComPtr<IMMDevice> spDevice;
		hr = spDevices->Item(i, &spDevice);
		if (FAILED(hr))
		{
			ATLTRACE(_T("��ȡIMMDevice�ӿ�ʧ�ܡ�"));
			continue;
		}
		CComPtr<IPropertyStore> spStore;
		hr = spDevice->OpenPropertyStore(STGM_READ, &spStore);
		if (FAILED(hr))
		{
			ATLTRACE(_T("��ȡIPropertyStore�ӿ�ʧ�ܡ�"));
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
			ATLTRACE(_T("PropertyVariant��vt����VT_LPWSTR"));
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


// ��ȡ���лỰ
void AudioManager::GetAllSession()
{
	// ��ȡ��ǰ���д��ڼ������ID
	std::map<DWORD, CString> context;
	GetWindows(context);
	// ��ȡ����session
	HRESULT hr = S_OK;
	CComPtr<IMMDeviceEnumerator> spDeviceEnumerator;
	hr = spDeviceEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
	if (FAILED(hr))
	{
		ATLTRACE(_T("Get IMMDeviceEnumerator failed."));
		return;
	}
	CComPtr<IMMDeviceCollection> spDeviceCollection;
	hr = spDeviceEnumerator->EnumAudioEndpoints(eRender, eMultimedia, &spDeviceCollection);
	if (FAILED(hr))
	{
		ATLTRACE(_T("Get IMMDeviceCollection failed."));
		return;
	}
	UINT uDeviceCount = 0;
	hr = spDeviceCollection->GetCount(&uDeviceCount);
	if (FAILED(hr) || uDeviceCount == 0)
	{
		ATLTRACE(_T("Get device count failed."));
		return;
	}
	CComPtr<IMMDevice> spDefaultDevice;
	hr = spDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &spDefaultDevice);
	if (FAILED(hr))
	{
		ATLTRACE(_T("Get default device failed."));
		return;
	}
	for (UINT i = 0; i < uDeviceCount; i++)
	{
		CComPtr<IMMDevice> spDevice;
		hr = spDeviceCollection->Item(i, &spDevice);
		if (FAILED(hr))
		{
			ATLTRACE(_T("Get device failed."));
			return;
		}
		CComPtr<IAudioSessionManager2> spSessionManager;
		hr = spDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_INPROC_SERVER, NULL, reinterpret_cast<void**>(&spSessionManager));
		if (FAILED(hr))
		{
			ATLTRACE(_T("Get AudioSessionManager2 failed."));
			return;
		}
		CComPtr<IAudioSessionEnumerator> spSessionEnumerator;
		hr = spSessionManager->GetSessionEnumerator(&spSessionEnumerator);
		if (FAILED(hr))
		{
			ATLTRACE(_T("Get IAudioSessionEnumerator failed."));
			return;
		}
		int nSessionCount = 0;
		hr = spSessionEnumerator->GetCount(&nSessionCount);
		if (FAILED(hr) || nSessionCount == 0)
		{
			ATLTRACE(_T("Get session count failed."));
			return;
		}
		for (int j = 0; j < nSessionCount; j++)
		{
			CComPtr<IAudioSessionControl> spSession;
			hr = spSessionEnumerator->GetSession(j, &spSession);
			if (FAILED(hr))
			{
				ATLTRACE(_T("Get session failed."));
				continue;
			}
			CComPtr<IAudioSessionControl2> spSession2;
			hr = spSession->QueryInterface(__uuidof(IAudioSessionControl2), reinterpret_cast<void**>(&spSession2));
			if (FAILED(hr))
			{
				ATLTRACE(_T("Get session2 failed."));
				continue;
			}
			if (spSession2->IsSystemSoundsSession() != S_FALSE)
			{
				LPWSTR lpwDefaultDeviceId = nullptr;
				LPWSTR lpwCurrentDeviceId = nullptr;
				hr = spDefaultDevice->GetId(&lpwDefaultDeviceId);
				if (FAILED(hr))
				{
					ATLTRACE(_T("Get default device id failed."));
					continue;
				}
				hr = spDevice->GetId(&lpwCurrentDeviceId);
				if (FAILED(hr))
				{
					ATLTRACE(_T("Get current device id failed."));
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
				entity.strName = TEXT("ϵͳ����");
				entity.spObject = spSession;
				_listSession.push_back(entity);
				CoTaskMemFree(lpwSessionId);
				continue;
			}
			AUDIO_CONTROL_SESSION_ENTITY entity;
			this->GetSessionDisplayName(spSession2, entity.strName);
			LPTSTR lpSessionId = nullptr;
			hr = spSession2->GetSessionInstanceIdentifier(&lpSessionId);
			if (FAILED(hr) || !lpSessionId)
			{
				ATLTRACE(_T("Get session id failed."));
				continue;
			}
			entity.strId = lpSessionId;
			CoTaskMemFree(lpSessionId);
			entity.spObject = spSession;
			this->_listSession.push_back(entity);
		}
	}
}


// ͨ������ID��ȡ���ڱ���
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


// ��ȡ�Ự����
void AudioManager::GetSessionDisplayName(CComPtr<IAudioSessionControl2>& spSession, CString& strDisplayName)
{
	LPTSTR lpDisplayName = nullptr;
	HRESULT hr = spSession->GetDisplayName(&lpDisplayName);
	if (FAILED(hr) || lpDisplayName == nullptr)
	{
		return;
	}
	// �ɹ���ȡ������
	if (*lpDisplayName)
	{
		strDisplayName = lpDisplayName;
		CoTaskMemFree(lpDisplayName);
		return;
	}
	// ��ȡ����ʧ�ܣ����Ի�ȡexe�ļ������ֶ�
	// ����ͨ������ID��ȡ��������·��
	// Ȼ��ͨ����������·���õ��ļ���Ϣ�е��ļ������ֶ�����
	CoTaskMemFree(lpDisplayName);
	// ��ȡ����ID
	DWORD dwProcessId = 0;
	hr = spSession->GetProcessId(&dwProcessId);
	if (FAILED(hr) || dwProcessId == 0)
	{
		return;
	}
	// ͨ������ID�õ�����·��
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
	if (hProcess == NULL)
	{
		return;
	}
	TCHAR szExePath[MAX_PATH] = { 0 };
	DWORD dwLength = MAX_PATH;
	QueryFullProcessImageName(hProcess, 0, szExePath, &dwLength);
	if (dwLength == 0)
	{
		TCHAR szError[MAX_PATH] = { 0 };
		_stprintf_s(szError, MAX_PATH, TEXT("error code=0x%x"), GetLastError());
		return;
	}
	DWORD dwSize = 0;
	DWORD dwHandle = 0;
	dwSize = GetFileVersionInfoSize(szExePath, &dwHandle);
	if (dwSize == 0)
	{
		return;
	}
	LPVOID lpBuffer = new BYTE[dwSize];
	ZeroMemory(lpBuffer, dwSize);
	if (!GetFileVersionInfo(szExePath, dwHandle, dwSize, lpBuffer))
	{
		delete[]lpBuffer;
		return;
	}
	dwLength = 0;
	// ��ȡ���Ժ��ַ�������
	LPVOID lpLangCode = nullptr;
	if (VerQueryValue(lpBuffer, TEXT("\\VarFileInfo\\Translation"), &lpLangCode, (UINT*)&dwLength))
	{
		// ������Ʒ���Ʋ�ѯ�ַ���
		TCHAR szQuery[MAX_PATH] = { 0 };
		_stprintf_s(szQuery, MAX_PATH, TEXT("\\StringFileInfo\\%.4x%.4x\\ProductName"), LOWORD((DWORD)(*(LPDWORD)lpLangCode)), HIWORD((DWORD)(*(LPDWORD)lpLangCode)));
		// ��ȡ��Ʒ����
		LPVOID lpProductName = nullptr;
		if (VerQueryValue(lpBuffer, szQuery, &lpProductName, (UINT*)&dwLength))
		{
			strDisplayName = (LPTSTR)lpProductName;
			delete[]lpBuffer;
			return;
		}
		// �����ļ�������ѯ�ַ���
		_stprintf_s(szQuery, MAX_PATH, TEXT("\\StringFileInfo\\%.4x%.4x\\FileDescription"), LOWORD((DWORD)(*(LPDWORD)lpLangCode)), HIWORD((DWORD)(*(LPDWORD)lpLangCode)));
		// ��ȡ�ļ�����
		LPVOID lpFileDescription = nullptr;
		if (VerQueryValue(lpBuffer, szQuery, &lpFileDescription, (UINT*)&dwLength))
		{
			strDisplayName = (LPTSTR)lpFileDescription;
			delete[]lpBuffer;
			return;
		}
		delete[]lpBuffer;
	}
	if (!*szExePath)
	{
		return;
	}
	CString strExePath = szExePath;
	DWORD dwLeft = strExePath.ReverseFind(TEXT('\\'));
	DWORD dwRight = strExePath.ReverseFind(TEXT('.'));
	strExePath = strExePath.Mid(dwLeft, (dwRight - dwLeft));
	strDisplayName = strExePath;
}


// ��ȡ�ط��豸����
FLOAT AudioManager::GetPlaybackDeviceVolume(UINT nIndex)
{
	FLOAT fVolume = 0.00f;
	AUDIO_CONTROL_DEVICE_ENTITY entity = this->GetPlaybackDevice(nIndex);
	CComPtr<IMMDevice> spDevice = entity.spObject;
	if (!spDevice)
	{
		ATLTRACE(_T("û���ҵ�ָ�����豸��"));
		return fVolume;
	}

	// ��ȡIAudioEndpointVolume�ӿ�
	CComPtr<IAudioEndpointVolume> spVolume;
	HRESULT hr = spDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, reinterpret_cast<void**>(&spVolume));
	if (FAILED(hr))
	{
		ATLTRACE(_T("��ȡIAudioEndpointVolume�ӿ�ʧ�ܡ�"));
		return fVolume;
	}

	// ��ȡ����
	spVolume->GetMasterVolumeLevelScalar(&fVolume);

	return fVolume;
}


// ���ûط��豸����
void AudioManager::SetPlaybackDeviceVolume(UINT nIndex, FLOAT fVolume)
{
	AUDIO_CONTROL_DEVICE_ENTITY entity = this->GetPlaybackDevice(nIndex);
	CComPtr<IMMDevice> spDevice = entity.spObject;
	if (!spDevice)
	{
		ATLTRACE(_T("û���ҵ�ָ�����豸��"));
		return;
	}

	// ��ȡIAudioEndpointVolume�ӿ�
	CComPtr<IAudioEndpointVolume> spVolume;
	HRESULT hr = spDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, reinterpret_cast<void**>(&spVolume));
	if (FAILED(hr))
	{
		ATLTRACE(_T("��ȡIAudioEndpointVolume�ӿ�ʧ�ܡ�"));
		return;
	}

	// ��������
	spVolume->SetMasterVolumeLevelScalar(fVolume, nullptr);
}


// ��ȡ¼���豸����
FLOAT AudioManager::GetRecordingDeviceVolume(UINT nIndex)
{
	FLOAT fVolume = 0.00f;
	AUDIO_CONTROL_DEVICE_ENTITY entity = this->GetRecordingDevice(nIndex);
	CComPtr<IMMDevice> spDevice = entity.spObject;
	if (!spDevice)
	{
		ATLTRACE(_T("û���ҵ�ָ�����豸��"));
		return fVolume;
	}

	// ��ȡIAudioEndpointVolume�ӿ�
	CComPtr<IAudioEndpointVolume> spVolume;
	HRESULT hr = spDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, reinterpret_cast<void**>(&spVolume));
	if (FAILED(hr))
	{
		ATLTRACE(_T("��ȡIAudioEndpointVolume�ӿ�ʧ�ܡ�"));
		return fVolume;
	}

	// ��ȡ����
	spVolume->GetMasterVolumeLevelScalar(&fVolume);

	return fVolume;
}


// ����¼���豸����
void AudioManager::SetRecordingDeviceVolume(UINT nIndex, FLOAT fVolume)
{
	AUDIO_CONTROL_DEVICE_ENTITY entity = this->GetRecordingDevice(nIndex);
	CComPtr<IMMDevice> spDevice = entity.spObject;
	if (!spDevice)
	{
		ATLTRACE(_T("û���ҵ�ָ�����豸��"));
		return;
	}

	// ��ȡIAudioEndpointVolume�ӿ�
	CComPtr<IAudioEndpointVolume> spVolume;
	HRESULT hr = spDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, reinterpret_cast<void**>(&spVolume));
	if (FAILED(hr))
	{
		ATLTRACE(_T("��ȡIAudioEndpointVolume�ӿ�ʧ�ܡ�"));
		return;
	}

	// ��������
	spVolume->SetMasterVolumeLevelScalar(fVolume, nullptr);
}


// ��ȡ�Ự����
FLOAT AudioManager::GetSessionVolume(UINT nIndex)
{
	FLOAT fVolume = 0.00f;
	AUDIO_CONTROL_SESSION_ENTITY entity = this->GetSession(nIndex);
	CComPtr<IAudioSessionControl> spSession = entity.spObject;
	if (!spSession)
	{
		ATLTRACE(_T("û���ҵ�ָ�����豸��"));
		return fVolume;
	}

	// ��ȡISimpleAudioVolume�ӿ�
	CComPtr<ISimpleAudioVolume> spVolume;
	HRESULT hr = spSession->QueryInterface(__uuidof(ISimpleAudioVolume), reinterpret_cast<void **>(&spVolume));
	if (FAILED(hr))
	{
		ATLTRACE(_T("��ȡISimpleAudioVolume�ӿ�ʧ�ܡ�"));
		return fVolume;
	}

	// ��ȡ����
	spVolume->GetMasterVolume(&fVolume);

	return fVolume;
}


// ���ûỰ����
void AudioManager::SetSessionVolume(UINT nIndex, FLOAT fVolume)
{
	AUDIO_CONTROL_SESSION_ENTITY entity = this->GetSession(nIndex);
	CComPtr<IAudioSessionControl> spSession = entity.spObject;
	if (!spSession)
	{
		ATLTRACE(_T("û���ҵ�ָ�����豸��"));
		return;
	}

	// ��ȡISimpleAudioVolume�ӿ�
	CComPtr<ISimpleAudioVolume> spVolume;
	HRESULT hr = spSession->QueryInterface(__uuidof(ISimpleAudioVolume), reinterpret_cast<void**>(&spVolume));
	if (FAILED(hr))
	{
		ATLTRACE(_T("��ȡISimpleAudioVolume�ӿ�ʧ�ܡ�"));
		return;
	}

	// ��������
	spVolume->SetMasterVolume(fVolume, nullptr);
}
