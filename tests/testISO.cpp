#include "catch.hpp"

#include "HeaderDisplay.h"
#include "ISO.h"
#include "LowLevel.h"

static const std::string path = "../tests/files/ISO/";

TEST_CASE("test all functions in ISO.cpp")
{
    /* display header in first test case */
    displayHeader("Testing ISO.cpp");

    /* creat iso for testing */
    System::runCMD(System::rm + " " + path + "test.iso");
    System::runCMD(System::cp + " ../tests/files/reference.iso "
        + path + "test.iso");

    /* create iso object */
    ISO iso(path + "test.iso");

    /* check DOL offset calculations */
    REQUIRE(iso.fileOffset(0x80003150) == 0x1e950);
    REQUIRE(iso.fileOffset(0x80005570) == 0x3d2670);
    REQUIRE(iso.fileOffset(0x80005950) == 0x20d30);
    REQUIRE(iso.fileOffset(0x803b7290) == 0x3d2a90);
    REQUIRE(iso.fileOffset(0x804d79f0) == 0x44fbd0);

    /** test iso read **/
    REQUIRE(iso.read(iso.fileOffset(0x80003100)) == 0x7c0802a6);
    REQUIRE(iso.read(iso.fileOffset(0x80003150)) == 0x41820018);
    REQUIRE(iso.read(iso.fileOffset(0x80005570)) == 0x20080000);
    REQUIRE(iso.read(iso.fileOffset(0x800056e0)) == 0x80005530);
    REQUIRE(iso.read(iso.fileOffset(0x80006000)) == 0xc0e10054);
    REQUIRE(iso.read(iso.fileOffset(0x803b8000)) == 0xc091999a);
    REQUIRE(iso.read(iso.fileOffset(0x804d4000)) == 0x6a6f626a);
    REQUIRE(iso.read(iso.fileOffset(0x804deb00)) == 0x40000000);

    /* write value */
    iso.write(iso.fileOffset(0x80006000), 0x1234abcd);
    REQUIRE(iso.read(iso.fileOffset(0x80006000)) == 0x1234abcd);

    /* replace with original code */
    iso.write(iso.fileOffset(0x80006000), 0xc0e10054);
    REQUIRE(iso.read(iso.fileOffset(0x80006000)) == 0xc0e10054);

    /* create code */
    ASMcode code;
    code.push_back(std::make_pair(0x80003100, 0x1234abcd));
    code.push_back(std::make_pair(0x80006000, 0x1234abcd));
    code.push_back(std::make_pair(0x803b8000, 0x1234abcd));

    /* inject code */
    iso.injectCode(code);
        
    /* check that values changed */
    REQUIRE(iso.read(iso.fileOffset(0x80003100)) == 0x1234abcd);
    REQUIRE(iso.read(iso.fileOffset(0x80006000)) == 0x1234abcd);
    REQUIRE(iso.read(iso.fileOffset(0x803b8000)) == 0x1234abcd);

    /* revert values to original */
    iso.write(iso.fileOffset(0x80003100), 0x7c0802a6);
    iso.write(iso.fileOffset(0x80006000), 0xc0e10054);
    iso.write(iso.fileOffset(0x803b8000), 0xc091999a);
}

    

