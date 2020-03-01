#pragma once

#include "ISubscriber.h"

#include <atomic>
#include <condition_variable>
#include <iostream>
#include <thread>
#include <tuple>
#include <array>


template <size_t ThreadCount>
class CoutSubscriber final : public ISubscriber
{
private:
    using Data_Type = std::deque<std::deque<std::string>>;
    using Metrix_Type = std::vector<std::tuple<size_t, size_t>>;

public:
    CoutSubscriber()
    {
        for(auto& thread : m_Threds)
        {
            thread = std::thread([this](){this->print();});
        }
    }

    virtual ~CoutSubscriber()
    {
        m_flag = true;
        m_cv.notify_all();

        for(auto& thread : m_Threds)
        {
            if(thread.joinable())
            {
                thread.join();
            }
        }
    }

    void printMetrix()
    {
        auto index = 0;

        m_flag = true;
        m_cv.notify_all();

        for(auto& thread : m_Threds)
        {
            if(thread.joinable())
            {
                thread.join();
            }
        }

        m_MetrixMutex.lock();
        for(auto& element : m_Metrix)
        {
            std::cout << "Log" << index << ": " << std::get<0>(element) << " commands, " << std::get<1>(element) << " blocks; " << std::endl;
            index++;
        }
        m_MetrixMutex.unlock();
    }


    void update( const std::time_t blockTime, const std::deque<std::string>& data ) override
    {
        std::ignore = blockTime;

        m_DataMutex.lock();
        m_data.emplace_back(data);
        m_DataMutex.unlock();

        m_cv.notify_one();
    }

private:
    void print()
    {
        auto commandCount = 0;
        auto blockCount = 0;

        while (!m_flag)
        {
            std::unique_lock<std::mutex> lk(cv_m);

            m_cv.wait(lk, [this](){ return !m_data.empty() || m_flag;});

            if(!m_data.empty())
            {
                m_DataMutex.lock();
                auto data = m_data.front();
                m_data.pop_front();
                m_DataMutex.unlock();

                ++blockCount;
                std::cout << "bulk: ";

                while (!data.empty())
                {
                    auto element = data.front();
                    data.pop_front();

                    std::cout << factorial(std::stoi(element));
                    ++commandCount;

                    if(!data.empty())
                    {
                        std::cout << ", ";
                    }
                }
            }

            std::cout << std::endl;
        }

        m_MetrixMutex.lock();
        m_Metrix.emplace_back(commandCount, blockCount);
        m_MetrixMutex.unlock();
    }

private:
    size_t factorial( size_t number )
    {
        return (number == 0) ? 1 : number * factorial(number-1);
    }

private:
    std::array<std::thread, ThreadCount> m_Threds;
    std::thread m_Log;
    std::condition_variable m_cv;
    std::mutex cv_m;
    std::atomic_bool m_flag;
    Data_Type m_data;
    std::mutex m_DataMutex;
    Metrix_Type m_Metrix;
    std::mutex m_MetrixMutex;
};