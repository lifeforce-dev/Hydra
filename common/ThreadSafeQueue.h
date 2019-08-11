//---------------------------------------------------------------
//
// ThreadSafeQueue.h
//

#pragma once

#include <mutex>
#include <queue>

namespace Common {

//==============================================================================

template<class T>
class ThreadSafeQueue
{

public:
	~ThreadSafeQueue()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_queue.clear();
	}

	// Push onto the queue.
	template <typename T>
	void Push(T&& item)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_queue.push_back(std::move(item));
	}

	// Returns a value at the head of the queue.
	template <typename T>
	T Pop(T&& item)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		T val = m_queue.front();
		return val;
	}

	// Returns true if empty.
	bool IsEmpty()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_queue.empty();
	}

	// Returns the number of values in the queue.
	uint32_t Size()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_queue.size();
	}

	// So we can handle queue items in chunks without having to pop every iteration.
	template <typename T>
	void SwapWithEmpty(std::deque<T>& other)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_queue.swap(other);
	}

private:
	std::deque<T> m_queue;
	std::mutex m_mutex;
};

//==============================================================================

} // namespace Common
