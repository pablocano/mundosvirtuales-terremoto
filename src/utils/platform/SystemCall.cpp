#include "SystemCall.h"
#include "../logger/Logger.h"

#include <thread>

#include <iostream>
#ifndef WIN32
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#else

#ifdef NOMINMAX
#include <windows.h>
#else
#define NOMINMAX
#include <windows.h>
#undef NOMINMAX
#endif

#endif
#include <cstring>

#ifdef WIN32
#else
#ifdef MACOSX
#include <mach/mach_time.h>
#else
#include <sys/sysinfo.h>
#endif
#include <time.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif

unsigned SystemCall::getCurrentSystemTime()
{
	return getRealSystemTime();
}

unsigned SystemCall::getRealSystemTime()
{
#ifdef WIN32
	unsigned time = timeGetTime();
#elif defined(MACOSX)
	static mach_timebase_info_data_t info = { 0, 0 };
	if (info.denom == 0)
		mach_timebase_info(&info);
	unsigned int time = unsigned(mach_absolute_time() * (info.numer / info.denom) / 1000000);
#else
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	unsigned int time = (unsigned int)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000l);
#endif
	static unsigned base = 0;
	if (!base)
		base = time - 10000; // avoid time == 0, because it is often used as a marker
	return time - base;
}

unsigned long long SystemCall::getCurrentThreadTime()
{
#if defined(WIN32) || defined(MACOSX) // FIXME
	return (unsigned long long) getRealSystemTime() * 1000;
#else
	clockid_t cid;
	struct timespec ts;

	if (pthread_getcpuclockid(pthread_self(), &cid) == 0)
		LOGGER_ERROR("Response Packet Client", "Problem pthread get CPU");
	if (clock_gettime(cid, &ts) == 0)
		LOGGER_ERROR("Response Packet Client", "Problem get time");

	unsigned long long time = ts.tv_sec * 1000000ll + ts.tv_nsec / 1000;

	static unsigned long long base = 0;
	if (!base)
		base = time - 1000000;
	return time - base;
#endif
}

const char* SystemCall::getHostName()
{
	static const char* hostname = 0;
	if (!hostname)
	{
		static char buf[100] = { 0 };
		if (!gethostname(buf, sizeof(buf)))
			LOGGER_ERROR("Response Packet Client", "Problem get buffer");
		hostname = buf;
	}
	return hostname;
}

const char* SystemCall::getHostAddr()
{
	static const char* hostaddr = 0;
	if (!hostaddr)
	{
		static char buf[100];
		hostent* hostAddr = (hostent*)gethostbyname(getHostName());
		if (hostAddr && *hostAddr->h_addr_list)
		{
			char* ipHost = inet_ntoa(*(in_addr*)*hostAddr->h_addr_list);
			strcpy_s(buf, strlen(ipHost) + 1, ipHost);
		}
		else
		{
			const char* ipLocalhost = "127.0.0.1";
			strcpy_s(buf, sizeof(ipLocalhost), ipLocalhost);
		}
		hostaddr = buf;
	}
	return hostaddr;
}

unsigned int SystemCall::getHostAddrInt()
{
	unsigned int hostaddr = 0;
	hostent* hostAddr = (hostent*)gethostbyname(getHostName());

	if (hostAddr && *hostAddr->h_addr_list)
		hostaddr = (unsigned int)ntohl((*(in_addr*)*hostAddr->h_addr_list).s_addr);

	return hostaddr;
}

const char* SystemCall::getAddr(long n) {
	sockaddr_in saddr;
	saddr.sin_addr.s_addr = ntohl(n);
	return inet_ntoa(saddr.sin_addr);
}

void SystemCall::sleep(unsigned int ms)
{
#ifdef WIN32
	Sleep(ms);
#else
	usleep(ms * 1000);
#endif
}

void SystemCall::getLoad(float& mem, float load[3])
{
#ifdef WIN32
	load[0] = load[1] = load[2] = -1.f; //Not implemented yet
	MEMORYSTATUSEX memStat;
	memStat.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memStat);
	mem = float(memStat.dwMemoryLoad) / 100.f;
#elif defined(MACOSX) // FIXME
	mem = -1.f;
	load[0] = load[1] = load[2] = -1.f;
#else
	struct sysinfo info;
	if (sysinfo(&info) == -1)
		load[0] = load[1] = load[2] = mem = -1.f;
	else
	{
		load[0] = float(info.loads[0]) / 65536.f;
		load[1] = float(info.loads[1]) / 65536.f;
		load[2] = float(info.loads[2]) / 65536.f;
		mem = float(info.totalram - info.freeram) / float(info.totalram);
	}
#endif
}

void SystemCall::killThread(void* lpThread, int exitCode)
{
#ifdef WIN32
	::TerminateThread(lpThread, exitCode);
	::CloseHandle(lpThread);
#else
	pthread_cancel(lpThread);
#endif
}

#ifdef WIN32
unsigned long __stdcall SystemCall::wrapperFunThreadWin(void* fun)
{
	((FN_CALLBACK)fun)();
	return 0;
}
#endif

bool SystemCall::createThread(void* lpThread, FN_CALLBACK fun_callback)
{
#ifdef WIN32
	unsigned long threadID;
	lpThread = ::CreateThread(nullptr, 0, &SystemCall::wrapperFunThreadWin, fun_callback, 0, &threadID);
	return lpThread != nullptr;
#else
	int err = pthread_create(lpThread, nullptr, fun_callback, nullptr);
	return err == 0;
#endif
}

void SystemCall::joinThread(void* lpThread)
{
#ifdef WIN32
	WaitForSingleObject(lpThread, false);
#else
	pthread_join(lpThread, nullptr);
#endif
}