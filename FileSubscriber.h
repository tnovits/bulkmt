#pragma once

#include "ISubscriber.h"
#include <fstream>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <iostream>
#include <vector>
#include <array>


template <size_t ThreadCount>
class FileSubscriber final : public ISubscriber
{
private:
    using Data_Type = std::deque< std::tuple<std::time_t ,std::deque<std::string>>>;
    using Metrix_Type = std::vector<std::tuple<size_t, size_t>>;

public:
    FileSubscriber()
    {
        for(auto& thread : m_Threds)
        {
            thread = std::thread([this](){this->print();});
        }
    }
    virtual ~FileSubscriber()
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
            std::cout << "File" << index << ": " << std::get<0>(element) << " commands, " << std::get<1>(element) << " blocks; " << std::endl;
            index++;
        }
        m_MetrixMutex.unlock();
    }

    void update( std::time_t blockTime, const std::deque<std::string>& data ) override
    {
        m_DataMutex.lock();
        m_data.emplace_back(blockTime, data);
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
  //              auto [time, data] = m_data.front();
                auto  data = m_data.front();
                m_data.pop_front();
                m_DataMutex.unlock();

                ++blockCount;

                static uint32_t count = 0;

                std::string fileName = "bulk" + std::to_string(std::get<0>(data)) + "_" + std::to_string(count++) +".log";

                std::fstream fs;

                fs.open(fileName, std::ios::out);

                if(fs.is_open())
                {
                    while( !std::get<1>(data).empty() )
                    {
                        auto element = std::get<1>(data).front();
                        std::get<1>(data).pop_front();

                        ++commandCount;

                        fs << fibonacci(std::stoi(element)) << "\n";
                    }

                    fs.close();
                }
            }
        }

        m_MetrixMutex.lock();
        m_Metrix.emplace_back(commandCount, blockCount);
        m_MetrixMutex.unlock();
    }

private:
    size_t fibonacci( size_t number )
    {
        return (number == 0 || number == 1) ? 1 : fibonacci(number-1) + fibonacci(number - 2);
    }

private:
    std::array<std::thread, ThreadCount> m_Threds;
    std::condition_variable m_cv;
    std::mutex cv_m;
    std::mutex m_DataMutex;
    std::atomic_bool m_flag;
    Data_Type m_data;
    Metrix_Type m_Metrix;
    std::mutex m_MetrixMutex;
};
