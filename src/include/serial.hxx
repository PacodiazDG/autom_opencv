// serial.hxx
#pragma once

#include <string>
#include <mutex>
#include <atomic>
#include <vector>

// Simple cross-platform POSIX serial wrapper (Linux-focused).
// Provides a global instance and a convenience static sendMessage() so other
// classes (like the camera/task) can notify the Arduino.
class Serial {
public:
	// Get singleton instance
	static Serial &instance();

	// Open serial port (e.g. "/dev/ttyUSB0"). Returns true on success.
	bool openPort(const std::string &device = "/dev/ttyUSB0", int baud = 115200);

	// Close port
	void closePort();

	// Send a text message over serial (thread-safe). Returns true on success.
	bool send(const std::string &message);

	// Convenience static wrapper declaration; definition is in the .cpp to
	// avoid potential inline/ODR issues and to allow richer diagnostics.
	static bool sendMessage(const std::string &message);

	// Start/stop internal reader thread
	bool startReader();
	void stopReader();

	// Whether port is open
	bool isOpen() const { return open_.load(); }

private:
	Serial();
	~Serial();

	// Non-copyable
	Serial(const Serial &) = delete;
	Serial &operator=(const Serial &) = delete;

	int fd_{-1};
	std::mutex writeMutex_;
	std::atomic<bool> open_{false};
	std::atomic<bool> readerRunning_{false};
	// Messages queued while serial port is not yet available. Protected by writeMutex_.
	std::vector<std::string> pendingMessages_;
	// ... reader thread handle lives in implementation file to avoid including <thread> here
};
