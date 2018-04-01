
#include <gmock/gmock.h>

#include <liblocate/utils.h>


class utils_test : public testing::Test
{
public:
    utils_test()
    {
    }
};


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
