//
//  FileLogger.cpp
//  dstd
//
//  Created by Daniel Lifshitz on 13/06/2026.
//
#include "FileLogger.hpp"

#include "Checkers.hpp"
#include "Pointers/RawBuffer.hpp"

#include <stdarg.h>
#include <string.h>


namespace dstd {

namespace {

constexpr size_t k_logLineCapacity = 1024;
constexpr size_t k_fileSizeProbeChunk = 4096;

Result<off_t> endOfFileOffset(const File& file) {
    off_t offset = 0;

    while (true) {
        const Result<RawBuffer> readResult = file.read(k_fileSizeProbeChunk, offset);
        if (readResult.hasError()) {
            return Error(readResult.error());
        }

        const size_t bytesRead = readResult.value().size();
        if (0 == bytesRead) {
            break;
        }

        offset += static_cast<off_t>(bytesRead);

        if (bytesRead < k_fileSizeProbeChunk) {
            break;
        }
    }

    return off_t{offset};
}

} // namespace

Result<FileLogger> FileLogger::make(const String& driverId, const String& path) {
    CHECK_RESULT(file, File::make(path), "Failed to open log file");

    CHECK_RESULT(writeOffset, endOfFileOffset(file), "Failed to determine log file end offset");

    return FileLogger(String(driverId), move(file), writeOffset);
}

FileLogger::FileLogger(String&& driverId, File&& file, off_t writeOffset)
    : m_file(move(file))
    , m_driverId(move(driverId))
    , m_writeOffset(writeOffset)
{}

FileLogger::FileLogger(FileLogger&& other)
    : m_file(move(other.m_file))
    , m_driverId(move(other.m_driverId))
    , m_writeOffset(other.m_writeOffset)
{
    other.m_writeOffset = 0;
}

FileLogger& FileLogger::operator=(FileLogger&& other) {
    if (this == &other) {
        return *this;
    }

    m_file = move(other.m_file);
    m_driverId = move(other.m_driverId);
    m_writeOffset = other.m_writeOffset;
    other.m_writeOffset = 0;

    return *this;
}

FileLogger::~FileLogger() = default;

void FileLogger::log(LogLevel level, const char* message, ...) {
    char logLine[k_logLineCapacity];

    const int prefixLength = snprintf(
        logLine,
        sizeof(logLine),
        "%s level: %d, ",
        m_driverId.c_str(),
        static_cast<int>(level)
    );
    if (prefixLength <= 0 || prefixLength >= static_cast<int>(sizeof(logLine))) {
        return;
    }

    va_list args;
    va_start(args, message);
    const int messageLength = vsnprintf(
        logLine + prefixLength,
        sizeof(logLine) - static_cast<size_t>(prefixLength),
        message,
        args
    );
    va_end(args);

    if (messageLength <= 0) {
        return;
    }

    const int totalLength = prefixLength + messageLength;
    if (totalLength >= static_cast<int>(sizeof(logLine))) {
        return;
    }

    Result<RawBuffer> bufferResult = RawBuffer::make(static_cast<size_t>(totalLength));
    if (bufferResult.hasError()) {
        printf("%s", logLine);
        return;
    }

    memcpy(bufferResult.value().getCharBuffer(), logLine, static_cast<size_t>(totalLength));

    const Result<void> writeResult = m_file.write(move(bufferResult.value()), m_writeOffset);
    if (writeResult.hasError()) {
        printf("%s", logLine);
        return;
    }

    m_writeOffset += totalLength;
}

} // namespace dstd
