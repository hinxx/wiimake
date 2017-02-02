#include "catch.hpp"

#include "../src/HighLevel/HighLevel.h"
#include "HeaderDisplay.h"

static const std::string prefix = "../tests/files/CodeSections/";

TEST_CASE("get section names and sizes")
{
    /* display header in first test case */
    displayHeader("Testing CodeSections.cpp");

    /* create file list */
    FileList files;
    files.push_back(prefix + "object1.o");
    files.push_back(prefix + "object2.o");
    files.push_back(prefix + "lib1.a");

    /* get section names */
    SectionList sections;
    REQUIRE_NOTHROW(CodeSections::storeNames(sections, files));

    REQUIRE(sections.size() == 13);        
    REQUIRE(sections[0].path == prefix + "object1.o (text)");
    REQUIRE(sections[1].path == prefix + "object1.o (rodata)");
    REQUIRE(sections[2].path == prefix + "object1.o (attr)");
    REQUIRE(sections[3].path == prefix + "object2.o (text)");
    REQUIRE(sections[4].path == prefix + "object2.o (attr)");
    REQUIRE(sections[5].path == prefix + "lib1.a (text0)");
    REQUIRE(sections[6].path == prefix + "lib1.a (attr0)");
    REQUIRE(sections[7].path == prefix + "lib1.a (text1)");
    REQUIRE(sections[8].path == prefix + "lib1.a (rodata1)");
    REQUIRE(sections[9].path == prefix + "lib1.a (attr1)");
    REQUIRE(sections[10].path == prefix + "lib1.a (text2)");
    REQUIRE(sections[11].path == prefix + "lib1.a (rodata2)");
    REQUIRE(sections[12].path == prefix + "lib1.a (attr2)");

    /* get section sizes */
    REQUIRE_NOTHROW(CodeSections::storeSizes(sections, "_main"));

    REQUIRE(sections.size() == 13);
    REQUIRE(sections[0].size == 0xc0);
    REQUIRE(sections[1].size == 0x14);
    REQUIRE(sections[2].size == 0x10);
    REQUIRE(sections[3].size == 0x2c);
    REQUIRE(sections[4].size == 0x10);
    REQUIRE(sections[5].size == 0);
    REQUIRE(sections[6].size == 0);
    REQUIRE(sections[7].size == 0x258);
    REQUIRE(sections[8].size == 0x3c);
    REQUIRE(sections[9].size == 0x10);
    REQUIRE(sections[10].size == 0);
    REQUIRE(sections[11].size == 0);
    REQUIRE(sections[12].size == 0);
}
