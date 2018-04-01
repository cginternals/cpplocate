
#include <gmock/gmock.h>

#include <liblocate/utils.h>


class utils_test : public testing::Test
{
public:
    utils_test()
    {
    }
};

TEST_F(utils_test, unifiedPath_EmptyPath)
{
    unifiedPath(nullptr, 0);

    SUCCEED();
}

TEST_F(utils_test, unifiedPath_NoPath)
{
    const char * source = "A-string";
    const char * expected = "A-string";

    const unsigned int length = strlen(source);
    char * actual = reinterpret_cast<char*>(malloc(sizeof(char) * length+1));
    memcpy(actual, source, length+1);

    unifiedPath(actual, length);

    EXPECT_STREQ(expected, actual);
}

TEST_F(utils_test, unifiedPath_UnixPath)
{
    const char * source = "/usr/include/c++/v1/tuple";
    const char * expected = "/usr/include/c++/v1/tuple";

    const unsigned int length = strlen(source);
    char * actual = reinterpret_cast<char*>(malloc(sizeof(char) * length+1));
    memcpy(actual, source, length+1);

    unifiedPath(actual, length);

    EXPECT_STREQ(expected, actual);
}

TEST_F(utils_test, unifiedPath_WindowsPath)
{
    const char * source = "C:\\dev\\include\\c++\\v1\\tuple";
    const char * expected = "C:/dev/include/c++/v1/tuple";

    const unsigned int length = strlen(source);
    char * actual = reinterpret_cast<char*>(malloc(sizeof(char) * length+1));
    memcpy(actual, source, length+1);

    unifiedPath(actual, length);

    EXPECT_STREQ(expected, actual);
}

TEST_F(utils_test, getDirectoryPath_EmptyPath)
{
    unsigned int newLength = 10;

    getDirectoryPath(nullptr, 0, &newLength);

    EXPECT_EQ(0, newLength);
}

TEST_F(utils_test, getDirectoryPath_NoPath)
{
    const char * source = "A-string";
    const unsigned int length = strlen(source);
    unsigned int newLength = 0;

    getDirectoryPath(source, length, &newLength);

    EXPECT_EQ(8, newLength);
}

TEST_F(utils_test, getDirectoryPath_UnixPath)
{
    const char * source = "/usr/include/c++/v1/tuple";
    const unsigned int length = strlen(source);
    unsigned int newLength = 0;

    getDirectoryPath(source, length, &newLength);

    EXPECT_EQ(20, newLength);
}

TEST_F(utils_test, getDirectoryPath_WindowsPath)
{
    const char * source = "C:\\dev\\include\\c++\\v1\\tuple";
    const unsigned int length = strlen(source);
    unsigned int newLength = 0;

    getDirectoryPath(source, length, &newLength);

    EXPECT_EQ(22, newLength);
}

TEST_F(utils_test, getSystemBasePath_EmptyPath)
{
    getSystemBasePath(nullptr, 0, nullptr);

    SUCCEED();
}

TEST_F(utils_test, getSystemBasePath_NoPath)
{
    const char * source = "A-string";
    const unsigned int length = strlen(source);
    unsigned int newLength = 0;

    getSystemBasePath(source, length, &newLength);

    EXPECT_EQ(0, newLength);
}

TEST_F(utils_test, getSystemBasePath_SystemPath)
{
    const char * source = "/usr/local/lib/cpplocate";
    const unsigned int length = strlen(source);
    unsigned int newLength = 0;

    getSystemBasePath(source, length, &newLength);

    EXPECT_EQ(11, newLength);
}

TEST_F(utils_test, getSystemBasePath_ChrootPath)
{
    const char * source = "/home/user/dev/deploy/usr/local/lib/cpplocate";
    const unsigned int length = strlen(source);
    unsigned int newLength = 0;

    getSystemBasePath(source, length, &newLength);

    EXPECT_EQ(32, newLength);
}
