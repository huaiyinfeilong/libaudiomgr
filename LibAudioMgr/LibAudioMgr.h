#pragma once

#include <Windows.h>

#ifdef LIBAUDIOMGRAPI
#undef LIBAUDIOMGRAPI
#define LIBAUDIOMGRAPI extern "C" __declspec(dllexport)
#else
#define LIBAUDIOMGRAPI extern "C" __declspec(dllimport)
#endif


// 初始化
LIBAUDIOMGRAPI
void
WINAPI
LAM_Initialize(
);


// 逆初始化
LIBAUDIOMGRAPI
void
WINAPI
LAM_Uninitialize(
);


// 获取播放设备数量
LIBAUDIOMGRAPI
DWORD
WINAPI
LAM_GetPlaybackDeviceCount(
);


// 获取播放设备名称
LIBAUDIOMGRAPI
DWORD
WINAPI
LAM_GetPlaybackDeviceName(
	DWORD dwIndex,
	LPWSTR lpszName,
	DWORD dwLength
);


// 获取播放设备音量
LIBAUDIOMGRAPI
DWORD
WINAPI
LAM_GetPlaybackDeviceVolume(
	DWORD dwIndex
);


// 设置播放设备音量
LIBAUDIOMGRAPI
void
WINAPI
LAM_SetPlaybackDeviceVolume(
	DWORD dwIndex,
	DWORD dwVolume
);


// 静音播放设备
LIBAUDIOMGRAPI
void
WINAPI
LAM_SetPlaybackDeviceMute(
	DWORD dwIndex,
	BOOL bMute
);


// 获取播放设备静音状态
LIBAUDIOMGRAPI
BOOL
WINAPI
LAM_GetPlaybackDeviceMute(
	DWORD dwIndex
);


// 获取默认播放设备
LIBAUDIOMGRAPI
DWORD
WINAPI
LAM_GetDefaultPlaybackDevice(
);


// 设置默认播放设备
LIBAUDIOMGRAPI
void
WINAPI
LAM_SetDefaultPlaybackDevice(
	DWORD dwIndex
);


// 获取录音设备数量
LIBAUDIOMGRAPI
DWORD
WINAPI
LAM_GetRecordingDeviceCount(
);


// 获取录音设备名称
LIBAUDIOMGRAPI
DWORD
WINAPI
LAM_GetRecordingDeviceName(
	DWORD dwIndex,
	LPWSTR lpszName,
	DWORD dwLength
);


// 获取录音设备音量
LIBAUDIOMGRAPI
DWORD
WINAPI
LAM_GetRecordingDeviceVolume(
	DWORD dwIndex
);


// 设置录音设备音量
LIBAUDIOMGRAPI
void
WINAPI
LAM_SetRecordingDeviceVolume(
	DWORD dwIndex,
	DWORD dwVolume
);


// 静音录音设备
LIBAUDIOMGRAPI
void
WINAPI
LAM_SetRecordingDeviceMute(
	DWORD dwIndex,
	BOOL bMute
);


// 获取录音设备静音状态
LIBAUDIOMGRAPI
BOOL
WINAPI
LAM_GetRecordingDeviceMute(
	DWORD dwIndex
);


// 获取默认录音设备
LIBAUDIOMGRAPI
DWORD
WINAPI
LAM_GetDefaultRecordingDevice(
);


// 设置默认录音设备
LIBAUDIOMGRAPI
void
WINAPI
LAM_SetDefaultRecordingDevice(
	DWORD dwIndex
);


// 获取会话数量
LIBAUDIOMGRAPI
DWORD
WINAPI
LAM_GetSessionCount(
);


// 获取会话名称
LIBAUDIOMGRAPI
DWORD
WINAPI
LAM_GetSessionName(
	DWORD dwIndex,
	LPWSTR lpszName,
	DWORD dwLength
);


// 获取会话音量
LIBAUDIOMGRAPI
DWORD
WINAPI
LAM_GetSessionVolume(
	DWORD dwIndex
);


// 设置会话音量
LIBAUDIOMGRAPI
void
WINAPI
LAM_SetSessionVolume(
	DWORD dwIndex,
	DWORD dwVolume
);


// 静音会话
LIBAUDIOMGRAPI
void
WINAPI
LAM_SetSessionMute(
	DWORD dwIndex,
	BOOL bMute
);


// 获取会话静音状态
LIBAUDIOMGRAPI
BOOL
WINAPI
LAM_GetSessionMute(
	DWORD dwIndex
);


// 获取会话播放设备
LIBAUDIOMGRAPI
DWORD
WINAPI
LAM_GetSessionPlaybackDevice(
	DWORD dwIndex
);


// 设置会话播放设备
LIBAUDIOMGRAPI
void
WINAPI
LAM_SetSessionPlaybackDevice(
	DWORD dwSessionIndex,
	DWORD dwDeviceIndex
);
