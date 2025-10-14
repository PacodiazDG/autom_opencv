
#pragma once

#include <string>
#include <mutex>
#include <atomic>
#include <vector>

class Serial
{
public:
	static Serial &instance();

	bool openPort(const std::string &device = "/dev/ttyUSB0", int baud = 115200);

	void closePort();

	bool send(const std::string &message);

	static bool sendMessage(const std::string &message);

	bool startReader();
	void stopReader();

	bool isOpen() const { return open_.load(); }

private:
	Serial();
	~Serial();

	Serial(const Serial &) = delete;
	Serial &operator=(const Serial &) = delete;

	int fd_{-1};
	std::mutex writeMutex_;
	std::atomic<bool> open_{false};
	std::atomic<bool> readerRunning_{false};

	std::vector<std::string> pendingMessages_;
};
