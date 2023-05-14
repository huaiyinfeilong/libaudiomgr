// AudioRouteTest.cpp
	// 音频路由测试
//

#include <iostream>
#include <atlbase.h>
#include <log4cxx/logger.h>
#include <Windows.h>
#include <winrt/base.h>
#include <winrt/windows.foundation.h>
#include <winstring.h>
#include <roapi.h>
#include "AudioPolicyConfig.h"


log4cxx::LoggerPtr logger = log4cxx::Logger::getRootLogger();


int main()
{
	// COM线程初始化
	RoInitialize(RO_INIT_MULTITHREADED);
	// COM线程初始化成功，开始执行业务
	LOG4CXX_INFO(logger, _T("初始化成功。"));
	winrt::com_ptr<IAudioPolicyConfig> factory;
	HSTRING clsid;
	const wchar_t* szClsid = L"Windows.Media.Internal.AudioPolicyConfig";
	HRESULT hr = WindowsCreateString(szClsid, wcslen(szClsid), &clsid);
	if (FAILED(hr))
	{
		LOG4CXX_ERROR(logger, _T("执行WindowsCreateString()失败。错误码：0x") << std::hex << hr);
		RoUninitialize();
		return 0;
	}
	hr = RoGetActivationFactory(clsid, winrt::guid_of<IActivationFactory>(), factory.put_void());
	if (FAILED(hr))
	{
		LOG4CXX_ERROR(logger, _T("执行RoGetActivationFactory()失败。错误码：0x") << std::hex << hr);
		WindowsDeleteString(clsid);
		RoUninitialize();
		return 0;
	}
	winrt::com_ptr<IAudioPolicyConfig> config;
	hr = factory->QueryInterface(__uuidof(IAudioPolicyConfig), config.put_void());
	if (FAILED(hr))
	{
		LOG4CXX_ERROR(logger, _T("执行ActivateInstance()失败。错误码：0x") << std::hex << hr);
		WindowsDeleteString(clsid);
		RoUninitialize();
		return 0;
	}
	try
	{
		if (config == nullptr)
		{
			LOG4CXX_ERROR(logger, _T("IAudioPolicyConfig为空。"));
			WindowsDeleteString(clsid);
			return 0;
		}
		hr = (reinterpret_cast<IAudioPolicyConfig*>(((DWORD_PTR)&config) + 27 * 4))->ClearAllPersistedApplicationDefaultEndpoints();
	}
	catch (std::exception& e)
	{
		LOG4CXX_ERROR(logger, e.what());
	}
	if (FAILED(hr))
	{
		LOG4CXX_ERROR(logger, _T("执行ActivateInstance()失败。错误码：0x") << std::hex << hr);
		WindowsDeleteString(clsid);
		RoUninitialize();
		return 0;
	}
	// 业务执行完成，COM线程清理
	RoUninitialize();
	LOG4CXX_INFO(logger, _T("逆初始化完成，程序退出。"));
}
