#pragma once
#include <string>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <iostream>

class AsyncLogger {
public:
    // Pobiera instancjê (Singleton) - ¿ebyœ móg³ logowaæ z ka¿dego miejsca
    static AsyncLogger& getInstance() {
        static AsyncLogger instance;
        return instance;
    }

    // Dodaje log do kolejki (To jest super szybkie!)
    void log(const std::string& message) {
        std::unique_lock<std::mutex> lock(queueMutex);
        logQueue.push(message);
        lock.unlock();
        cv.notify_one(); // ObudŸ w¹tek loggera
    }

    // Konstruktor uruchamia w¹tek
    AsyncLogger() : running(true) {
        workerThread = std::thread(&AsyncLogger::processLogs, this);
    }

    // Destruktor sprz¹ta
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

    // To robi osobny w¹tek w tle
    void processLogs() {
        while (true) {
            std::unique_lock<std::mutex> lock(queueMutex);

            // Czekaj, a¿ pojawi siê log albo koniec programu
            cv.wait(lock, [this] { return !logQueue.empty() || !running; });

            // Jeœli koniec i pusto, wyjdŸ
            if (!running && logQueue.empty()) {
                break;
            }

            // Pobierz wszystkie wiadomoœci naraz (¿eby rzadziej blokowaæ mutex)
            while (!logQueue.empty()) {
                std::string msg = logQueue.front();
                logQueue.pop();

                // Zwalniamy mutex na czas samego wypisywania (I/O jest wolne)
                lock.unlock();
                std::cout << msg << "\n"; // Tu jest powolne cout
                lock.lock();
            }
        }
    }
};

// Makro dla wygody - u¿ywaj LOG("tekst")
#define LOG(msg) AsyncLogger::getInstance().log(msg)