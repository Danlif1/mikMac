//
//  FileLoggerFlusher.hpp
//  dstd
//
//  Created by Daniel Lifshitz on 20/06/2026.
//
#pragma once

#include "Result.hpp"
#include "Threads/Thread.hpp"


namespace dstd {

class FileLoggerFlusher {
public:
    static Result<FileLoggerFlusher> make();

private:
    explicit FileLoggerFlusher(Thread&& thread);

    static void threadEntry(void* parameter);

    Thread m_thread;
};

} // namespace dstd
