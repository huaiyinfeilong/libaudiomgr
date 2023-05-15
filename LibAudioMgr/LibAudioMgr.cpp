#include "pch.h"

#define LIBAUDIOMGRAPI

#include "LibAudioMgr.h"
#include "AudioManager.h"
#include <math.h>


AudioManager g_audioManager;

DWORD CopyStringBuffer(LPCWSTR src, DWORD srcLen, LPWSTR dst, DWORD dstLen)
{
	// 获取需要的缓冲区字符数量
	DWORD len = (dstLen > srcLen) ? srcLen : dstLen - 1;
	// 缓冲区可容纳的字符数量必须大于1
	if (len < 1)
	{
		return 0;
	}
	// 拷贝数据
	_tcsncpy_s(dst, dstLen, src, len);
	dst[len] = 0;
	return len;
}


// 初始化
void WINAPI LAM_Initialize()
{
	g_audioManager.Initialize();
}


// 逆初始化
void WINAPI LAM_Uninitialize()
{
	g_audioManager.Uninitialize();
}


// 获取播放设备数量
DWORD WINAPI LAM_GetPlaybackDeviceCount()
{
	return g_audioManager.GetPlaybackDeviceCount();
}


// 获取播放设备名称Unicode版
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


// 获取播放设备音量
DWORD WINAPI LAM_GetPlaybackDeviceVolume(DWORD dwIndex)
{
	FLOAT fVolume = g_audioManager.GetPlaybackDeviceVolume((UINT)dwIndex);
	return static_cast<DWORD>(std::round(fVolume * 100.00f));
}


// 设置播放设备音量
void WINAPI LAM_SetPlaybackDeviceVolume(DWORD dwIndex, DWORD dwVolume)
{
	g_audioManager.SetPlaybackDeviceVolume((UINT)dwIndex, static_cast<FLOAT>(dwVolume) * 0.01f);
}


// 设置播放设备静音
void WINAPI LAM_SetPlaybackDeviceMute(DWORD dwIndex, BOOL bMute)
{
	g_audioManager.SetPlaybackDeviceMute((UINT)dwIndex, bMute);
}


// 获取播放设备静音状态
BOOL WINAPI LAM_GetPlaybackDeviceMute(DWORD dwIndex)
{
	return g_audioManager.GetPlaybackDeviceMute((UINT)dwIndex);
}


// 设置默认播放设备
void WINAPI LAM_SetDefaultPlaybackDevice(DWORD dwIndex)
{
	g_audioManager.SetDefaultPlaybackDevice(dwIndex);
}


// 获取默认播放设备
DWORD WINAPI LAM_GetDefaultPlaybackDevice()
{
	return g_audioManager.GetDefaultPlaybackDevice();
}


// 获取录音设备数量
DWORD WINAPI LAM_GetRecordingDeviceCount()
{
	return g_audioManager.GetRecordingDeviceCount();
}


// 获取录音设备名称Unicode版
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


// 获取录音设备音量
DWORD WINAPI LAM_GetRecordingDeviceVolume(DWORD dwIndex)
{
	FLOAT fVolume = g_audioManager.GetRecordingDeviceVolume((UINT)dwIndex);
	return static_cast<DWORD>(std::round(fVolume * 100.00f));
}


// 设置录音设备音量
void WINAPI LAM_SetRecordingDeviceVolume(DWORD dwIndex, DWORD dwVolume)
{
	g_audioManager.SetRecordingDeviceVolume((UINT)dwIndex, static_cast<FLOAT>(dwVolume) * 0.01f);
}


// 设置录音设备静音
void WINAPI LAM_SetRecordingDeviceMute(DWORD dwIndex, BOOL bMute)
{
	g_audioManager.SetRecordingDeviceMute((UINT)dwIndex, bMute);
}


// 获取录音设备静音状态
BOOL WINAPI LAM_GetRecordingDeviceMute(DWORD dwIndex)
{
	return g_audioManager.GetRecordingDeviceMute((UINT)dwIndex);
}


// 设置默认录音设备
void WINAPI LAM_SetDefaultRecordingDevice(DWORD dwIndex)
{
	g_audioManager.SetDefaultRecordingDevice(dwIndex);
}


// 获取默认录音设备
DWORD WINAPI LAM_GetDefaultRecordingDevice()
{
	return g_audioManager.GetDefaultRecordingDevice();
}


// 获取会话数量
DWORD WINAPI LAM_GetSessionCount()
{
	return g_audioManager.GetSessionCount();
}


// 获取会话名称Unicode版
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


// 获取会话音量
DWORD WINAPI LAM_GetSessionVolume(DWORD dwIndex)
{
	FLOAT fVolume = g_audioManager.GetSessionVolume((UINT)dwIndex);
	return static_cast<DWORD>(std::round(fVolume * 100.00f));
}


// 设置会话音量
void WINAPI LAM_SetSessionVolume(DWORD dwIndex, DWORD dwVolume)
{
	g_audioManager.SetSessionVolume((UINT)dwIndex, static_cast<FLOAT>(dwVolume) * 0.01f);
}


// 设置会话静音
void WINAPI LAM_SetSessionMute(DWORD dwIndex, BOOL bMute)
{
	g_audioManager.SetSessionMute((UINT)dwIndex, bMute);
}


// 获取会话静音状态
BOOL WINAPI LAM_GetSessionMute(DWORD dwIndex)		
{
	return g_audioManager.GetSessionMute((UINT)dwIndex);
}


// 获取会话播放设备
DWORD WINAPI LAM_GetSessionPlaybackDevice(DWORD dwIndex)
{
	return g_audioManager.GetSessionPlaybackDevice(dwIndex);
}



// 设置会话播放设备
void WINAPI LAM_SetSessionPlaybackDevice(DWORD dwSessionIndex, DWORD dwDeviceIndex)
{
	g_audioManager.SetSessionPlaybackDevice(dwSessionIndex, dwDeviceIndex);
}