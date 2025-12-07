#pragma once
#include <string>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <iostream>

class AsyncLogger {
public:
	// Singleton instance, so we can log from anywhere
    static AsyncLogger& getInstance() {
        static AsyncLogger instance;
        return instance;
    }

	// Add log to queue, thread-safe, fast
    void log(const std::string& message) {
        std::unique_lock<std::mutex> lock(queueMutex);
        logQueue.push(message);
        lock.unlock();
        cv.notify_one(); // Wake up the thread
    }

	// Constructor deploying the worker thread
    AsyncLogger() : running(true) {
        workerThread = std::thread(&AsyncLogger::processLogs, this);
    }

    // Destructor
    ~AsyncLogger() {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            running = false;
        }
        cv.notify_all();
        if (workerThread.joinable()) {
            workerThread.join();
        }
    }

private:
    std::queue<std::string> logQueue;
    std::mutex queueMutex;
    std::condition_variable cv;
    std::thread workerThread;
    bool running;

	// Open new thread to process logs
    void processLogs() {
        while (true) {
            std::unique_lock<std::mutex> lock(queueMutex);

			// Wait for log to show up or the program to end
            cv.wait(lock, [this] { return !logQueue.empty() || !running; });

            // Break if empty
            if (!running && logQueue.empty()) {
                break;
            }

			// Get all messages from the queue
            while (!logQueue.empty()) {
                std::string msg = logQueue.front();
                logQueue.pop();

				// Unlock mutex for output, then lock again
                lock.unlock();
                std::cout << msg << "\n"; // Slow cout
                lock.lock();
            }
        }
    }
};

// Makro for easier logging
#define LOG(msg) AsyncLogger::getInstance().log(msg)