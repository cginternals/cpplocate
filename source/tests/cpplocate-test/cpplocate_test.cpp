
#include <gmock/gmock.h>

#include <cpplocate/cpplocate.h>


class cpplocate_test : public testing::Test
{
public:
    cpplocate_test()
    {
    }
};

TEST_F(cpplocate_test, getExecutablePath_Return)
{
    const auto result = cpplocate::getExecutablePath();

    EXPECT_LT(0, result.size());
    EXPECT_FALSE(result.c_str() == 0x0);
}

TEST_F(cpplocate_test, getBundlePath_Return)
{
#ifdef SYSTEM_DARWIN
    const auto result = cpplocate::getBundlePath();

    EXPECT_LT(0, result.size());
    EXPECT_NE(0x0, result.c_str());
#else
    SUCCEED();
#endif
}

TEST_F(cpplocate_test, getModulePath_Return)
{
    const auto result = cpplocate::getModulePath();

    EXPECT_LT(0, result.size());
    EXPECT_FALSE(result.c_str() == 0x0);
}

TEST_F(cpplocate_test, getLibraryPath_Return)
{
    const auto result = cpplocate::getLibraryPath(reinterpret_cast<void*>(cpplocate::getExecutablePath));

    EXPECT_LT(0, result.size());
    EXPECT_NE(nullptr, result.c_str());
}

TEST_F(cpplocate_test, locatePath_Return)
{
    const auto relPath = std::string("source/version.h.in");
    const auto systemPath = std::string("share/liblocate");

    const auto result = cpplocate::locatePath(relPath, systemPath, reinterpret_cast<void*>(cpplocate::getExecutablePath));

    EXPECT_LT(0, result.size());
    EXPECT_NE(nullptr, result.c_str());
}
