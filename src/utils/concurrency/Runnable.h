#pragma once

#include <atomic>
#include <thread>

/// <summary>
/// Runnable is an abstract class that encapsulate functionality of a thread.
/// </summary>
class Runnable
{
public:
	/// <summary>
	/// Constructor.
	/// </summary>
	/// <param name="isDetach">Whether this flag is true the thread detaches of parent thread.</param>
	Runnable(bool isDetach = false) : m_stop(), m_thread(), m_isDetach(isDetach) { }

	/// <summary>
	/// Destructor.
	/// </summary>
	virtual ~Runnable() { try { stop(); } catch (...) { /*??*/ } }

	/* Delete copy constructor for avoid mistake with thread. */
	Runnable(Runnable const&) = delete;
	Runnable& operator =(Runnable const&) = delete;

	/// <summary>
	/// Stops thread.
	/// </summary>
	void stop() { m_stop = true; if (!m_isDetach) m_thread.join(); }

	/// <summary>
	/// Starts thread.
	/// </summary>
	void start() { m_thread = std::thread(&Runnable::run, this); if (m_isDetach) m_thread.detach(); }

protected:
	/// <summary>
	/// Function executed for the thread. This function must be overwritten in the sub-classes
	/// </summary>
	virtual void run() = 0;

	std::atomic<bool> m_stop; /* Whether this flag is true, the thread gets stop. */
	std::atomic<bool> m_isDetach; /* Whether this flag is true, the thread detaches of parent thread. */

private:
	std::thread m_thread; /* Thread */
};
