//
//  Vnode.cpp
//  dstd
//
//  Created by Daniel Lifshitz on 13/06/2026.
//
#include "Vnode.hpp"

#include <sys/uio.h>


namespace dstd {

Result<Vnode> Vnode::openWithFlags(const String& path, int fmode, mode_t cmode, int lookupFlags) {
    CHECK_RESULT(context, VnodeContext::make(), "Failed to create vfs context");

    vnode_t vnode = nullptr;
    const errno_t error = vnode_open(path.c_str(), fmode, cmode, lookupFlags, &vnode, context.get());
    if (0 != error) {
        return Result<Vnode>::makeError(static_cast<kern_return_t>(error));
    }

    return Result<Vnode>::make(Vnode(vnode, move(context)));
}

Result<Vnode> Vnode::open(const String& path, int fmode, int lookupFlags) {
    return openWithFlags(path, fmode, 0, lookupFlags);
}

Result<Vnode> Vnode::create(const String& path, int fmode, mode_t cmode, int lookupFlags) {
    return openWithFlags(path, fmode | O_CREAT | O_EXCL, cmode, lookupFlags);
}

Result<Vnode> Vnode::make(const String& path, int fmode, mode_t cmode, int lookupFlags) {
    return openWithFlags(path, fmode | O_CREAT, cmode, lookupFlags);
}

Vnode::Vnode(vnode_t vnode, VnodeContext&& context)
    : m_vnode(vnode)
    , m_context(move(context))
{}

Vnode::Vnode(Vnode&& other)
    : m_vnode(other.m_vnode)
    , m_context(move(other.m_context))
{
    other.m_vnode = nullptr;
}

Vnode& Vnode::operator=(Vnode&& other) {
    if (this == &other) {
        return *this;
    }

    close();

    m_vnode = other.m_vnode;
    m_context = move(other.m_context);

    other.m_vnode = nullptr;

    return *this;
}

Vnode::~Vnode() {
    close();
}

vnode_t Vnode::get() const {
    return m_vnode;
}

Vnode::operator vnode_t() const {
    return m_vnode;
}

bool Vnode::isValid() const {
    return nullptr != m_vnode;
}

vfs_context_t Vnode::context() const {
    return m_context.get();
}

Result<String> Vnode::path() const {
    GENERIC_CHECK(isValid(), KERN_INVALID_ADDRESS, "Invalid vnode");

    CHECK_RESULT(longName, String::make(MAXPATHLEN), "Failed to allocate path buffer");

    int length = MAXPATHLEN;
    vn_getpath(m_vnode, longName.c_str(), &length);
    GENERIC_CHECK(0 != length, KERN_FAILURE, "Failed to get vnode path");

    CHECK_RESULT(shortName, String::make(longName.c_str()), "Failed to copy vnode path");

    return Result<String>::make(move(shortName));
}

Result<RawBuffer> Vnode::read(size_t length, off_t offset) const {
    GENERIC_CHECK(isValid(), KERN_INVALID_ADDRESS, "Invalid vnode");
    GENERIC_CHECK(0 != length, KERN_INVALID_ARGUMENT, "Invalid read length");
    GENERIC_CHECK(length <= static_cast<size_t>(INT_MAX), KERN_INVALID_ARGUMENT, "Read length is too large");

    CHECK_RESULT(buffer, RawBuffer::make(length), "Failed to allocate read buffer");

    int bytesRemaining = 0;
    const int error = vn_rdwr(
        UIO_READ,
        m_vnode,
        buffer.getCharBuffer(),
        static_cast<int>(length),
        offset,
        UIO_SYSSPACE,
        IO_NOAUTH,
        vfs_context_ucred(m_context.get()),
        &bytesRemaining,
        vfs_context_proc(m_context.get())
    );
    if (0 != error) {
        return Result<RawBuffer>::makeError(static_cast<kern_return_t>(error));
    }

    const size_t bytesRead = length - static_cast<size_t>(bytesRemaining);
    if (bytesRead < length) {
        CHECK_RESULT_NO_VALUE(buffer.resize(bytesRead), "Failed to resize read buffer");
    }

    return Result<RawBuffer>::make(move(buffer));
}

Result<void> Vnode::write(RawBuffer&& buffer, off_t offset) {
    GENERIC_CHECK(isValid(), KERN_INVALID_ADDRESS, "Invalid vnode");
    GENERIC_CHECK(0 != buffer.size(), KERN_INVALID_ARGUMENT, "Invalid write buffer");
    GENERIC_CHECK(buffer.size() <= static_cast<size_t>(INT_MAX), KERN_INVALID_ARGUMENT, "Write buffer is too large");

    int bytesRemaining = 0;
    const int error = vn_rdwr(
        UIO_WRITE,
        m_vnode,
        buffer.getCharBuffer(),
        static_cast<int>(buffer.size()),
        offset,
        UIO_SYSSPACE,
        IO_NOAUTH,
        vfs_context_ucred(m_context.get()),
        &bytesRemaining,
        vfs_context_proc(m_context.get())
    );
    if (0 != error) {
        return Result<void>::makeError(static_cast<kern_return_t>(error));
    }

    GENERIC_CHECK(0 == bytesRemaining, KERN_FAILURE, "Incomplete write");

    return Result<void>::make();
}

void Vnode::close() {
    if (nullptr == m_vnode) {
        return;
    }

    vnode_close(m_vnode, 0, m_context.get());
    m_vnode = nullptr;
}

} // namespace dstd
