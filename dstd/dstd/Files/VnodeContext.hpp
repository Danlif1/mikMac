//
//  VnodeContext.hpp
//  dstd
//
//  Created by Daniel Lifshitz on 13/06/2026.
//
#pragma once

#include <sys/vnode.h>

#include "Result.hpp"


namespace dstd {

class VnodeContext {
public:
    // Uses kernel credentials by default; pass a source context to act as another process.
    static Result<VnodeContext> make(vfs_context_t sourceContext = nullptr);

    VnodeContext(VnodeContext&& other);
    VnodeContext& operator=(VnodeContext&& other);
    VnodeContext(const VnodeContext&) = delete;
    VnodeContext& operator=(const VnodeContext&) = delete;

    ~VnodeContext();

    vfs_context_t get() const;
    operator vfs_context_t() const;
    bool isValid() const;

private:
    explicit VnodeContext(vfs_context_t context);

    void reset();

    vfs_context_t m_context;
};

} // namespace dstd
