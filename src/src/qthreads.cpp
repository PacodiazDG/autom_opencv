#include "qthreads.hxx"
#include "task.hxx"
#include "serial.hxx"
#include <iostream>

qthreads::qthreads() {}

qthreads::~qthreads()
{
    stop();
}

bool qthreads::start()
{
    if (running_.load())
        return false;

    running_.store(true);


    serialThread_ = std::thread([this]() {
        printf("Starting serial thread...\n");
        auto &s = Serial::instance();
        if (!s.openPort("/dev/ttyUSB0", 115200))
        {
            std::cerr << "Warning: could not open serial port /dev/ttyUSB0" << std::endl;

            while (running_.load())
                std::this_thread::sleep_for(std::chrono::seconds(1));
            return;
        }
    Serial::sendMessage("Hello from C++ Serial!\n");

        s.startReader();
        while (running_.load())
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        s.stopReader();
        s.closePort();
    });


    camThread_ = std::thread([this]() {

        auto &s = Serial::instance();
        int waited = 0;
        while (!s.isOpen() && waited < 2000 && running_.load())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            waited += 50;
        }


        int rc = task::task_function();
        (void)rc;

        running_.store(false);
    });

    return true;
}

void qthreads::stop()
{
    if (!running_.load())
        return;

    running_.store(false);
    if (camThread_.joinable())
        camThread_.join();
    if (serialThread_.joinable())
        serialThread_.join();
}