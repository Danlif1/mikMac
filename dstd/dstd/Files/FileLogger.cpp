//
//  FileLogger.cpp
//  dstd
//
//  Created by Daniel Lifshitz on 13/06/2026.
//
#include "FileLogger.hpp"

#include "Checkers.hpp"
#include "DataStructures/Arrays/Vector.hpp"
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
    CHECK_RESULT(lock, Lock::make("dstd.fileLogger"), "Failed to create file logger lock");
    CHECK_RESULT(unloggedMessages, Vector<String>::make(), "Failed to allocate unlogged message buffer");
    CHECK_RESULT(
        unloggedMessagesPtr,
        makeUnique<Vector<String>>(move(unloggedMessages)),
        "Failed to create unlogged message buffer");

    return FileLogger(String(driverId), move(file), writeOffset, move(lock), move(unloggedMessagesPtr));
}

FileLogger::FileLogger(
    String&& driverId,
    File&& file,
    off_t writeOffset,
    Lock&& lock,
    UniquePtr<Vector<String>>&& unloggedMessages)
    : m_file(move(file))
    , m_driverId(move(driverId))
    , m_writeOffset(writeOffset)
    , m_lock(move(lock))
    , m_unloggedMessages(move(unloggedMessages))
{}

FileLogger::FileLogger(FileLogger&& other)
    : m_file(move(other.m_file))
    , m_driverId(move(other.m_driverId))
    , m_writeOffset(other.m_writeOffset)
    , m_lock(move(other.m_lock))
    , m_unloggedMessages(move(other.m_unloggedMessages))
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
    m_lock = move(other.m_lock);
    m_unloggedMessages = move(other.m_unloggedMessages);
    other.m_writeOffset = 0;

    return *this;
}

FileLogger::~FileLogger() = default;

Result<RawBuffer> FileLogger::buildBufferFromLines(const Vector<String>& lines) const {
    size_t totalLength = 0;
    for (size_t index = 0; index < lines.size(); ++index) {
        totalLength += lines[index].length();
    }

    if (0 == totalLength) {
        return Error(KERN_INVALID_ARGUMENT);
    }

    CHECK_RESULT(buffer, RawBuffer::make(totalLength), "Failed to allocate log write buffer");

    char* writePosition = buffer.getCharBuffer();
    for (size_t index = 0; index < lines.size(); ++index) {
        const size_t lineLength = lines[index].length();
        memcpy(writePosition, lines[index].c_str(), lineLength);
        writePosition += lineLength;
    }

    return move(buffer);
}

Result<void> FileLogger::tryWriteLines(Vector<String>& lines) {
    if (lines.empty()) {
        return {};
    }

    CHECK_RESULT(buffer, buildBufferFromLines(lines), "Failed to build log write buffer");

    const size_t totalLength = buffer.size();
    const Result<void> writeResult = m_file.write(move(buffer), m_writeOffset);
    if (writeResult.hasError()) {
        return Error(writeResult.error());
    }

    m_writeOffset += static_cast<off_t>(totalLength);
    CHECK_RESULT_NO_VALUE(lines.clear(), "Failed to clear flushed log lines");

    return {};
}

void FileLogger::enqueueUnlogged(String&& line) {
    if (m_unloggedMessages.getValue()->size() >= k_maxUnloggedMessages) {
        return;
    }

    (void)m_unloggedMessages.getValue()->push_back(move(line));
}

void FileLogger::flushUnlogged() {
    m_lock.lockExclusive();

    Vector<String>* unloggedMessages = m_unloggedMessages.getValue();
    if (unloggedMessages->empty()) {
        m_lock.unlockExclusive();
        return;
    }

    Vector<String> linesToFlush = move(*unloggedMessages);
    Result<Vector<String>> restoredMessagesResult = Vector<String>::make();
    if (restoredMessagesResult.hasError()) {
        *unloggedMessages = move(linesToFlush);
        m_lock.unlockExclusive();
        return;
    }
    *unloggedMessages = move(restoredMessagesResult.value());

    const Result<void> flushResult = tryWriteLines(linesToFlush);
    if (flushResult.hasError()) {
        for (size_t index = 0; index < linesToFlush.size(); ++index) {
            enqueueUnlogged(move(linesToFlush[index]));
        }
    }

    m_lock.unlockExclusive();
}

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

    m_lock.lockExclusive();

    Result<Vector<String>> linesToWriteResult = Vector<String>::make();
    if (linesToWriteResult.hasError()) {
        m_lock.unlockExclusive();
        printf("%s", logLine);
        return;
    }
    Vector<String> linesToWrite = move(linesToWriteResult.value());

    Vector<String>* unloggedMessages = m_unloggedMessages.getValue();
    for (size_t index = 0; index < unloggedMessages->size(); ++index) {
        const Result<void> appendResult = linesToWrite.push_back(String((*unloggedMessages)[index]));
        if (appendResult.hasError()) {
            m_lock.unlockExclusive();
            printf("%s", logLine);
            return;
        }
    }

    const Result<String> currentLineResult = String::make(logLine, static_cast<size_t>(totalLength));
    if (currentLineResult.hasError()) {
        m_lock.unlockExclusive();
        printf("%s", logLine);
        return;
    }

    const Result<void> appendCurrentResult = linesToWrite.push_back(move(currentLineResult.value()));
    if (appendCurrentResult.hasError()) {
        m_lock.unlockExclusive();
        printf("%s", logLine);
        return;
    }

    const Result<void> writeResult = tryWriteLines(linesToWrite);
    if (writeResult.hasError()) {
        enqueueUnlogged(move(linesToWrite.back()));
        m_lock.unlockExclusive();
        printf("%s", logLine);
        return;
    }

    (void)unloggedMessages->clear();

    m_lock.unlockExclusive();
}

} // namespace dstd
