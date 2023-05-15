#include "pch.h"

#define LIBAUDIOMGRAPI

#include "LibAudioMgr.h"
#include "AudioManager.h"
#include <math.h>


AudioManager g_audioManager;

DWORD CopyStringBuffer(LPCWSTR src, DWORD srcLen, LPWSTR dst, DWORD dstLen)
{
	// ��ȡ��Ҫ�Ļ������ַ�����
	DWORD len = (dstLen > srcLen) ? srcLen : dstLen - 1;
	// �����������ɵ��ַ������������1
	if (len < 1)
	{
		return 0;
	}
	// ��������
	_tcsncpy_s(dst, dstLen, src, len);
	dst[len] = 0;
	return len;
}


// ��ʼ��
void WINAPI LAM_Initialize()
{
	g_audioManager.Initialize();
}


// ���ʼ��
void WINAPI LAM_Uninitialize()
{
	g_audioManager.Uninitialize();
}


// ��ȡ�����豸����
DWORD WINAPI LAM_GetPlaybackDeviceCount()
{
	return g_audioManager.GetPlaybackDeviceCount();
}


// ��ȡ�����豸����Unicode��
DWORD WINAPI LAM_GetPlaybackDeviceName(DWORD dwIndex, LPWSTR lpszName, DWORD dwLength)
{
	UINT count = g_audioManager.GetPlaybackDeviceCount();
	if (dwIndex < 0 || dwIndex >= count)
	{
		return 0;
	}

	AUDIO_CONTROL_DEVICE_ENTITY entity = g_audioManager.GetPlaybackDevice((UINT)dwIndex);
	if (lpszName == nullptr)
	{
		return entity.strName.GetLength();
	}
	return CopyStringBuffer(entity.strName, entity.strName.GetLength(), lpszName, dwLength);
}


// ��ȡ�����豸����
DWORD WINAPI LAM_GetPlaybackDeviceVolume(DWORD dwIndex)
{
	FLOAT fVolume = g_audioManager.GetPlaybackDeviceVolume((UINT)dwIndex);
	return static_cast<DWORD>(std::round(fVolume * 100.00f));
}


// ���ò����豸����
void WINAPI LAM_SetPlaybackDeviceVolume(DWORD dwIndex, DWORD dwVolume)
{
	g_audioManager.SetPlaybackDeviceVolume((UINT)dwIndex, static_cast<FLOAT>(dwVolume) * 0.01f);
}


// ���ò����豸����
void WINAPI LAM_SetPlaybackDeviceMute(DWORD dwIndex, BOOL bMute)
{
	g_audioManager.SetPlaybackDeviceMute((UINT)dwIndex, bMute);
}


// ��ȡ�����豸����״̬
BOOL WINAPI LAM_GetPlaybackDeviceMute(DWORD dwIndex)
{
	return g_audioManager.GetPlaybackDeviceMute((UINT)dwIndex);
}


// ����Ĭ�ϲ����豸
void WINAPI LAM_SetDefaultPlaybackDevice(DWORD dwIndex)
{
	g_audioManager.SetDefaultPlaybackDevice(dwIndex);
}


// ��ȡĬ�ϲ����豸
DWORD WINAPI LAM_GetDefaultPlaybackDevice()
{
	return g_audioManager.GetDefaultPlaybackDevice();
}


// ��ȡ¼���豸����
DWORD WINAPI LAM_GetRecordingDeviceCount()
{
	return g_audioManager.GetRecordingDeviceCount();
}


// ��ȡ¼���豸����Unicode��
DWORD WINAPI LAM_GetRecordingDeviceName(DWORD dwIndex, LPWSTR lpszName, DWORD dwLength)
{
	UINT count = g_audioManager.GetRecordingDeviceCount();
	if (dwIndex < 0 || dwIndex >= count)
	{
		return 0;
	}

	AUDIO_CONTROL_DEVICE_ENTITY entity = g_audioManager.GetRecordingDevice((UINT)dwIndex);
	if (lpszName == nullptr)
	{
		return entity.strName.GetLength();
	}
	return CopyStringBuffer(entity.strName, entity.strName.GetLength(), lpszName, dwLength);
}


