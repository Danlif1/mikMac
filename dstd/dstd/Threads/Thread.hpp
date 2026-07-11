//
//  Thread.hpp
//  dstd
//
//  Created by Daniel Lifshitz on 20/06/2026.
//
#pragma once

#include <mach/mach_types.h>

#include "Pointers/UniquePtr.hpp"
#include "Result.hpp"


namespace dstd {

class Thread {
public:
    using EntryPoint = void (*)(void* parameter);

    static Result<Thread> make(EntryPoint entryPoint, void* parameter = nullptr);

    Thread(Thread&& other) noexcept = default;
    Thread& operator=(Thread&& other) = default;
    Thread(const Thread&) = delete;
    Thread& operator=(const Thread&) = delete;

    ~Thread() = default;

private:
    struct Context {
        EntryPoint entryPoint;
        void* parameter;

        Context(EntryPoint entryPointValue, void* parameterValue)
            : entryPoint(entryPointValue)
            , parameter(parameterValue)
        {}
    };

    struct ThreadDeleter {
        void operator()(thread_t thread);
    };

    static void continuation(void* parameter, wait_result_t waitResult);

    Thread(UniquePtr<ThreadResource, ThreadResourceDeleter>&& thread, UniquePtr<Context>&& context);

    void stop();

    UniquePtr<thread, ThreadDeleter> m_thread;
    UniquePtr<Context> m_context;
};

} // namespace dstd
