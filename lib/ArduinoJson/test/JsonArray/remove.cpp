// Copyright Benoit Blanchon 2014-2017
// MIT License
//
// Arduino JSON library
// https://bblanchon.github.io/ArduinoJson/
// If you like this project, please add a star!

#include <ArduinoJson.h>
#include <gtest/gtest.h>

class JsonArray_Remove_Tests : public ::testing::Test {
 protected:
  JsonArray_Remove_Tests() : _array(_jsonBuffer.createArray()) {
    _array.add("one");
    _array.add("two");
    _array.add("three");
  }

  DynamicJsonBuffer _jsonBuffer;
  JsonArray& _array;
};

#define TEST_(name) TEST_F(JsonArray_Remove_Tests, name)

TEST_(RemoveFirstByIndex) {
  _array.remove(0);

  EXPECT_EQ(2, _array.size());
  EXPECT_STREQ("two", _array[0]);
  EXPECT_STREQ("three", _array[1]);
}

TEST_(RemoveMiddleByIndex) {
  _array.remove(1);

  EXPECT_EQ(2, _array.size());
  EXPECT_STREQ("one", _array[0]);
  EXPECT_STREQ("three", _array[1]);
}

TEST_(RemoveLastByIndex) {
  _array.remove(2);

  EXPECT_EQ(2, _array.size());
  EXPECT_STREQ("one", _array[0]);
  EXPECT_STREQ("two", _array[1]);
}

TEST_(RemoveFirstByIterator) {
  JsonArray::iterator it = _array.begin();
  _array.remove(it);

  EXPECT_EQ(2, _array.size());
  EXPECT_STREQ("two", _array[0]);
  EXPECT_STREQ("three", _array[1]);
}

TEST_(RemoveMiddleByIterator) {
  JsonArray::iterator it = _array.begin();
  ++it;
  _array.remove(it);

  EXPECT_EQ(2, _array.size());
  EXPECT_STREQ("one", _array[0]);
  EXPECT_STREQ("three", _array[1]);
}

TEST_(RemoveLastByIterator) {
  JsonArray::iterator it = _array.begin();
  ++it;
  ++it;
  _array.remove(it);

  EXPECT_EQ(2, _array.size());
  EXPECT_STREQ("one", _array[0]);
  EXPECT_STREQ("two", _array[1]);
}
