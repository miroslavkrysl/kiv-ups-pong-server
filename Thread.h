#pragma once

#include <memory>
#include <thread>
#include <mutex>

class Thread
{
    std::unique_ptr<std::thread> thread_;
    bool shouldStop_;
public:

    Thread();
    virtual ~Thread();

    virtual void run() = 0;

    virtual void start();
    virtual void stop();
    virtual void join() final;
    virtual bool shouldStop() final;
    virtual bool isRunning() final;
};
