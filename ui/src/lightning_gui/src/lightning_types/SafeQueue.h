

#pragma once

#include <mutex>
#include <queue>
#include <condition_variable>
#include <iostream>


template <typename T>
class SafeQueue {
public:
    SafeQueue(): queueCond_(), mMutex(){};
    SafeQueue(SafeQueue&& other){};
    ~SafeQueue(){};

public:
    bool empty()
    {
        std::unique_lock<std::mutex> lock(mMutex);
        return mQueue.empty();
    };
    int size()
    {
        std::unique_lock<std::mutex> lock(mMutex);
        return mQueue.size();
    };
    void put(T& t)
    {
        std::unique_lock<std::mutex> lock(mMutex);
        mQueue.push(t);
        queueCond_.notify_one();
    };

    void put(T&& t)
    {
        std::unique_lock<std::mutex> lock(mMutex);
        mQueue.push(std::forward<T>(t));
        queueCond_.notify_one();
    };

    bool get(T& t)
    {
        std::unique_lock<std::mutex> lock(mMutex);
        // wait_for returns false if the return is due to timeout
	    auto start = std::chrono::steady_clock::now();
        auto timeoutOccured = !queueCond_.wait_for(lock, std::chrono::milliseconds(3000), \
                                [this] { return !mQueue.empty(); });
                            
	    auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_1 = end - start;
        //std::cout << "======time for timeoutOccured: " <<  elapsed_1.count() * 1000 << " ms" << std::endl;    
        if (timeoutOccured) {
            std::cout << "!!!!!!!!! timeoutOccured!!!!!! "<< std::endl;    
            return true;
        }
        t = std::move(mQueue.front());
        mQueue.pop();
        return false;
    };

private:
    std::queue<T> mQueue;
    std::mutex mMutex;
    std::condition_variable queueCond_;
};
