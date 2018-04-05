
#include <gmock/gmock.h>

#include <liblocate/liblocate.h>


class liblocate_test : public testing::Test
{
public:
    liblocate_test()
    {
    }
};

TEST_F(liblocate_test, getExecutablePath_NoReturn)
{
    getExecutablePath(nullptr, nullptr);

    SUCCEED();
}

TEST_F(liblocate_test, getExecutablePath_Return)
{
    char * executablePath = 0x0;
    unsigned int length = 0;

    getExecutablePath(&executablePath, &length);

    EXPECT_LT(0, length);
    EXPECT_FALSE(executablePath == 0x0);
    EXPECT_EQ(0, executablePath[length]);
    EXPECT_EQ(length, strlen(executablePath));

    free(executablePath);
}

TEST_F(liblocate_test, getBundlePath_NoReturn)
{
    getBundlePath(nullptr, nullptr);

    SUCCEED();
}

TEST_F(liblocate_test, getBundlePath_Return)
{
#ifdef SYSTEM_DARWIN
    char * bundlePath = 0x0;
    unsigned int length = 0;

    getBundlePath(&bundlePath, &length);

    EXPECT_LT(0, length);
    EXPECT_FALSE(bundlePath == 0x0);
    EXPECT_EQ(0, bundlePath[length]);
    EXPECT_EQ(length, strlen(bundlePath));

    free(bundlePath);
#else
    SUCCEED();
#endif
}

TEST_F(liblocate_test, getModulePath_NoReturn)
{
    getModulePath(nullptr, nullptr);

    SUCCEED();
}

TEST_F(liblocate_test, getModulePath_Return)
{
    char * modulePath = 0x0;
    unsigned int length = 0;

    getModulePath(&modulePath, &length);

    EXPECT_LT(0, length);
    EXPECT_FALSE(modulePath == 0x0);
    EXPECT_EQ(0, modulePath[length]);
    EXPECT_EQ(length, strlen(modulePath));

    free(modulePath);
}

TEST_F(liblocate_test, getLibraryPath_NoReturn)
{
    getLibraryPath(reinterpret_cast<void*>(getExecutablePath), nullptr, nullptr);

    SUCCEED();
}

TEST_F(liblocate_test, getLibraryPath_Return)
{
    char * libraryPath = 0x0;
    unsigned int length = 0;

    getLibraryPath(reinterpret_cast<void*>(getExecutablePath), &libraryPath, &length);

    EXPECT_LT(0, length);
    EXPECT_FALSE(libraryPath == 0x0);
    EXPECT_EQ(0, libraryPath[length]);
    EXPECT_EQ(length, strlen(libraryPath));

    free(libraryPath);
}

TEST_F(liblocate_test, locatePath_NoReturn)
{
    const char * relPath = "source/version.h.in";
    const char * systemPath = "share/liblocate";

    locatePath(nullptr, nullptr, relPath, strlen(relPath), systemPath, strlen(systemPath), reinterpret_cast<void*>(getExecutablePath)); // 111
    locatePath(nullptr, nullptr, relPath, strlen(relPath), systemPath, strlen(systemPath), nullptr);                                    // 110
    locatePath(nullptr, nullptr, relPath, strlen(relPath), nullptr, 0, reinterpret_cast<void*>(getExecutablePath));                     // 101
    locatePath(nullptr, nullptr, relPath, strlen(relPath), nullptr, 0, nullptr);                                                        // 100
    locatePath(nullptr, nullptr, nullptr, 0, systemPath, strlen(systemPath), reinterpret_cast<void*>(getExecutablePath));               // 011
    locatePath(nullptr, nullptr, nullptr, 0, systemPath, strlen(systemPath), nullptr);                                                  // 010
    locatePath(nullptr, nullptr, nullptr, 0, nullptr, 0, reinterpret_cast<void*>(getExecutablePath));                                   // 001
    locatePath(nullptr, nullptr, nullptr, 0, nullptr, 0, nullptr);                                                                      // 000

    SUCCEED();
}

TEST_F(liblocate_test, locatePath_Return)
{
    char * path = 0x0;
    unsigned int length = 0;

    const char * relPath = "source/version.h.in";
    const char * systemPath = "share/liblocate";

    locatePath(&path, &length, relPath, strlen(relPath), systemPath, strlen(systemPath), reinterpret_cast<void*>(getExecutablePath));

    EXPECT_LT(0, length);
    EXPECT_FALSE(path == 0x0);
    EXPECT_EQ(0, path[length]);
    EXPECT_EQ(length, strlen(path));

    free(path);
}

TEST_F(liblocate_test, locatePath_ReturnNoSymbol)
{
    char * path = 0x0;
    unsigned int length = 0;

    const char * relPath = "source/version.h.in";
    const char * systemPath = "share/liblocate";

    locatePath(&path, &length, relPath, strlen(relPath), systemPath, strlen(systemPath), nullptr);

    EXPECT_LT(0, length);
    EXPECT_FALSE(path == 0x0);
    EXPECT_EQ(0, path[length]);
    EXPECT_EQ(length, strlen(path));

    free(path);
}

TEST_F(liblocate_test, locatePath_ReturnNoSystemDir)
{
    char * path = 0x0;
    unsigned int length = 0;

    const char * relPath = "source/version.h.in";

    locatePath(&path, &length, relPath, strlen(relPath), nullptr, 0, reinterpret_cast<void*>(getExecutablePath));

    EXPECT_LT(0, length);
    EXPECT_FALSE(path == 0x0);
    EXPECT_EQ(0, path[length]);
    EXPECT_EQ(length, strlen(path));

    free(path);
}
