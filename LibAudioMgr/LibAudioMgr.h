#pragma once

#include <Windows.h>

#ifdef LIBAUDIOMGRAPI
#undef LIBAUDIOMGRAPI
#define LIBAUDIOMGRAPI extern "C" __declspec(dllexport)
#else
#define LIBAUDIOMGRAPI extern "C" __declspec(dllimport)
#endif


// ��ʼ��
LIBAUDIOMGRAPI
void
WINAPI
LAM_Initialize(
);


// ���ʼ��
LIBAUDIOMGRAPI
void
WINAPI
LAM_Uninitialize(
);


// ��ȡ�����豸����
LIBAUDIOMGRAPI
DWORD
WINAPI
LAM_GetPlaybackDeviceCount(
);


// ��ȡ�����豸����
LIBAUDIOMGRAPI
DWORD
WINAPI
LAM_GetPlaybackDeviceName(
	DWORD dwIndex,
	LPWSTR lpszName,
	DWORD dwLength
);


// ��ȡ�����豸����
LIBAUDIOMGRAPI
DWORD
WINAPI
LAM_GetPlaybackDeviceVolume(
	DWORD dwIndex
);


// ���ò����豸����
LIBAUDIOMGRAPI
void
WINAPI
LAM_SetPlaybackDeviceVolume(
	DWORD dwIndex,
	DWORD dwVolume
);


// ���������豸
LIBAUDIOMGRAPI
void
WINAPI
LAM_SetPlaybackDeviceMute(
	DWORD dwIndex,
	BOOL bMute
);


// ��ȡ�����豸����״̬
LIBAUDIOMGRAPI
BOOL
WINAPI
LAM_GetPlaybackDeviceMute(
	DWORD dwIndex
);


// ��ȡĬ�ϲ����豸
LIBAUDIOMGRAPI
DWORD
WINAPI
LAM_GetDefaultPlaybackDevice(
);


// ����Ĭ�ϲ����豸
LIBAUDIOMGRAPI
void
WINAPI
LAM_SetDefaultPlaybackDevice(
	DWORD dwIndex
);


// ��ȡ¼���豸����
LIBAUDIOMGRAPI
DWORD
WINAPI
LAM_GetRecordingDeviceCount(
);


// ��ȡ¼���豸����
LIBAUDIOMGRAPI
DWORD
WINAPI
LAM_GetRecordingDeviceName(
	DWORD dwIndex,
	LPWSTR lpszName,
	DWORD dwLength
);


// ��ȡ¼���豸����
LIBAUDIOMGRAPI
DWORD
WINAPI
LAM_GetRecordingDeviceVolume(
	DWORD dwIndex
);


// ����¼���豸����
LIBAUDIOMGRAPI
void
WINAPI
LAM_SetRecordingDeviceVolume(
	DWORD dwIndex,
	DWORD dwVolume
);


// ����¼���豸
LIBAUDIOMGRAPI
void
WINAPI
LAM_SetRecordingDeviceMute(
	DWORD dwIndex,
	BOOL bMute
);


// ��ȡ¼���豸����״̬
LIBAUDIOMGRAPI
BOOL
WINAPI
LAM_GetRecordingDeviceMute(
	DWORD dwIndex
);


// ��ȡĬ��¼���豸
LIBAUDIOMGRAPI
DWORD
WINAPI
LAM_GetDefaultRecordingDevice(
);


// ����Ĭ��¼���豸
LIBAUDIOMGRAPI
void
WINAPI
LAM_SetDefaultRecordingDevice(
	DWORD dwIndex
);


// ��ȡ�Ự����
LIBAUDIOMGRAPI
DWORD
WINAPI
LAM_GetSessionCount(
);


// ��ȡ�Ự����
LIBAUDIOMGRAPI
DWORD
WINAPI
LAM_GetSessionName(
	DWORD dwIndex,
	LPWSTR lpszName,
	DWORD dwLength
);


// ��ȡ�Ự����
LIBAUDIOMGRAPI
DWORD
WINAPI
LAM_GetSessionVolume(
	DWORD dwIndex
);


// ���ûỰ����
LIBAUDIOMGRAPI
void
WINAPI
LAM_SetSessionVolume(
	DWORD dwIndex,
	DWORD dwVolume
);


// �����Ự
LIBAUDIOMGRAPI
void
WINAPI
LAM_SetSessionMute(
	DWORD dwIndex,
	BOOL bMute
);


// ��ȡ�Ự����״̬
LIBAUDIOMGRAPI
BOOL
WINAPI
LAM_GetSessionMute(
	DWORD dwIndex
);


// ��ȡ�Ự�����豸
LIBAUDIOMGRAPI
DWORD
WINAPI
LAM_GetSessionPlaybackDevice(
	DWORD dwIndex
);


// ���ûỰ�����豸
LIBAUDIOMGRAPI
void
WINAPI
LAM_SetSessionPlaybackDevice(
	DWORD dwSessionIndex,
	DWORD dwDeviceIndex
);
