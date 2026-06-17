//
//  FileLogger.hpp
//  dstd
//
//  Created by Daniel Lifshitz on 13/06/2026.
//
#pragma once

#include "Result.hpp"

#include "Files/Vnode.hpp"
#include "Strings/String.hpp"


namespace dstd {

enum LogLevel {
    LOG_DEBUG = 0,
    LOG_INFO = 1,
    LOG_WARNING = 2,
    LOG_ERROR = 3,
    LOG_CRITICAL = 4,
};

class FileLogger {
public:
    static Result<FileLogger> make(const String& driverId, const String& path);

    void log(LogLevel level, const char* message, ...);

    FileLogger(FileLogger&& other);
    FileLogger& operator=(FileLogger&& other);
    FileLogger(const FileLogger&) = delete;
    FileLogger& operator=(const FileLogger&) = delete;

    ~FileLogger();

private:
    FileLogger(String&& driverId, File&& file, off_t writeOffset);

    File m_file;
    String m_driverId;
    off_t m_writeOffset;
};

} // namespace dstd
