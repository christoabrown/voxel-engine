#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

// A threadsafe-queue.
template <class T>
class SafeQueue
{
public:
	SafeQueue(void)
		: q()
		, m()
		, c()
	{
		_empty = q.empty();
		_size = q.size();
	}

	~SafeQueue(void)
	{}

	SafeQueue(const SafeQueue& sq) : m(), c()
	{
		std::lock_guard<std::mutex> lock(sq.m);
		q = sq.q;
		_empty = q.empty();
		_size = q.size();
	}

	// Add an element to the queue.
	void enqueue(T t)
	{
		//while (dequeueFirst)
			//std::this_thread::sleep_for(std::chrono::milliseconds(1));
		std::lock_guard<std::mutex> lock(m);
		q.push(t);
		_empty = q.empty();
		_size = q.size();
		c.notify_one();
	}

	// Get the "front"-element.
	// If the queue is empty, wait till a element is avaiable.
	T dequeue(void)
	{
		std::unique_lock<std::mutex> lock(m);
		//while (q.empty())
		//{
			// release lock as long as the wait and reaquire it afterwards.
			//c.wait(lock);
		//}
		T val = q.front();
		q.pop();
		_empty = q.empty();
		_size = q.size();
		return val;
	}

	void clear(void)
	{
		std::lock_guard<std::mutex> lock(m);
		std::queue<T> empty;
		std::swap(q, empty);
		_empty = q.empty();
		_size = q.size();
	}

	bool empty(void)
	{
		return(_empty);
	}
	size_t size(void)
	{
		return(_size);
	}
private:
	std::queue<T> q;
	mutable std::mutex m;
	std::condition_variable c;
	bool _empty;
	size_t _size;
	
};