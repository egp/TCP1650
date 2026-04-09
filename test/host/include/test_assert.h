#ifndef TEST_ASSERT_H
#define TEST_ASSERT_H

#include <cstdlib>
#include <iostream>

#define TEST_ASSERT_TRUE(expr)                                                     \
  do {                                                                             \
    if (!(expr)) {                                                                 \
      std::cerr << "ASSERT_TRUE failed: " #expr << " at " << __FILE__ << ':'      \
                << __LINE__ << std::endl;                                          \
      std::exit(1);                                                                \
    }                                                                              \
  } while (0)

#define TEST_ASSERT_EQ(expected, actual)                                           \
  do {                                                                             \
    const auto expected_value = (expected);                                        \
    const auto actual_value = (actual);                                            \
    if (!(expected_value == actual_value)) {                                       \
      std::cerr << "ASSERT_EQ failed at " << __FILE__ << ':' << __LINE__          \
                << " expected=" << expected_value                                 \
                << " actual=" << actual_value << std::endl;                       \
      std::exit(1);                                                                \
    }                                                                              \
  } while (0)

#endif
