#include <ThreadPool.h>
#include <Accumulator.h>

#include <gtest/gtest.h>
#include <vector>
#include <numeric>

const auto THREADS_COUNT = std::thread::hardware_concurrency() - 2;

TEST(BasicTests, EmptyVector)
{
    //arrange
    std::vector<int> v;
    Dcl::ThreadPool pool(THREADS_COUNT);
    Dcl::Accumulator acc(pool);

    //act
    auto f = acc.calculate(std::cbegin(v), std::cend(v), 0, pool.threadsCount());
    auto res = f.get();

    //assert
    EXPECT_EQ(0, res);
}

TEST(BasicTests, AllOnes)
{
    //arrange
    std::vector<int> v(100, 1);
    Dcl::ThreadPool pool(THREADS_COUNT);
    Dcl::Accumulator acc(pool);

    //act
    auto f = acc.calculate(std::cbegin(v), std::cend(v), 0, pool.threadsCount());
    auto res = f.get();

    //assert
    EXPECT_EQ(100, res);
}

TEST(BasicTests, NumericSequence)
{
    //arrange
    std::vector<int> v;
    v.reserve(100);
    for (int i = 0; i < v.size(); ++i)
    {
        v.push_back(i);
    }

    auto first = std::cbegin(v);
    auto last = std::cend(v);

    auto sum = std::accumulate(first, last, 0);

    Dcl::ThreadPool pool(THREADS_COUNT);
    Dcl::Accumulator acc(pool);

    //act
    auto f = acc.calculate(first, last, 0, pool.threadsCount());
    auto res = f.get();

    //assert
    EXPECT_EQ(sum, res);
}