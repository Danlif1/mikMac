//
//  FileLogger.hpp
//  dstd
//
//  Created by Daniel Lifshitz on 13/06/2026.
//
#pragma once

#include "Files/Vnode.hpp"
#include "Logger.hpp"
#include "Result.hpp"
#include "Strings/String.hpp"


namespace dstd {

class FileLogger {
public:
    static Result<FileLogger> make(const String& path, const String& driverId);

    void log(LogLevel level, const char* message, ...);

    FileLogger(FileLogger&& other);
    FileLogger& operator=(FileLogger&& other);
    FileLogger(const FileLogger&) = delete;
    FileLogger& operator=(const FileLogger&) = delete;

    ~FileLogger();

private:
    FileLogger(File&& file, String&& driverId, off_t writeOffset);

    File m_file;
    String m_driverId;
    off_t m_writeOffset;
};

#define FILE_LOG(fileLogger, logLevel, message, ...) \
    (fileLogger).log( \
        logLevel, \
        "file: %s, function: %s, line %d " message "\n", \
        __FILE__, \
        __func__, \
        __LINE__, \
        ##__VA_ARGS__)

} // namespace dstd
