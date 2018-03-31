
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
    char * str1 = "A-string";

    unifiedPath(str1, strlen(str1));

    EXPECT_STREQ(str1, "A-string");
}

TEST_F(utils_test, unifiedPath_UnixPath)
{
    char * str1 = "/usr/include/c++/v1/tuple";

    unifiedPath(str1, strlen(str1));

    EXPECT_STREQ(str1, "/usr/include/c++/v1/tuple");
}

TEST_F(utils_test, unifiedPath_WindowsPath)
{
    char * str1 = "C:\\dev\\include\\c++\\v1\\tuple";

    unifiedPath(str1, strlen(str1));

    EXPECT_STREQ(str1, "C:/dev/include/c++/v1/tuple");
}
