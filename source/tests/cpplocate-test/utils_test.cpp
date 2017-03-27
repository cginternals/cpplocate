
#include <gmock/gmock.h>

#include <cpplocate/utils.h>


using namespace cpplocate;


class utils_test : public testing::Test
{
public:
    utils_test()
    {
    }
};


TEST_F(utils_test, posAfterString_once)
{
    const std::string str = "posAfter378String";

    auto val = utils::posAfterString(str, "378");

    ASSERT_EQ(11, val);
}

TEST_F(utils_test, posAfterString_mult)
{
    const std::string str = "pos378After378String";

    auto val = utils::posAfterString(str, "378");

    ASSERT_EQ(14, val);
}

TEST_F(utils_test, posAfterString_not_found)
{
    const std::string str = "pos377After379String";

    auto val = utils::posAfterString(str, "378");

    ASSERT_EQ(std::string::npos, val);
}
