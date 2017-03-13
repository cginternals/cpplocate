
#include <gmock/gmock.h>


#include <cpplocate/utils.h>


class replace_test: public testing::Test
{
public:
};

TEST_F(replace_test, testReplace)
{
    auto s0 = std::string("ThiS iS a Small teSt");
    auto s1 = std::string("Hello %1, why you're called %1?");

    cpplocate::utils::replace(s0, "S", "s");
    cpplocate::utils::replace(s1, "%1", "CG Internals");

    EXPECT_EQ("This is a small test", s0);
    EXPECT_EQ("Hello CG Internals, why you're called CG Internals?", s1);
}
