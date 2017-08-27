#include "BinaryFile.h"
#include "Global.h"

#include <stdint.h>
#include <vector>
#include <fstream>
#include <algorithm>
#include <iostream>

// constructor
BinaryFile::BinaryFile(std::string path)
{
    // create file stream
    mFile = new std::fstream(path, std::ios::in | std::ios::out
        | std::ios::binary);
}

// destructor
BinaryFile::~BinaryFile()
{
    mFile->close();
    delete mFile;
}

// populate dol table with values from file
void BinaryFile::populateDolTable()
{
    // get text and data sections
    for (uint32_t offset = mDolStart; offset < mDolStart + 0x47; offset += 0x04)
    {
        // create section and get values from file
        DolSection section;
        section.offset = read(offset);
        section.start_address = read(offset + 0x48);
        section.size = read(offset + 0x90);

        // add to table
        mDolInfo.table.push_back(section); 
    }
    RUNTIME_ERROR(mDolInfo.table.size() != 18, "invalid DOL table");

    // get bss and entry point info
    mDolInfo.bss_address = read(mDolStart + 0xd8);
    mDolInfo.bss_size = read(mDolStart + 0xdc);
    mDolInfo.entry_point = read(mDolStart + 0xe0);
}

void BinaryFile::getSectionCode()
{
    for (auto& section : mDolInfo.table)
    {
        uint32_t offset = section.offset;
        for (; offset < section.offset + section.size; offset += 0x04)
            section.code.push_back(read(offset + mDolStart));
    }
}

// find file offset corresponding to RAM address (look up in dol table)
uint32_t BinaryFile::fileOffset(uint32_t address) const
{
    // find section that contains this address
    auto section = std::find_if(mDolInfo.table.begin(), mDolInfo.table.end(),
        [&](const DolSection& dolSection)
        {
            return address >= dolSection.start_address &&
                address < dolSection.start_address + dolSection.size;
        });

    // address wasn't found
    RUNTIME_ERROR(section == mDolInfo.table.end(), "address not found in dol table");

    // return offset
    return address - section->start_address + section->offset + mDolStart;
}

// big endian 32-bit read from file offset
uint32_t BinaryFile::read(uint32_t offset) const
{
    // put stream at correct offset
    mFile->seekg(offset);

    // read 4 bytes, one at a time
    big32_t val;
    mFile->read(reinterpret_cast<char*>(&val.byte_1), sizeof(uint8_t));
    mFile->read(reinterpret_cast<char*>(&val.byte_2), sizeof(uint8_t));
    mFile->read(reinterpret_cast<char*>(&val.byte_3), sizeof(uint8_t));
    mFile->read(reinterpret_cast<char*>(&val.byte_4), sizeof(uint8_t));

    // return 32 bit value
    return val.value();
}
    
// big endian 32-bit write to file offset
void BinaryFile::write(uint32_t offset, uint32_t value)
{
    // put stream at correct offset
    mFile->seekp(offset);

    // account for endianess
    big32_t val (value);

    // write value
    mFile->write(reinterpret_cast<char*>(&val.byte_1), sizeof(uint8_t));
    mFile->write(reinterpret_cast<char*>(&val.byte_2), sizeof(uint8_t));
    mFile->write(reinterpret_cast<char*>(&val.byte_3), sizeof(uint8_t));
    mFile->write(reinterpret_cast<char*>(&val.byte_4), sizeof(uint8_t));
}
