#include "../platform/SystemCall.h"
#include "../util.h"

#include <functional>
#include <thread>
#include <chrono>

/// <summary>
/// Class to create a concurrent task.
///  The task is repeated until it returns true (that means the task was over) or timeout occurs.
/// </summary>
class Concurrency
{
protected:
	std::function<bool()> m_mainTask; /* The principal task. */
	std::function<void()> m_postTask; /* This task runs after principal task was over. */
	unsigned int m_nTimeout; /* Timeout for principal task, in milliseconds. */
	std::thread m_thread; /* Thread where principal task is executed. */
	int m_nSleep; /* Interval of time waiting between trials principal task. */

	/* Definitions for simplifying code. */
	typedef std::chrono::system_clock Time;
	typedef std::chrono::duration<float, std::milli> Duration;

	/// <summary>
	/// Thread Function.
	/// </summary>
	void run()
	{
		auto t0 = Time::now();
		while (true)
		{
			Duration elapsed = Time::now() - t0;
			if (m_mainTask() || (m_nTimeout > 0 && (elapsed.count() > m_nTimeout)))
				break; // Break if principal task was over or timeout occurs.
			SystemCall::sleep(m_nSleep);
		}

		if (m_postTask)
			m_postTask();
	}

public:
	/// <summary>
	/// Constructor
	/// </summary>
	/// <param name="_main">Principal task.</param>
	/// <param name="_post">Task executed after when principal task was over.</param>
	/// <param name="nTimeout">Timeout for principal task. If this argument is zero, then timeout doesn't use.</param>
	/// <param name="nSleep">Interval of time waiting between trials principal task.</param>
	Concurrency(std::function<bool()> _main, std::function<void()> _post = nullptr, unsigned int nTimeout = 0, unsigned int nSleep = 10) :
		m_mainTask(_main), m_postTask(_post), m_thread(), m_nTimeout(nTimeout), m_nSleep(nSleep)
	{
		m_thread = std::thread(&Concurrency::run, this);
		m_thread.detach();
	}

	/* Delete copy constructor for avoid mistake with thread. */
	Concurrency(Concurrency const&) = delete;
	Concurrency& operator =(Concurrency const&) = delete;
};