// ��ȡ¼���豸����
DWORD WINAPI LAM_GetRecordingDeviceVolume(DWORD dwIndex)
{
	FLOAT fVolume = g_audioManager.GetRecordingDeviceVolume((UINT)dwIndex);
	return static_cast<DWORD>(std::round(fVolume * 100.00f));
}


// ����¼���豸����
void WINAPI LAM_SetRecordingDeviceVolume(DWORD dwIndex, DWORD dwVolume)
{
	g_audioManager.SetRecordingDeviceVolume((UINT)dwIndex, static_cast<FLOAT>(dwVolume) * 0.01f);
}


// ����¼���豸����
void WINAPI LAM_SetRecordingDeviceMute(DWORD dwIndex, BOOL bMute)
{
	g_audioManager.SetRecordingDeviceMute((UINT)dwIndex, bMute);
}


// ��ȡ¼���豸����״̬
BOOL WINAPI LAM_GetRecordingDeviceMute(DWORD dwIndex)
{
	return g_audioManager.GetRecordingDeviceMute((UINT)dwIndex);
}


// ����Ĭ��¼���豸
void WINAPI LAM_SetDefaultRecordingDevice(DWORD dwIndex)
{
	g_audioManager.SetDefaultRecordingDevice(dwIndex);
}


// ��ȡĬ��¼���豸
DWORD WINAPI LAM_GetDefaultRecordingDevice()
{
	return g_audioManager.GetDefaultRecordingDevice();
}


// ��ȡ�Ự����
DWORD WINAPI LAM_GetSessionCount()
{
	return g_audioManager.GetSessionCount();
}


// ��ȡ�Ự����Unicode��
DWORD WINAPI LAM_GetSessionName(DWORD dwIndex, LPWSTR lpszName, DWORD dwLength)
{
	UINT count = g_audioManager.GetSessionCount();
	if (dwIndex < 0 || dwIndex >= count)
	{
		return 0;
	}

	AUDIO_CONTROL_SESSION_ENTITY entity = g_audioManager.GetSession((UINT)dwIndex);
	if (lpszName == nullptr)
	{
		return entity.strName.GetLength();
	}
	return CopyStringBuffer(entity.strName, entity.strName.GetLength(), lpszName, dwLength);
}


// ��ȡ�Ự����
DWORD WINAPI LAM_GetSessionVolume(DWORD dwIndex)
{
	FLOAT fVolume = g_audioManager.GetSessionVolume((UINT)dwIndex);
	return static_cast<DWORD>(std::round(fVolume * 100.00f));
}


// ���ûỰ����
void WINAPI LAM_SetSessionVolume(DWORD dwIndex, DWORD dwVolume)
{
	g_audioManager.SetSessionVolume((UINT)dwIndex, static_cast<FLOAT>(dwVolume) * 0.01f);
}


// ���ûỰ����
void WINAPI LAM_SetSessionMute(DWORD dwIndex, BOOL bMute)
{
	g_audioManager.SetSessionMute((UINT)dwIndex, bMute);
}


// ��ȡ�Ự����״̬
BOOL WINAPI LAM_GetSessionMute(DWORD dwIndex)		
{
	return g_audioManager.GetSessionMute((UINT)dwIndex);
}


// ��ȡ�Ự�����豸
DWORD WINAPI LAM_GetSessionPlaybackDevice(DWORD dwIndex)
{
	return g_audioManager.GetSessionPlaybackDevice(dwIndex);
}



// ���ûỰ�����豸
void WINAPI LAM_SetSessionPlaybackDevice(DWORD dwSessionIndex, DWORD dwDeviceIndex)
{
	g_audioManager.SetSessionPlaybackDevice(dwSessionIndex, dwDeviceIndex);
}