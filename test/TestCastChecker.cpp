#include <diff/CastChecker.h>
#include <gtest/gtest.h>

using namespace diff;

template <typename T, typename U>
static inline void test(bool a, bool b, bool c, bool d, bool e) {
    T data;
    EXPECT_EQ(a, (data = std::numeric_limits<T>::min(),   //
                  IntegralCastChecker::check(data, typeid(U))));
    EXPECT_EQ(b, (data = static_cast<T>(-1),   //
                  IntegralCastChecker::check(data, typeid(U))));
    EXPECT_EQ(c, (data = static_cast<T>(0),   //
                  IntegralCastChecker::check(data, typeid(U))));
    EXPECT_EQ(d, (data = static_cast<T>(1),   //
                  IntegralCastChecker::check(data, typeid(U))));
    EXPECT_EQ(e, (data = std::numeric_limits<T>::max(),   //
                  IntegralCastChecker::check(data, typeid(U))));
}

TEST(TestCastChecker, s8_s8) { test<int8_t, int8_t>(true, true, true, true, true); }
TEST(TestCastChecker, s8_s16) { test<int8_t, int16_t>(false, false, false, false, false); }
TEST(TestCastChecker, s8_s32) { test<int8_t, int32_t>(false, false, false, false, false); }
TEST(TestCastChecker, s8_s64) { test<int8_t, int64_t>(false, false, false, false, false); }

TEST(TestCastChecker, s16_s8) { test<int16_t, int8_t>(false, true, true, true, false); }
TEST(TestCastChecker, s16_s16) { test<int16_t, int16_t>(true, true, true, true, true); }
TEST(TestCastChecker, s16_s32) { test<int16_t, int32_t>(false, false, false, false, false); }
TEST(TestCastChecker, s16_s64) { test<int16_t, int64_t>(false, false, false, false, false); }

TEST(TestCastChecker, s32_s8) { test<int32_t, int8_t>(false, true, true, true, false); }
TEST(TestCastChecker, s32_s16) { test<int32_t, int16_t>(false, true, true, true, false); }
TEST(TestCastChecker, s32_s32) { test<int32_t, int32_t>(true, true, true, true, true); }
TEST(TestCastChecker, s32_s64) { test<int32_t, int64_t>(false, false, false, false, false); }

TEST(TestCastChecker, s64_s8) { test<int64_t, int8_t>(false, true, true, true, false); }
TEST(TestCastChecker, s64_s16) { test<int64_t, int16_t>(false, true, true, true, false); }
TEST(TestCastChecker, s64_s32) { test<int64_t, int32_t>(false, true, true, true, false); }
TEST(TestCastChecker, s64_s64) { test<int64_t, int64_t>(true, true, true, true, true); }

TEST(TestCastChecker, s8_u8) { test<int8_t, uint8_t>(false, false, true, true, true); }
TEST(TestCastChecker, s8_u16) { test<int8_t, uint16_t>(false, false, false, false, false); }
TEST(TestCastChecker, s8_u32) { test<int8_t, uint32_t>(false, false, false, false, false); }
TEST(TestCastChecker, s8_u64) { test<int8_t, uint64_t>(false, false, false, false, false); }

TEST(TestCastChecker, s16_u8) { test<int16_t, uint8_t>(false, false, true, true, false); }
TEST(TestCastChecker, s16_u16) { test<int16_t, uint16_t>(false, false, true, true, true); }
TEST(TestCastChecker, s16_u32) { test<int16_t, uint32_t>(false, false, false, false, false); }
TEST(TestCastChecker, s16_u64) { test<int16_t, uint64_t>(false, false, false, false, false); }

TEST(TestCastChecker, s32_u8) { test<int32_t, uint8_t>(false, false, true, true, false); }
TEST(TestCastChecker, s32_u16) { test<int32_t, uint16_t>(false, false, true, true, false); }
TEST(TestCastChecker, s32_u32) { test<int32_t, uint32_t>(false, false, true, true, true); }
TEST(TestCastChecker, s32_u64) { test<int32_t, uint64_t>(false, false, false, false, false); }

