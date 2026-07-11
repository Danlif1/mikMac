//
//  Thread.cpp
//  dstd
//
//  Created by Daniel Lifshitz on 20/06/2026.
//
#include "Thread.hpp"

#include "Checkers.hpp"

#include <libkern/libkern.h>


namespace dstd {

void Thread::ThreadDeleter::operator()(thread_t thread) {
    if (nullptr != thread) {
        thread_terminate(thread);
    }
}

void Thread::continuation(void* parameter, wait_result_t waitResult) {
    (void)waitResult;

    Context* context = static_cast<Context*>(parameter);
    if (nullptr == context || nullptr == context->entryPoint) {
        return;
    }

    context->entryPoint(context->parameter);
}

Thread::Thread(UniquePtr<ThreadResource, ThreadResourceDeleter>&& thread, UniquePtr<Context>&& context)
    : m_thread(move(thread))
    , m_context(move(context))
{}

Result<Thread> Thread::make(EntryPoint entryPoint, void* parameter) {
    GENERIC_CHECK(nullptr != entryPoint, KERN_INVALID_ARGUMENT, "thread entry point is null");

    CHECK_RESULT(context, makeUnique<Context>(entryPoint, parameter), "failed to allocate thread context");

    thread_t thread = nullptr;
    const kern_return_t startResult = kernel_thread_start(&Thread::continuation, context.getValue(), &thread);
    if (KERN_SUCCESS != startResult) {
        return Error(startResult);
    }

    thread_deallocate(thread);

    auto threadResourceResult = makeUniqueWithDeleter<ThreadResource, ThreadResourceDeleter>(ThreadResourceDeleter(), thread);
    if (threadResourceResult.hasError()) {
        return Error(threadResourceResult.error());
    }

    return Thread(move(threadResourceResult.value()), move(context));
}

} // namespace dstd
