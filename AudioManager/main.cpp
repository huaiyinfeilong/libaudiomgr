// AudioManager.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "AudioManager.h"

int main()
{
	AudioManager audioManager;
	audioManager.Initialize();
	UINT count = audioManager.GetPlaybackDeviceCount();
	std::wcout << count << "个输出设备：" << std::endl;
	for (UINT i = 0; i < count; i++)
	{
		std::wcout << audioManager.GetPlaybackDevice(i) << std::endl;
	}
	 count = audioManager.GetRecordingDeviceCount();
	std::wcout << count << "个输入设备：" << std::endl;
	for (UINT i = 0; i < count; i++)
	{
		std::wcout << audioManager.GetRecordingDevice(i) << std::endl;
	}
	audioManager.GetChannels();

	return 0;
}
