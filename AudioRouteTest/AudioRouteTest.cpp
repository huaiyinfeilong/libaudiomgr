// AudioRouteTest.cpp
// 音频路由测试
//

#include <iostream>
#include <atlbase.h>
#include <log4cxx/logger.h>
#include <Windows.h>

log4cxx::LoggerPtr logger = log4cxx::Logger::getRootLogger();


int main()
{
	// COM线程初始化
	HRESULT hr;
	hr = CoInitialize(nullptr);
	if (FAILED(hr))
	{
		LOG4CXX_ERROR(logger, _T("CoInitialize()初始化失败，错误码：") << GetLastError() << _T("，程序退出。"));
		return 0;
	}
	// COM线程初始化成功，开始执行业务
	LOG4CXX_INFO(logger, _T("初始化成功。"));
	// 业务执行完成，COM线程清理
	CoUninitialize();
	LOG4CXX_INFO(logger, _T("逆初始化完成，程序退出。"));
}
