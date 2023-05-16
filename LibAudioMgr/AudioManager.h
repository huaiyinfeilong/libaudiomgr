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
	DWORD GetPlaybackDeviceCount();
	// 获取回放设备
	AUDIO_CONTROL_DEVICE_ENTITY GetPlaybackDevice(DWORD dwIndex);
	// 获取录音设备数量
	DWORD GetRecordingDeviceCount();
	// 获取录音设备
	AUDIO_CONTROL_DEVICE_ENTITY GetRecordingDevice(DWORD dwIndex);
	// 获取会话数量
	DWORD GetSessionCount();
	// 获取会话
	AUDIO_CONTROL_SESSION_ENTITY GetSession(DWORD dwIndex) const;
	// 获取回放设备音量
	FLOAT GetPlaybackDeviceVolume(DWORD dwIndex);
	// 设置回放设备音量
	void SetPlaybackDeviceVolume(DWORD dwIndex, FLOAT fVolume);
	// 静音回放设备
	void SetPlaybackDeviceMute(DWORD dwIndex, BOOL bMute);
	// 获取回放设备静音状态
	BOOL GetPlaybackDeviceMute(DWORD dwIndex);
	// 获取默认播放设备
	DWORD GetDefaultPlaybackDevice();
	// 设置默认播放设备
	void SetDefaultPlaybackDevice(DWORD dwIndex);
	// 获取录音设备音量
	FLOAT GetRecordingDeviceVolume(DWORD dwIndex);
	// 设置录音设备音量
	void SetRecordingDeviceVolume(DWORD dwIndex, FLOAT fVolume);
	// 静音录音设备
	void SetRecordingDeviceMute(DWORD dwIndex, BOOL bMute);
	// 获取录音设备静音状态
	BOOL GetRecordingDeviceMute(DWORD dwIndex);
	// 获取默认录音设备
	DWORD GetDefaultRecordingDevice();
	// 设置默认录音设备
	void SetDefaultRecordingDevice(DWORD dwIndex);
	// 获取会话音量
	FLOAT GetSessionVolume(DWORD dwIndex);
	// 设置会话音量
	void SetSessionVolume(DWORD dwIndex, FLOAT fVolume);
	// 静音会话
	void SetSessionMute(DWORD dwIndex, BOOL bMute);
	// 获取会话静音状态
	BOOL GetSessionMute(DWORD dwIndex);
	// 设置会话播放设备
	void SetSessionPlaybackDevice(DWORD dwSessionIndex, DWORD dwDeviceIndex);
	// 获取会话播放设备
	DWORD GetSessionPlaybackDevice(DWORD dwIndex);
	// 重置所有会话播放设备
	void ResetAllSessionPlaybackDevice();
	// 设置会话录音设备
	void SetSessionRecordingDevice(DWORD dwSessionIndex, DWORD dwDeviceIndex);
	// 获取会话录音设备
	DWORD GetSessionRecordingDevice(DWORD dwIndex);

protected:
	// 根据类型获取设备列表
	void GetDevices(EDataFlow dataFlow, ERole eRole, std::vector<AUDIO_CONTROL_DEVICE_ENTITY>& listDevice);
	// 设置默认播放、录音设备
	void SetDefaultDevice(LPCTSTR lpDeviceId, ERole eRole);
	// 获取默认播放、录音设备
	DWORD GetDefaultDevice(EDataFlow eDataFlow, ERole eRole);
	// 设置会话播放、录音设备
	void SetSessionDevice(DWORD dwIndex, CComPtr<IMMDevice>& spDevice);
	// 获取所有会话
	void GetAllSession();
	// 构建用以音频会话所需的设备ID
	CString GenerateDeviceId(EDataFlow dataFlow, CString strDeviceId);
	// 获取会话名称
	BOOL GetSessionDisplayName(CComPtr<IAudioSessionControl2>& spSession, CString& strDisplayName);

protected:
	// 输出设备
	std::vector<AUDIO_CONTROL_DEVICE_ENTITY> _listPlaybackDevice;
	// 输入设备
	std::vector<AUDIO_CONTROL_DEVICE_ENTITY> _listRecordingDevice;
	// 应用程序会话列表
	std::vector<AUDIO_CONTROL_SESSION_ENTITY> _listSession;
};

