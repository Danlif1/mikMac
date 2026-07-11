//
//  VnodeContext.cpp
//  dstd
//
//  Created by Daniel Lifshitz on 13/06/2026.
//
#include "VnodeContext.hpp"

#include "Checkers.hpp"
#include "Signatures/Address.hpp"

#include <mach/arm/vm_param.h>
#include <sys/proc.h>


namespace dstd {

namespace {

struct vfs_context {
    thread_t        vc_thread;              /* pointer to Mach thread */
    kauth_cred_t    vc_ucred;               /* per thread credential */
};

constexpr uintptr_t k_vfsContextKernelOffsetFromRele = 12;

Result<vfs_context_t> makeKernelContext() {
    const uintptr_t vfsContextKernelFunctionAddress =
        VM_KERNEL_STRIP_PTR(vfs_context_rele) - k_vfsContextKernelOffsetFromRele;

    CHECK_RESULT(
        kernelContextAddress,
        unstable::addressFromAdrl(reinterpret_cast<void*>(vfsContextKernelFunctionAddress)),
        "Failed to resolve kernel vfs context"
    );
    
    LOG(LogLevel::LOG_DEBUG, "kernelContextAddress: %p, vfsContextKernelFunctionAddress: %p", kernelContextAddress, vfsContextKernelFunctionAddress);
    LOG(LogLevel::LOG_DEBUG, "vfs_context_rele: %p", VM_KERNEL_STRIP_PTR(vfs_context_rele));
    LOG(LogLevel::LOG_DEBUG, "kernelContextAddress.vc_thread: %p, kernelContextAddress.vc_ucred: %p", reinterpret_cast<vfs_context*>(kernelContextAddress)->vc_thread, reinterpret_cast<vfs_context*>(kernelContextAddress)->vc_ucred);
    return static_cast<vfs_context_t>(kernelContextAddress);
}

} // namespace

Result<VnodeContext> VnodeContext::make(vfs_context_t sourceContext) {
    static vfs_context_t kernelContext = nullptr;

    vfs_context_t context = nullptr;
    if (nullptr == sourceContext) {
        if (nullptr == kernelContext) {
            CHECK_RESULT(tempKernelContext, makeKernelContext(), "Failed to create kernel vfs context");
            kernelContext = tempKernelContext;
        }
        
        context = kernelContext;
    } else {
        context = vfs_context_create(sourceContext);
    }
    
    GENERIC_CHECK(nullptr != context, KERN_FAILURE, "Failed to create vfs context");

    return VnodeContext(context);
}

VnodeContext::VnodeContext(vfs_context_t context)
    : m_context(context)
{}

VnodeContext::VnodeContext(VnodeContext&& other)
    : m_context(other.m_context)
{
    other.m_context = nullptr;
}

VnodeContext& VnodeContext::operator=(VnodeContext&& other) {
    if (this == &other) {
        return *this;
    }

    reset();

    m_context = other.m_context;
    other.m_context = nullptr;

    return *this;
}

VnodeContext::~VnodeContext() {
    reset();
}

vfs_context_t VnodeContext::get() const {
    return m_context;
}

VnodeContext::operator vfs_context_t() const {
    return m_context;
}

bool VnodeContext::isValid() const {
    return nullptr != m_context;
}

void VnodeContext::reset() {
    if (nullptr == m_context) {
        return;
    }

    // vfs_context_rele(m_context);
    m_context = nullptr;
}

} // namespace dstd
