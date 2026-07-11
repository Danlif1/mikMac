//
//  FileLogger.hpp
//  dstd
//
//  Created by Daniel Lifshitz on 13/06/2026.
//
#pragma once

#include "Result.hpp"

#include "Files/Vnode.hpp"
#include "Pointers/UniquePtr.hpp"
#include "Strings/String.hpp"
#include "Synchronization/Lock.hpp"


namespace dstd {

template<typename T>
class Vector;

class RawBuffer;

enum LogLevel {
    LOG_DEBUG = 0,
    LOG_INFO = 1,
    LOG_WARNING = 2,
    LOG_ERROR = 3,
    LOG_CRITICAL = 4,
};

class FileLogger {
public:
    static constexpr size_t k_maxUnloggedMessages = 100;

    static Result<FileLogger> make(const String& driverId, const String& path);

    void log(LogLevel level, const char* message, ...);
    void flushUnlogged();

    FileLogger(FileLogger&& other);
    FileLogger& operator=(FileLogger&& other);
    FileLogger(const FileLogger&) = delete;
    FileLogger& operator=(const FileLogger&) = delete;

    ~FileLogger();

private:
    FileLogger(
        String&& driverId,
        File&& file,
        off_t writeOffset,
        Lock&& lock,
        UniquePtr<Vector<String>>&& unloggedMessages);

    Result<void> tryWriteLines(Vector<String>& lines);
    Result<RawBuffer> buildBufferFromLines(const Vector<String>& lines) const;
    void enqueueUnlogged(String&& line);

    File m_file;
    String m_driverId;
    off_t m_writeOffset;
    Lock m_lock;
    UniquePtr<Vector<String>> m_unloggedMessages;
};

} // namespace dstd
