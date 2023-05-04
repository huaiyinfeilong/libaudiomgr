#pragma once

#include <tchar.h>
#include <iostream>
#include<map>
#include <vector>
#include <atlbase.h>
#include <mmdeviceapi.h>
#include <functiondiscoverykeys_devpkey.h>
#include <audiopolicy.h>


// ��Ƶ���ƶ������Ͷ���
template <class _TypeT>
struct AUDIO_CONTROL_ENTITY {
	WCHAR szId[MAX_PATH];
	WCHAR szName[MAX_PATH];
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
	LPCWSTR GetPlaybackDevice(UINT nIndex);
	// ��ȡ¼���豸����
	UINT GetRecordingDeviceCount();
	// ��ȡ¼���豸
	LPCWSTR GetRecordingDevice(UINT nIndex);
	// ��ȡͨ��
	void GetChannels();
	// ��ȡ�Ự����
	UINT GetSessionCount();
	// ��ȡ�Ự
	AUDIO_CONTROL_SESSION_ENTITY GetSession(UINT nIndex) const;

protected:
	// �������ͻ�ȡ�豸�б�
	void GetDevices(EDataFlow dataFlow, std::map<std::wstring, CComPtr<IMMDevice>>& mapDevices);
	// ��ȡ���лỰ
	void GetAllSession();

protected:
	// ����豸
	std::map<std::wstring, CComPtr<IMMDevice>> _mapPlaybackDevices;
	// �����豸
	std::map<std::wstring, CComPtr<IMMDevice>> _mapRecordingDevices;
	// Ӧ�ó���Ự�б�
	std::vector<AUDIO_CONTROL_SESSION_ENTITY> _listSession;
};

