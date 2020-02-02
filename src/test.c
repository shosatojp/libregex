#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "libregex.h"
#include "testlib.h"

void test() {
    REGEX_TEST("a", "", "a")
    REGEX_TEST("1", "", "1")
    REGEX_TEST_FAIL("1", "", "a")
    REGEX_TEST("\\d", "", "3")
    REGEX_TEST("\\\\", "", "\\")
    REGEX_TEST("\\w+", "", "hello")
    REGEX_TEST("\\w+", "", "hello!")
    REGEX_TEST("^\\w", "", "hoge10")
    REGEX_TEST("^\\w", "", "10hoge")
    REGEX_TEST("\\w+$", "", "10hoge")
    REGEX_TEST("\\w+$", "", "hoge10")

    REGEX_TEST("[0-9]", "", "3")
    REGEX_TEST("[^0-9]", "", "a")

    REGEX_TEST_FAIL("\\d{3,5}", "", "34")
    REGEX_TEST("\\d{3,5}", "", "345")

    REGEX_TEST("(\\d+)", "", "123")
    REGEX_TEST("(\\d+)?", "", "")
    REGEX_TEST("(\\d+)?", "", "123")

    REGEX_TEST("\\d+", "", "123")  // 5a
    REGEX_TEST("\\d*", "", "a")    // 5a

    REGEX_TEST("\\d*", "", "a")  // 5a

    REGEX_TEST("(\\d+)", "", "1")
    REGEX_TEST_FIND_ALL("\\d+", "", "1s2", "1", "2")
}

int main() {
    test();
    REGEX_TEST_FIND_ALL("([\\w.-]+)@([\\w.-]+)", "", "<a href=\"mailto:hoge@example.com\">Send Mail</a>", "hoge@example.com")
    REGEX_TEST_FIND_ALL("0x[\\dA-F]{2}", "", "hoge0x340x12 hige0x5678", "0x34", "0x12", "0x56")
    REGEX_TEST_FIND_ALL("<.*>", "i", "<H1>hoge</h1>", "<H1>hoge</h1>")
    REGEX_TEST_FIND_ALL("<.*?>", "i", "<H1>hoge</h1>", "<H1>", "</h1>")

    REGEX_TEST_FIND_ALL("Apple?", "i", "apple Appl APPLEE", "apple", "Appl", "APPLE")

    REGEX_TEST_CAPTURE("(\\d+)", "", "222,234", "222")
    REGEX_TEST_CAPTURE("(.*)@(.*)", "", "hoge@example.com", "hoge", "example.com")
    REGEX_TEST_CAPTURE("(.*)@(?:.*)", "", "hoge@example.com", "hoge")

    REGEX_TEST_FIND_ALL("^\\d+", "m", "123\n4\nXXX56\n", "123", "4")

    REGEX_TEST_FIND_ALL("hello", "i", "hello Hello HELLO", "hello", "Hello", "HELLO")
    REGEX_TEST_FIND_ALL("(\\d+|[a-z]+)", "", "123abc456def", "123", "abc", "456", "def")
    REGEX_TEST_END
}