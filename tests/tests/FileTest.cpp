//
//  FileTest.cpp
//  tests
//
#include "Framework/TestFramework.hpp"

#include "Files/Vnode.hpp"
#include "Pointers/RawBuffer.hpp"
#include "Strings/String.hpp"

#include <mach/mach_time.h>
#include <sys/errno.h>
#include <string.h>


using namespace dstd;
using namespace tests;

namespace {

Result<String> makeTmpPath(const char* suffix) {
    char pathBuffer[128];
    snprintf(pathBuffer, sizeof(pathBuffer), "/tmp/dstd_test_%s_%llu", suffix, mach_absolute_time());
    return String::make(pathBuffer);
}

Result<RawBuffer> makeWriteBuffer(const char* contents) {
    const size_t length = strlen(contents);
    CHECK_RESULT(buffer, RawBuffer::make(length), "Failed to allocate write buffer");
    memcpy(buffer.getCharBuffer(), contents, length);
    return Result<RawBuffer>::make(move(buffer));
}

} // namespace


dstd::Result<void> tests::testFile() {
    static_assert(is_same<Result<File>, decltype(File::open(declval<const String&>()))>::value, "File::open interface changed");
    static_assert(is_same<Result<File>, decltype(File::create(declval<const String&>()))>::value, "File::create interface changed");
    static_assert(is_same<Result<File>, decltype(File::make(declval<const String&>()))>::value, "File::make interface changed");
    CHECK_RESULT(createPath, makeTmpPath("create"), "Failed to build create path");
    {
        CHECK_RESULT(createdFile, File::create(createPath), "File::create failed");
        TEST_ASSERT(createdFile.isValid(), "created file should be valid");
        
        const auto duplicateCreate = File::create(createPath);
        TEST_ASSERT(duplicateCreate.hasError(), "File::create should fail when file already exists");
        TEST_ASSERT_EQ(EEXIST, duplicateCreate.error(), "File::create should return EEXIST");
    }

    CHECK_RESULT(openPath, makeTmpPath("open"), "Failed to build open path");
    {
        CHECK_RESULT(preparedFile, File::make(openPath), "Failed to prepare file for open test");
        TEST_ASSERT(preparedFile.isValid(), "prepared file should be valid");
    }
    {
        CHECK_RESULT(openedFile, File::open(openPath), "File::open failed");
        TEST_ASSERT(openedFile.isValid(), "opened file should be valid");
    }
    {
        CHECK_RESULT(missingPath, String::make("/tmp/dstd_test_missing_file"), "Failed to build missing path");
        const auto missingFile = File::open(missingPath);
        TEST_ASSERT(missingFile.hasError(), "File::open should fail when file does not exist");
        TEST_ASSERT_EQ(ENOENT, missingFile.error(), "File::open should return ENOENT");
    }

    CHECK_RESULT(makePath, makeTmpPath("make"), "Failed to build make path");
    {
        CHECK_RESULT(newFile, File::make(makePath), "File::make failed on new file");
        TEST_ASSERT(newFile.isValid(), "File::make should create a new file");
    }
    {
        CHECK_RESULT(existingFile, File::make(makePath), "File::make failed on existing file");
        TEST_ASSERT(existingFile.isValid(), "File::make should open an existing file");
    }

    CHECK_RESULT(readWritePath, makeTmpPath("readwrite"), "Failed to build read/write path");
    static constexpr const char* kPayload = "dstd file test payload";
    {
        CHECK_RESULT(readWriteFile, File::make(readWritePath), "Failed to open file for writing");
        CHECK_RESULT(writeBuffer, makeWriteBuffer(kPayload), "Failed to build write buffer");
        CHECK_RESULT_NO_VALUE(readWriteFile.write(move(writeBuffer)), "File::write failed");
    }
    {
        CHECK_RESULT(readFile, File::open(readWritePath), "Failed to open file for reading");
        CHECK_RESULT(readBuffer, readFile.read(strlen(kPayload)), "File::read failed");
        TEST_ASSERT_EQ(strlen(kPayload), readBuffer.size(), "File::read should return requested bytes");
        TEST_ASSERT_EQ(0, memcmp(readBuffer.getCharBuffer(), kPayload, strlen(kPayload)), "File::read should return written bytes");
    }
    {
        CHECK_RESULT(pathFile, File::open(readWritePath), "Failed to open file for path test");
        CHECK_RESULT(filePath, pathFile.path(), "File::path failed");
        TEST_ASSERT(nullptr != strstr(filePath.c_str(), "/tmp/"), "File::path should return a path under /tmp");
        TEST_ASSERT(nullptr != strstr(filePath.c_str(), "readwrite"), "File::path should include the file name");
    }

    CHECK_RESULT(realPath, String::make("/Users/daniellifshitz/macosKernelCoding/code/tests/a.txt"), "Failed to build real path");
    CHECK_RESULT(realFile, File::make(realPath), "Failed to open file for real path");

    return Result<void>::make();
}
