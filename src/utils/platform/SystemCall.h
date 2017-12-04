#pragma once

#include "../util.h"

/// <summary>
/// Class with multiplatform function.
/// </summary>
class SystemCall
{
public:

	/// <summary>
	/// Returns the current system time in milliseconds.
	/// </summary>
	/// <returns>Returns the current system time in milliseconds.</returns>
	static unsigned getCurrentSystemTime();

	/// <summary>
	/// Returns the real system time in milliseconds.
	/// </summary>
	/// <returns>Returns the real system time in milliseconds (never the simulated one).</returns>
	static unsigned getRealSystemTime();

	/// <summary>
	/// The function returns the thread cpu time of the calling thread in microseconds.
	/// </summary>
	/// <returns>Returns thread cpu time of the calling thread.</returns>
	static unsigned long long getCurrentThreadTime();

	/// <summary>
	/// Gets the time since aTime argument.
	/// </summary>
	/// <param name="aTime">Initial time.</param>
	/// <returns>Returns the time since aTime</returns>
	inline static int getTimeSince(unsigned aTime)
	{
		return (int)(getCurrentSystemTime() - aTime);
	}

	/// <summary>
	/// Gets the real time since aTime argument.
	/// </summary>
	/// <param name="aTime">Initial time.</param>
	/// <returns>Returns the real time since aTime.</returns>
	inline static int getRealTimeSince(unsigned aTime)
	{
		return (int)(getRealSystemTime() - aTime);
	}

	/// <summary>
	/// Gets DNS of local machine.
	/// </summary>
	/// <returns>returns the name of the local machine</returns>
	static const char* getHostName();

	/** returns the first ip m_address of the local machine*/
	static const char* getHostAddr();

	/// <summary>
	/// Gets the first ip m_address of the local machine.
	/// </summary>
	/// <returns>returns the first ip m_address of the local machine (unsigned int)</returns>
	static unsigned int getHostAddrInt();

	/// <summary>
	/// Returns ip from m_address.
	/// </summary>
	/// <param name="n">Address</param>
	/// <returns>Returns a string with ip host.</returns>
	static const char* getAddr(long n);

	/// <summary>
	/// Sleeps for some milliseconds (current thread).
	/// </summary>
	/// <param name="ms">The amount of milliseconds</param>
	static void sleep(unsigned int ms);

	/// <summary>
	/// Returns the load and the physical memory usage in percent
	/// </summary>
	/// <param name="mem">Variable where the percent of physical memory usage is storaged.</param>
	/// <param name="load">Variable where the percent of load CPU usage is storaged. (not implemented yet)</param>
	static void getLoad(float& mem, float load[3]);

#ifdef WIN32
	/// <summary>
	/// Wrapper for function used for thread library of Windows.
	/// </summary>
	/// <param name="fun">Callback function</param>
	/// <returns>Always returns zero.</returns>
	static unsigned long __stdcall wrapperFunThreadWin(void* fun);
#endif

	/// <summary>
	/// Create thread multiplatform.
	/// </summary>
	/// <param name="lpThread">Pointer to native thread.</param>
	/// <param name="fun_callback">Callback function for be executed in thread.</param>
	/// <returns>Returns true if the thread is created successful, false otherwise.</returns>
	static bool createThread(void* lpThread, FN_CALLBACK fun_callback);

	/// <summary>
	/// Kill thread multiplatform.
	/// </summary>
	/// <param name="lpThread">Pointer to native thread.</param>
	/// <param name="exitCode">Exit code (only for Windows).</param>
	static void killThread(void* lpThread, int exitCode = 0);

	/// <summary>
	/// Join thread multiplatform.
	/// </summary>
	/// <param name="lpThread">Pointer to native thread.</param>
	static void joinThread(void* lpThread);
};
