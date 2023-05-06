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


// 音频控制对象类型定义
template <class _TypeT>
struct AUDIO_CONTROL_ENTITY {
	CString strId;
	CString strName;
	_TypeT spObject;
};

// 会话对象类型定义
typedef AUDIO_CONTROL_ENTITY<CComPtr<IAudioSessionControl>> AUDIO_CONTROL_SESSION_ENTITY;
// 设备对象类型定义
typedef AUDIO_CONTROL_ENTITY<CComPtr<IMMDevice>> AUDIO_CONTROL_DEVICE_ENTITY;


// AudioManager 音频管理器类声明
class AudioManager
{
public:
	// 初始化
	void Initialize();
	// 逆初始化
	void Uninitialize();
	// 获取回放设备数量
	UINT GetPlaybackDeviceCount();
	// 获取回放设备
	AUDIO_CONTROL_DEVICE_ENTITY GetPlaybackDevice(UINT nIndex);
	// 获取录音设备数量
	UINT GetRecordingDeviceCount();
	// 获取录音设备
	AUDIO_CONTROL_DEVICE_ENTITY GetRecordingDevice(UINT nIndex);
	// 获取会话数量
	UINT GetSessionCount();
	// 获取会话
	AUDIO_CONTROL_SESSION_ENTITY GetSession(UINT nIndex) const;
	// 获取回放设备音量
	FLOAT GetPlaybackDeviceVolume(UINT nIndex);
	// 设置回放设备音量
	void SetPlaybackDeviceVolume(UINT nIndex, FLOAT fVolume);
	// 获取录音设备音量
	FLOAT GetRecordingDeviceVolume(UINT nIndex);
	// 设置录音设备音量
	void SetRecordingDeviceVolume(UINT nIndex, FLOAT fVolume);
	// 获取会话音量
	FLOAT GetSessionVolume(UINT nIndex);
	// 设置会话音量
	void SetSessionVolume(UINT nIndex, FLOAT fVolume);

protected:
	// 根据类型获取设备列表
	void GetDevices(EDataFlow dataFlow, DWORD dwStateMask, std::vector<AUDIO_CONTROL_DEVICE_ENTITY>& listDevice);
	// 获取所有会话
	void GetAllSession();
	// 获取所有顶级窗口
	BOOL GetWindows(std::map<DWORD, CString>& context);
	// 获取会话名称
	void GetSessionDisplayName(CComPtr<IAudioSessionControl2>& spSession, CString& strDisplayName);

protected:
	// 输出设备
	std::vector<AUDIO_CONTROL_DEVICE_ENTITY> _listPlaybackDevice;
	// 输入设备
	std::vector<AUDIO_CONTROL_DEVICE_ENTITY> _listRecordingDevice;
	// 应用程序会话列表
	std::vector<AUDIO_CONTROL_SESSION_ENTITY> _listSession;
};

