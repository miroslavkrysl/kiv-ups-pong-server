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

    virtual void start() final;
    virtual void stop() final;
    virtual void join() final;
    virtual bool shouldStop() final;
    virtual bool isRunning() final;
};
