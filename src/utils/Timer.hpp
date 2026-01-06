#pragma once

#include <chrono>
#include <string>

class Timer {
public:
    Timer(const std::string& title);
    ~Timer();

    void Stop();

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
    std::string m_Title;
};