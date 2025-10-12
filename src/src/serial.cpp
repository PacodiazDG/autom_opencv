#include "serial.hxx"

#include <thread>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

// Minimal POSIX serial implementation (Linux). For Windows this would need
// a separate implementation.

Serial &Serial::instance()
{
    static Serial s;
    return s;
}

Serial::Serial() {}

Serial::~Serial()
{
    stopReader();
    closePort();
}

bool Serial::openPort(const std::string &device, int baud)
{
    std::lock_guard<std::mutex> lk(writeMutex_);
    if (open_.load())
        return true;

    fd_ = ::open(device.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
    if (fd_ < 0)
    {
        std::cerr << "Failed to open serial port " << device << " : " << strerror(errno) << std::endl;
        return false;
    }

    struct termios tty;
    if (tcgetattr(fd_, &tty) != 0)
    {
        std::cerr << "tcgetattr failed: " << strerror(errno) << std::endl;
        ::close(fd_);
        fd_ = -1;
        return false;
    }

    cfmakeraw(&tty);
    cfsetispeed(&tty, B115200);
    cfsetospeed(&tty, B115200);
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;

    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 5; // 0.5 seconds

    if (tcsetattr(fd_, TCSANOW, &tty) != 0)
    {
        std::cerr << "tcsetattr failed: " << strerror(errno) << std::endl;
        ::close(fd_);
        fd_ = -1;
        return false;
    }

    open_.store(true);
    // Flush any pending messages that were queued while port was closed.
    if (!pendingMessages_.empty())
    {
        for (const auto &m : pendingMessages_)
        {
            ssize_t written = ::write(fd_, m.data(), m.size());
            if (written < 0)
            {
                std::cerr << "Serial write failed while flushing queue: " << strerror(errno) << std::endl;
            }
            else
            {
                tcdrain(fd_);
            }
        }
        pendingMessages_.clear();
    }
    return true;
}

void Serial::closePort()
{
    std::lock_guard<std::mutex> lk(writeMutex_);
    if (fd_ >= 0)
    {
        ::close(fd_);
        fd_ = -1;
    }
    open_.store(false);
}

bool Serial::send(const std::string &message)
{
    if (!open_.load())
    {
        std::cerr << "Serial::send() - port not open, cannot send message: '" << message << "'\n";
        return false;
    }
       

    std::lock_guard<std::mutex> lk(writeMutex_);
    ssize_t written = ::write(fd_, message.data(), message.size());
    if (written < 0)
    {
        std::cerr << "Serial write failed: " << strerror(errno) << std::endl;
        return false;
    }
    // Ensure data is transmitted out to the device (block until output flushed)
    if (tcdrain(fd_) != 0)
    {
        std::cerr << "tcdrain failed: " << strerror(errno) << std::endl;
        // not fatal; we already wrote bytes
    }
    return true;
}

bool Serial::startReader()
{
    if (!open_.load() || readerRunning_.load())
        return false;

    readerRunning_.store(true);
    std::thread([this]() {
        constexpr size_t BUF = 256;
        char buf[BUF];
        std::string messageBuffer;

        while (readerRunning_.load())
        {
            int n = ::read(fd_, buf, BUF - 1);
            if (n > 0)
            {
                buf[n] = '\0';
                messageBuffer += buf;

                // Buscar fin de línea o delimitador
                size_t pos;
                while ((pos = messageBuffer.find('\n')) != std::string::npos)
                {
                    std::string completeMessage = messageBuffer.substr(0, pos);
                    std::cout << "[Serial RX] " << completeMessage << std::endl;
                    messageBuffer.erase(0, pos + 1); // Eliminar el mensaje procesado
                }
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    }).detach();

    return true;
}


void Serial::stopReader()
{
    readerRunning_.store(false);
}

// Static convenience wrapper implementation.
bool Serial::sendMessage(const std::string &message)
{
    // If the port is not open, attempt a lazy open once. This helps when
    // callers invoke sendMessage shortly after program start and the
    // serial thread hasn't fully opened the device yet.
    Serial &s = Serial::instance();
    if (!s.isOpen())
    {
        std::cerr << "Serial::sendMessage() - port not open, attempting lazy open...\n";
        // try common device - keep this consistent with qthreads
        if (!s.openPort("/dev/ttyUSB0", 115200))
        {
            std::cerr << "Serial::sendMessage() - lazy open failed, queuing message for later\n";
            // Queue the message to send later when port becomes available.
            std::lock_guard<std::mutex> lk(s.writeMutex_);
            s.pendingMessages_.push_back(message);
            return true;
        }
        // start reader if desired
        s.startReader();
    }

    bool ok = s.send(message);
    if (!ok)
    {
        std::cerr << "Serial::sendMessage() - send() failed, queuing message for later: '" << message << "'\n";
        std::lock_guard<std::mutex> lk(s.writeMutex_);
        s.pendingMessages_.push_back(message);
        return true; // accepted for later delivery
    }
    return true;
}
