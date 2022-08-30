#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/utils/sc_base64.hpp"

TEST(ScStringUtils, Base64)
{
  std::string const & testString = "OSTIS Technology";
  std::string const & encodedString
      = ScBase64::Encode(reinterpret_cast<sc_uchar const *>(testString.c_str()), testString.size());

  std::string const & decodedString =
      ScBase64::Decode(encodedString);

  EXPECT_EQ(testString, decodedString);
}

TEST(StringUtils, Trim)
{
  std::string trimLeft = "  value";
  std::string trimRight = "value  ";
  std::string trim = " value  ";
  std::string empty = "     ";

  utils::StringUtils::TrimLeft(trimLeft);
  EXPECT_EQ(trimLeft, "value");

  utils::StringUtils::TrimRight(trimRight);
  EXPECT_EQ(trimRight, "value");

  utils::StringUtils::Trim(trim);
  EXPECT_EQ(trim, "value");

  utils::StringUtils::Trim(empty);
  EXPECT_EQ(empty, "");

  std::string value = "  value value \t\n ";
  utils::StringUtils::Trim(value);

  EXPECT_EQ(value, "value value");
}

TEST(StringUtils, Split)
{
  std::vector<std::string> res;

  // Do not return empty item after ;
  utils::StringUtils::SplitString("begin;end;", ';', res);
  EXPECT_EQ(res.size(), 2u);
  EXPECT_EQ("begin", res[0]);
  EXPECT_EQ("end", res[1]);
}

TEST(StringUtils, ParseNumbers)
{
  {
    float resultFloat;
    EXPECT_TRUE(utils::StringUtils::ParseNumber("7.56", resultFloat));
    EXPECT_EQ(resultFloat, 7.56f);
  }

  {
    double resultDouble;
    EXPECT_TRUE(utils::StringUtils::ParseNumber("8.56", resultDouble));
    EXPECT_EQ(resultDouble, 8.56);
  }

  {
    int8_t resultInt8;
    EXPECT_TRUE(utils::StringUtils::ParseNumber("8", resultInt8));
    EXPECT_EQ(resultInt8, 8);
  }

  {
    int16_t resultInt16;
    EXPECT_TRUE(utils::StringUtils::ParseNumber("16", resultInt16));
    EXPECT_EQ(resultInt16, 16);
  }

  {
    int32_t resultInt32;
    EXPECT_TRUE(utils::StringUtils::ParseNumber("32", resultInt32));
    EXPECT_EQ(resultInt32, 32);
  }


  {
    int64_t resultInt64;
    EXPECT_TRUE(utils::StringUtils::ParseNumber("64", resultInt64));
    EXPECT_EQ(resultInt64, 64);
  }

  {
    uint8_t resultUint8;
    EXPECT_TRUE(utils::StringUtils::ParseNumber("8", resultUint8));
    EXPECT_EQ(resultUint8, 8u);
  }

  {
    uint16_t resultUint16;
    EXPECT_TRUE(utils::StringUtils::ParseNumber("16", resultUint16));
    EXPECT_EQ(resultUint16, 16u);
  }

  {
    uint32_t resultUint32;
    EXPECT_TRUE(utils::StringUtils::ParseNumber("32", resultUint32));
    EXPECT_EQ(resultUint32, 32u);
  }

  {
    uint64_t resultUint64;
    EXPECT_TRUE(utils::StringUtils::ParseNumber("64", resultUint64));
    EXPECT_EQ(resultUint64, 64u);
  }
}
