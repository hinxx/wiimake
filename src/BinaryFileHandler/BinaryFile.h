#ifndef WIIMAKE_BINARY_FILE_H
#define WIIMAKE_BINARY_FILE_H

#include <stdint.h>
#include <vector>
#include <fstream>

struct big32_t
{
    uint8_t byte_1;
    uint8_t byte_2;
    uint8_t byte_3;
    uint8_t byte_4;

    big32_t() : byte_1(0), byte_2(0), byte_3(0), byte_4(0) {}

    big32_t(uint32_t ul)
    {
        byte_4 = ul % 0x100;
        ul -= byte_4;
        ul /= 0x100;

        byte_3 = ul % 0x100;
        ul -= byte_3;
        ul /= 0x100;

        byte_2 = ul % 0x100;
        ul -= byte_2;
        ul /= 0x100;

        byte_1 = ul % 0x100;
    }

    uint32_t value() const
    {
        return byte_1 * 0x01000000 + byte_2 * 0x00010000
            + byte_3 * 0x00000100 + byte_4;
    }
};

struct DolSection
{
    uint32_t offset; // address in file 
    uint32_t start_address; // address once loaded into RAM
    uint32_t size; // size of section, i.e. number of bytes

    std::vector<uint32_t> code; // assembly code of this section

    DolSection() {}

    DolSection(uint32_t o, uint32_t a, uint32_t s)
        : offset(o), start_address(a), size(s) {}

    bool operator<(const DolSection& other) const
    {
        return start_address < other.start_address;
    }
};

struct DolTable
{
    std::vector<DolSection> table;
    uint32_t bss_address;
    uint32_t bss_size;
    uint32_t entry_point;
};

class BinaryFile
{
private:

    // file stream
    std::fstream* mFile;

public:

    // table outlining DOL structure
    DolTable dol_table;

    // constructor from file path
    BinaryFile(std::string);

    // destructor - close file stream
    ~BinaryFile();

    // populate dol table with values from file starting at offset
    void populateDolTable(uint32_t);

    void getSectionCode();

    // find DOL offset corresponding to RAM address
    uint32_t dolOffset(uint32_t) const;

    // read from file offset
    uint32_t read(uint32_t) const;

    // write 32-bit value to file offset
    void write(uint32_t, uint32_t);
};

#endif
