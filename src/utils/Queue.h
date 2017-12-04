#pragma once

#include <list>
#include <thread>
#include <mutex>
#include <condition_variable>

/// <summary>
/// Queue class with concurrency support.
/// </summary>
template <typename T> class Queue
{
protected:
	std::list<T> m_queue; /* Queue where the data is stored. */
	std::mutex m_mutex; /* Mutex for synchronized access to queue. */
	std::condition_variable m_condv; /* Condition for synchronized access to queue. */

public:

	/// <summary>
	/// Constructor
	/// </summary>
	Queue();

	/// <summary>
	/// Added an item to queue.
	/// </summary>
	/// <param name="item">New item to add to queue.</param>
	void add(T item);

	/// <summary>
	/// Remove first item from queue.
	/// </summary>
	/// <returns>Returns first element from queue and remove it.</returns>
	T remove();

	/// <summary>
	/// Look for an item in queue and remove it.
	/// </summary>
	/// <param name="item">Item to search in queue.</param>
	/// <returns>If item is found, this function returns pointer with item, nullptr otherwise.</returns>
	std::shared_ptr<T> remove(T item);

	/// <summary>
	/// Size of queue.
	/// </summary>
	/// <returns>Returns amount of items in queue.</returns>
	int size();
};

template<typename T>
Queue<T>::Queue() : m_mutex(), m_condv()
{
}

template<typename T>
void Queue<T>::add(T item)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	m_queue.push_back(item);
	m_condv.notify_one();
}

template<typename T>
T Queue<T>::remove()
{
	std::unique_lock<std::mutex> lock(m_mutex);
	while (m_queue.size() == 0)
	{
		m_condv.wait(lock);
	}
	T item = m_queue.front();
	m_queue.pop_front();
	return item;
}

template<typename T>
std::shared_ptr<T> Queue<T>::remove(T item)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	while (m_queue.size() == 0)
	{
		m_condv.wait(lock); // waiting until recover mutex
	}
	std::list<T>::iterator it = std::find(m_queue.begin(), m_queue.end(), item);

	if (it != m_queue.end())
	{
		T t(*it);
		m_queue.remove(*it);
		return std::make_shared<T>(t);
	}
	else
	{
		return nullptr;
	}
}

template<typename T>
int Queue<T>::size()
{
	std::unique_lock<std::mutex> lock(m_mutex);
	return (int)m_queue.size();
}