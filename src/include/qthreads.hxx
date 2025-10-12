#pragma once

#include <thread>
#include <atomic>

class qthreads
{
public:
    qthreads();
    ~qthreads();

    // Start camera and serial worker threads
    bool start();
    void stop();

private:
    std::thread camThread_;
    std::thread serialThread_;
    std::atomic<bool> running_{false};
};


