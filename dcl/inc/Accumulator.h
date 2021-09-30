#ifndef __ACCUMULATOR_H
#define __ACCUMULATOR_H

#include "ThreadPool.h"

#include <memory>
#include <vector>
#include <future>
#include <functional>
#include <numeric>
#include <string>

namespace Dcl
{
	class Accumulator
	{
	public:
		Accumulator(ThreadPool& pool)
			: _threadPool(pool)
		{
		}

		Accumulator(Accumulator&) = delete;
		Accumulator& operator=(Accumulator&) = delete;

		Accumulator(Accumulator&&) = default;
		Accumulator& operator=(Accumulator&&) = default;

		~Accumulator() = default;

		template <typename InputIt, typename T>
		std::future<T> calculate(InputIt first, InputIt last,
			T init, std::size_t parts_count)
		{
			std::size_t rangeSize = std::distance(first, last);

			using FuturesVecType = std::vector<std::future<T>>;
			auto chunkFutures = std::make_shared<FuturesVecType>();
			chunkFutures->reserve(parts_count);

			std::size_t chunkSize = rangeSize / parts_count;

			while (first != last) {
				auto next = static_cast<size_t>(std::distance(first, last)) >= chunkSize
					? first + chunkSize
					: last;
				
				std::future<T> future;
				scheduleAccumulate(first, next, future);
				chunkFutures->push_back(std::move(future));

				first = next;
			}

			std::future<T> result = _threadPool.schedule(
				[chunkFutures, rangeSize]()
				{
					return std::accumulate(
						std::begin(*chunkFutures),
						std::end(*chunkFutures),
						0,
						[](T a, std::future<T>& b) {return a + b.get(); });
				}
			);

			return result;
		}

	private:
		template <typename InputIt, typename T>
		void scheduleAccumulate(InputIt first, InputIt last, std::future<T>& future)
		{
			future = _threadPool.schedule(
				[first, last]()
				{
					return std::accumulate(first, last, 0);
				}
			);
		}

		ThreadPool& _threadPool;	
	};
}

#endif /*__ACCUMULATOR_H*/
