#include "DebugLogger.h"

DebugLogger::DebugLogger(const DebugLogger& other)
{
	if (m_fileStream.is_open())
		m_fileStream.close();
	m_fileStream.open(other.getFilename(), std::ios::out | std::ios_base::app);
}

DebugLogger& DebugLogger::operator=(const DebugLogger& other)
{
	if (m_fileStream.is_open())
		m_fileStream.close();
	m_fileStream.open(other.getFilename(), std::ios::out | std::ios_base::app);
	return *this;
}

DebugLogger::DebugLogger(const std::string& filename)
{
	m_fileStream.open(filename, std::ios::out | std::ios_base::app);
}

DebugLogger::~DebugLogger()
{
	if (m_fileStream.is_open())
		m_fileStream.close();
}

void DebugLogger::log(const std::string& msg)
{
	if (!m_fileStream.is_open())
		return;

	m_fileStream << msg << std::endl;
}

const std::string& DebugLogger::getFilename() const
{
	return m_logFilename;
}
