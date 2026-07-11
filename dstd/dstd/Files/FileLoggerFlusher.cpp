//
//  FileLoggerFlusher.cpp
//  dstd
//
//  Created by Daniel Lifshitz on 20/06/2026.
//
#include "FileLoggerFlusher.hpp"

#include "Checkers.hpp"
#include "DataStructures/Singleton.hpp"
#include "Files/FileLogger.hpp"

#include <sys/systm.h>


extern int hz;
extern "C" void delay(int);


namespace dstd {

FileLoggerFlusher::FileLoggerFlusher(Thread&& thread)
    : m_thread(move(thread))
{}

void FileLoggerFlusher::threadEntry(void* parameter) {
    (void)parameter;

    while (true) {
        Result<Singleton<FileLogger>> loggerResult = Singleton<FileLogger>::get();
        if (loggerResult.hasValue()) {
            loggerResult.value()->flushUnlogged();
        }

        delay(hz);
    }
}

Result<FileLoggerFlusher> FileLoggerFlusher::make() {
    CHECK_RESULT(thread, Thread::make(&FileLoggerFlusher::threadEntry), "Failed to create file logger flusher thread");

    return FileLoggerFlusher(move(thread));
}

} // namespace dstd