TEST(TestCastChecker, s64_u8) { test<int64_t, uint8_t>(false, false, true, true, false); }
TEST(TestCastChecker, s64_u16) { test<int64_t, uint16_t>(false, false, true, true, false); }
TEST(TestCastChecker, s64_u32) { test<int64_t, uint32_t>(false, false, true, true, false); }
TEST(TestCastChecker, s64_u64) { test<int64_t, uint64_t>(false, false, true, true, true); }

TEST(TestCastChecker, u8_s8) { test<uint8_t, int8_t>(true, false, true, true, false); }
TEST(TestCastChecker, u8_s16) { test<uint8_t, int16_t>(false, false, false, false, false); }
TEST(TestCastChecker, u8_s32) { test<uint8_t, int32_t>(false, false, false, false, false); }
TEST(TestCastChecker, u8_s64) { test<uint8_t, int64_t>(false, false, false, false, false); }

TEST(TestCastChecker, u16_s8) { test<uint16_t, int8_t>(true, false, true, true, false); }
TEST(TestCastChecker, u16_s16) { test<uint16_t, int16_t>(true, false, true, true, false); }
TEST(TestCastChecker, u16_s32) { test<uint16_t, int32_t>(false, false, false, false, false); }
TEST(TestCastChecker, u16_s64) { test<uint16_t, int64_t>(false, false, false, false, false); }

TEST(TestCastChecker, u32_s8) { test<uint32_t, int8_t>(true, false, true, true, false); }
TEST(TestCastChecker, u32_s16) { test<uint32_t, int16_t>(true, false, true, true, false); }
TEST(TestCastChecker, u32_s32) { test<uint32_t, int32_t>(true, false, true, true, false); }
TEST(TestCastChecker, u32_s64) { test<uint32_t, int64_t>(false, false, false, false, false); }

TEST(TestCastChecker, u64_s8) { test<uint64_t, int8_t>(true, false, true, true, false); }
TEST(TestCastChecker, u64_s16) { test<uint64_t, int16_t>(true, false, true, true, false); }
TEST(TestCastChecker, u64_s32) { test<uint64_t, int32_t>(true, false, true, true, false); }
TEST(TestCastChecker, u64_s64) { test<uint64_t, int64_t>(true, false, true, true, false); }

TEST(TestCastChecker, u8_u8) { test<uint8_t, uint8_t>(true, true, true, true, true); }
TEST(TestCastChecker, u8_u16) { test<uint8_t, uint16_t>(false, false, false, false, false); }
TEST(TestCastChecker, u8_u32) { test<uint8_t, uint32_t>(false, false, false, false, false); }
TEST(TestCastChecker, u8_u64) { test<uint8_t, uint64_t>(false, false, false, false, false); }

TEST(TestCastChecker, u16_u8) { test<uint16_t, uint8_t>(true, false, true, true, false); }
TEST(TestCastChecker, u16_u16) { test<uint16_t, uint16_t>(true, true, true, true, true); }
TEST(TestCastChecker, u16_u32) { test<uint16_t, uint32_t>(false, false, false, false, false); }
TEST(TestCastChecker, u16_u64) { test<uint16_t, uint64_t>(false, false, false, false, false); }

TEST(TestCastChecker, u32_u8) { test<uint32_t, uint8_t>(true, false, true, true, false); }
TEST(TestCastChecker, u32_u16) { test<uint32_t, uint16_t>(true, false, true, true, false); }
TEST(TestCastChecker, u32_u32) { test<uint32_t, uint32_t>(true, true, true, true, true); }
TEST(TestCastChecker, u32_u64) { test<uint32_t, uint64_t>(false, false, false, false, false); }

TEST(TestCastChecker, u64_u8) { test<uint64_t, uint8_t>(true, false, true, true, false); }
TEST(TestCastChecker, u64_u16) { test<uint64_t, uint16_t>(true, false, true, true, false); }
TEST(TestCastChecker, u64_u32) { test<uint64_t, uint32_t>(true, false, true, true, false); }
TEST(TestCastChecker, u64_u64) { test<uint64_t, uint64_t>(true, true, true, true, true); }
