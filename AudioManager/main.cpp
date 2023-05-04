// AudioManager.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "AudioManager.h"

int main()
{
	setlocale(LC_ALL, "");
	AudioManager audioManager;
	audioManager.Initialize();
	UINT count = audioManager.GetSessionCount();
	std::wcout << L"数量：" << count << std::endl;
	for (UINT i = 0; i < count; i++)
	{
		AUDIO_CONTROL_SESSION_ENTITY entity = audioManager.GetSession(i);
		std::wcout << entity.szName << std::endl;
	}

	return 0;
}
