#pragma once

#include <tchar.h>
#include <iostream>
#include<map>
#include <vector>
#include <atlbase.h>
#include <atlstr.h>
#include <mmdeviceapi.h>
#include <functiondiscoverykeys_devpkey.h>
#include <audiopolicy.h>


// ��Ƶ���ƶ������Ͷ���
template <class _TypeT>
struct AUDIO_CONTROL_ENTITY {
	CString strId;
	CString strName;
	_TypeT spObject;
};

// �Ự�������Ͷ���
typedef AUDIO_CONTROL_ENTITY<CComPtr<IAudioSessionControl>> AUDIO_CONTROL_SESSION_ENTITY;
// �豸�������Ͷ���
typedef AUDIO_CONTROL_ENTITY<CComPtr<IMMDevice>> AUDIO_CONTROL_DEVICE_ENTITY;


// AudioManager ��Ƶ������������
class AudioManager
{
public:
	// ��ʼ��
	void Initialize();
	// ���ʼ��
	void Uninitialize();
	// ��ȡ�ط��豸����
	UINT GetPlaybackDeviceCount();
	// ��ȡ�ط��豸
	AUDIO_CONTROL_DEVICE_ENTITY GetPlaybackDevice(UINT nIndex);
	// ��ȡ¼���豸����
	UINT GetRecordingDeviceCount();
	// ��ȡ¼���豸
	AUDIO_CONTROL_DEVICE_ENTITY GetRecordingDevice(UINT nIndex);
	// ��ȡ�Ự����
	UINT GetSessionCount();
	// ��ȡ�Ự
	AUDIO_CONTROL_SESSION_ENTITY GetSession(UINT nIndex) const;
	// ��ȡ�ط��豸����
	FLOAT GetPlaybackDeviceVolume(UINT nIndex);
	// ���ûط��豸����
	void SetPlaybackDeviceVolume(UINT nIndex, FLOAT fVolume);
	// ��ȡ¼���豸����
	FLOAT GetRecordingDeviceVolume(UINT nIndex);
	// ����¼���豸����
	void SetRecordingDeviceVolume(UINT nIndex, FLOAT fVolume);
	// ��ȡ�Ự����
	FLOAT GetSessionVolume(UINT nIndex);
	// ���ûỰ����
	void SetSessionVolume(UINT nIndex, FLOAT fVolume);

protected:
	// �������ͻ�ȡ�豸�б�
	void GetDevices(EDataFlow dataFlow, DWORD dwStateMask, std::vector<AUDIO_CONTROL_DEVICE_ENTITY>& listDevice);
	// ��ȡ���лỰ
	void GetAllSession();
	// ��ȡ���ж�������
	BOOL GetWindows(std::map<DWORD, CString>& context);
	// ��ȡ�Ự����
	void GetSessionDisplayName(CComPtr<IAudioSessionControl2>& spSession, CString& strDisplayName);

protected:
	// ����豸
	std::vector<AUDIO_CONTROL_DEVICE_ENTITY> _listPlaybackDevice;
	// �����豸
	std::vector<AUDIO_CONTROL_DEVICE_ENTITY> _listRecordingDevice;
	// Ӧ�ó���Ự�б�
	std::vector<AUDIO_CONTROL_SESSION_ENTITY> _listSession;
};

