#pragma once

#include <memory>
#include <thread>
#include <mutex>

class Thread
{
    std::unique_ptr<std::thread> thread_;
    bool shouldStop_;

protected:
    virtual void run_() = 0;

public:
    Thread();
    ~Thread();

    void start();
    void stop();
    void join();
    bool shouldStop();
    bool isRunning();
};


