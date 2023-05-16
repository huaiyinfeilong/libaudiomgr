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
	DWORD GetPlaybackDeviceCount();
	// ��ȡ�ط��豸
	AUDIO_CONTROL_DEVICE_ENTITY GetPlaybackDevice(DWORD dwIndex);
	// ��ȡ¼���豸����
	DWORD GetRecordingDeviceCount();
	// ��ȡ¼���豸
	AUDIO_CONTROL_DEVICE_ENTITY GetRecordingDevice(DWORD dwIndex);
	// ��ȡ�Ự����
	DWORD GetSessionCount();
	// ��ȡ�Ự
	AUDIO_CONTROL_SESSION_ENTITY GetSession(DWORD dwIndex) const;
	// ��ȡ�ط��豸����
	FLOAT GetPlaybackDeviceVolume(DWORD dwIndex);
	// ���ûط��豸����
	void SetPlaybackDeviceVolume(DWORD dwIndex, FLOAT fVolume);
	// �����ط��豸
	void SetPlaybackDeviceMute(DWORD dwIndex, BOOL bMute);
	// ��ȡ�ط��豸����״̬
	BOOL GetPlaybackDeviceMute(DWORD dwIndex);
	// ��ȡĬ�ϲ����豸
	DWORD GetDefaultPlaybackDevice();
	// ����Ĭ�ϲ����豸
	void SetDefaultPlaybackDevice(DWORD dwIndex);
	// ��ȡ¼���豸����
	FLOAT GetRecordingDeviceVolume(DWORD dwIndex);
	// ����¼���豸����
	void SetRecordingDeviceVolume(DWORD dwIndex, FLOAT fVolume);
	// ����¼���豸
	void SetRecordingDeviceMute(DWORD dwIndex, BOOL bMute);
	// ��ȡ¼���豸����״̬
	BOOL GetRecordingDeviceMute(DWORD dwIndex);
	// ��ȡĬ��¼���豸
	DWORD GetDefaultRecordingDevice();
	// ����Ĭ��¼���豸
	void SetDefaultRecordingDevice(DWORD dwIndex);
	// ��ȡ�Ự����
	FLOAT GetSessionVolume(DWORD dwIndex);
	// ���ûỰ����
	void SetSessionVolume(DWORD dwIndex, FLOAT fVolume);
	// �����Ự
	void SetSessionMute(DWORD dwIndex, BOOL bMute);
	// ��ȡ�Ự����״̬
	BOOL GetSessionMute(DWORD dwIndex);
	// ���ûỰ�����豸
	void SetSessionPlaybackDevice(DWORD dwSessionIndex, DWORD dwDeviceIndex);
	// ��ȡ�Ự�����豸
	DWORD GetSessionPlaybackDevice(DWORD dwIndex);
	// �������лỰ�����豸
	void ResetAllSessionPlaybackDevice();
	// ���ûỰ¼���豸
	void SetSessionRecordingDevice(DWORD dwSessionIndex, DWORD dwDeviceIndex);
	// ��ȡ�Ự¼���豸
	DWORD GetSessionRecordingDevice(DWORD dwIndex);

protected:
	// �������ͻ�ȡ�豸�б�
	void GetDevices(EDataFlow dataFlow, ERole eRole, std::vector<AUDIO_CONTROL_DEVICE_ENTITY>& listDevice);
	// ����Ĭ�ϲ��š�¼���豸
	void SetDefaultDevice(LPCTSTR lpDeviceId, ERole eRole);
	// ��ȡĬ�ϲ��š�¼���豸
	DWORD GetDefaultDevice(EDataFlow eDataFlow, ERole eRole);
	// ���ûỰ���š�¼���豸
	void SetSessionDevice(DWORD dwIndex, CComPtr<IMMDevice>& spDevice);
	// ��ȡ���лỰ
	void GetAllSession();
	// ����������Ƶ�Ự������豸ID
	CString GenerateDeviceId(EDataFlow dataFlow, CString strDeviceId);
	// ��ȡ�Ự����
	BOOL GetSessionDisplayName(CComPtr<IAudioSessionControl2>& spSession, CString& strDisplayName);

protected:
	// ����豸
	std::vector<AUDIO_CONTROL_DEVICE_ENTITY> _listPlaybackDevice;
	// �����豸
	std::vector<AUDIO_CONTROL_DEVICE_ENTITY> _listRecordingDevice;
	// Ӧ�ó���Ự�б�
	std::vector<AUDIO_CONTROL_SESSION_ENTITY> _listSession;
};

