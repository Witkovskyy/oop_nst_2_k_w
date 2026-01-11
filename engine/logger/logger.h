/**
 * @brief Get instance.
 *
 * @details Returns a value derived from current state.
 * @return Requested value.
 */


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
    /**
 * @brief Get instance.
 *
 * @details Returns a value derived from current state.
 * @return Requested value.
 */
    static AsyncLogger& getInstance() {
        static AsyncLogger instance;
        return instance;
    }

	// Add log to queue, thread-safe, fast
    /**
 * @brief Perform log.
 *
 * @details Implements the behavior implied by the function name.
 * @param message Parameter.
 */
    void log(const std::string& message) {
        std::unique_lock<std::mutex> lock(queueMutex);
        logQueue.push(message);
        lock.unlock();
        cv.notify_one(); // Wake up the thread
    }

	// Constructor deploying the worker thread
    /**
 * @brief Destroy the instance and release resources.
 *
 * @details Cleans up owned resources (if any).
 */
    AsyncLogger() : running(true) {
        workerThread = std::thread(&AsyncLogger::processLogs, this);
    }

    // Destructor
    /**
 * @brief Destroy the instance and release resources.
 *
 * @details Cleans up owned resources (if any).
 */
    ~AsyncLogger() {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            running = false;
        }
        /**
 * @brief Handle s logs.
 *
 * @details Processes an event or input and updates state.
 */
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
    /**
 * @brief Handle s logs.
 *
 * @details Processes an event or input and updates state.
 */
    void processLogs() {
        while (true) {
            std::unique_lock<std::mutex> lock(queueMutex);

			// Wait for log to show up or the program to end
            /**
             * @brief Perform `wait`.
             *
             * @details Documentation for `wait`.
             * @param lock Parameter.
             * @param running Parameter.
             * @return Result of the operation.
             */
            cv.wait(lock, [this] { return !logQueue.empty() || !running; });

            // Break if empty
            if (!running && logQueue.empty()) {
                break;
            }

			// Get all messages from the queue
            while (!logQueue.empty()) {
                /**
                 * @brief Perform `front`.
                 *
                 * @details Documentation for `front`.
                 * @return Result of the operation.
                 */
                std::string msg = logQueue.front();
                logQueue.pop();

				// Unlock mutex for output, then lock again
                lock.unlock();
                std::cout << msg << "\n"; // Slow cout
                /**
                 * @brief Perform `lock`.
                 *
                 * @details Documentation for `lock`.
                 * @return Result of the operation.
                 */
                lock.lock();
            }
        }
    }
};

// Makro for easier logging
#define LOG(msg) AsyncLogger::getInstance().log(msg)