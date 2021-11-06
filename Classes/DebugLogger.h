#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <memory>

class DebugLogger
{
public:
	DebugLogger() = default;
	DebugLogger(const DebugLogger& other);
	DebugLogger& operator=(const DebugLogger& other);
	DebugLogger(const std::string& filename);
	~DebugLogger();

	void log(const std::string& msg);
	const std::string& getFilename() const;

private:
	std::string m_logFilename;
	std::ofstream m_fileStream;
};

typedef std::shared_ptr<DebugLogger> DebugLoggerPtr;