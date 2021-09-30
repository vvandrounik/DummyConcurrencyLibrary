#ifndef __THREAD_POOL_H
#define __THREAD_POOL_H

#include <vector>
#include <queue>

#include <mutex>
#include <atomic>
#include <condition_variable>
#include <future>

namespace Dcl
{
	class ThreadPool
	{
	public:
		using TaskType = std::function<void()>;

		ThreadPool(
			const std::size_t threadsCount = std::thread::hardware_concurrency())
			: _isShutdown(false)
		{
			assignThreads(threadsCount);
		}

		~ThreadPool()
		{
			shutdown();
		}

		template <typename Func, typename... Args, 
			typename Res = std::result_of_t<Func&()>>
		std::future<Res> schedule(Func&& f, Args&&... args)
		{
			auto task = std::make_shared<std::packaged_task<Res()>>(
				std::bind(std::forward<Func>(f), std::forward<Args>(args)...));

			std::future<Res> future = task->get_future();

			{
				std::unique_lock<std::mutex> lock(_mutex);
				_tasksQueue.emplace(
					[task]()
					{ 
						if (task->valid())
						{
							(*task)();
						}
					}
				);
			}

			_cv.notify_one();
			return future;
		}

		std::size_t threadsCount() const
		{
			return _threads.size();
		}

		void shutdown()
		{
			if (_isShutdown)
			{
				return;
			}

			_isShutdown = true;
			_cv.notify_all();

			for (auto& th : _threads)
			{
				th.join();
			}
		}

	private:
		void assignThreads(std::size_t threadsCount)
		{
			_threads.resize(threadsCount);

			for (std::size_t i = 0; i < _threads.size(); ++i)
			{
				_threads[i] = std::thread(&ThreadPool::workerFunction, this);
			}
		}

		void workerFunction()
		{
			while (!_isShutdown)
			{
				TaskType task;

				{
					std::unique_lock<std::mutex> lock(_mutex);
					_cv.wait(lock,
						[this]() {return !_tasksQueue.empty() || _isShutdown; });

					if (_isShutdown)
					{
						return;
					}

					task = std::move(_tasksQueue.front());
					_tasksQueue.pop();
				}

				task();
			}
		}

		std::vector<std::thread> _threads;
		std::queue<TaskType> _tasksQueue;

		std::mutex _mutex;
		std::condition_variable _cv;
		std::atomic_bool _isShutdown;
	};
}

#endif /*__THREAD_POOL_H*/

