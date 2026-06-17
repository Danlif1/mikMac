//
//  Vnode.hpp
//  dstd
//
//  Created by Daniel Lifshitz on 13/06/2026.
//
#pragma once

#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/vnode.h>

#include "Result.hpp"

#include "Pointers/RawBuffer.hpp"
#include "Strings/String.hpp"

#include "Files/VnodeContext.hpp"


namespace dstd {

class Vnode {
public:
    static Result<Vnode> open(const String& path, int fmode = O_RDONLY, int lookupFlags = 0);
    static Result<Vnode> create(const String& path, int fmode = O_RDWR, mode_t cmode = 0644, int lookupFlags = 0);
    static Result<Vnode> make(const String& path, int fmode = O_RDWR, mode_t cmode = 0644, int lookupFlags = 0);

    Vnode(Vnode&& other);
    Vnode& operator=(Vnode&& other);
    Vnode(const Vnode&) = delete;
    Vnode& operator=(const Vnode&) = delete;

    ~Vnode();

    vnode_t get() const;
    operator vnode_t() const;
    bool isValid() const;

    vfs_context_t context() const;
    Result<String> path() const;

    Result<RawBuffer> read(size_t length, off_t offset = 0) const;
    Result<void> write(RawBuffer&& buffer, off_t offset = 0);

private:
    static Result<Vnode> openWithFlags(const String& path, int fmode, mode_t cmode, int lookupFlags);

    Vnode(vnode_t vnode, VnodeContext&& context);

    void close();

    vnode_t m_vnode;
    VnodeContext m_context;
};

using File = Vnode;

} // namespace dstd
