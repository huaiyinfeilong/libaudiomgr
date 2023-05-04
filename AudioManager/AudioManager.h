#pragma once

#include <atlbase.h>
#include <mmdeviceapi.h>
#include <iostream>
#include<map>


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
	LPCWSTR GetPlaybackDevice(UINT nIndex);
	// 获取录音设备数量
	UINT GetRecordingDeviceCount();
	// 获取录音设备
	LPCWSTR GetRecordingDevice(UINT nIndex);
	// 获取通道
	void GetChannels();

protected:
	// 根据类型获取设备列表
	void GetDevices(EDataFlow dataFlow, std::map<std::wstring, CComPtr<IMMDevice>>& mapDevices);

protected:
	// 输出设备
	std::map<std::wstring, CComPtr<IMMDevice>> _mapPlaybackDevices;
	// 输入设备
	std::map<std::wstring, CComPtr<IMMDevice>> _mapRecordingDevices;
};

