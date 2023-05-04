#pragma once

#include <atlbase.h>
#include <mmdeviceapi.h>
#include <iostream>
#include<map>


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

protected:
	// �������ͻ�ȡ�豸�б�
	void GetDevices(EDataFlow dataFlow, std::map<std::wstring, CComPtr<IMMDevice>>& mapDevices);

protected:
	// ����豸
	std::map<std::wstring, CComPtr<IMMDevice>> _mapPlaybackDevices;
	// �����豸
	std::map<std::wstring, CComPtr<IMMDevice>> _mapRecordingDevices;
};

