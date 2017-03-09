#include "Global.h"
#include "ISO.h"

#include <stdint.h>
#include <vector>
#include <fstream>
#include <algorithm>
#include <iostream>

/* constructor from file path //TODO: check for valid iso (throw error) */
ISO::ISO(std::string path)
{
    /* open up file stream */
    mFile = new std::fstream(path, std::ios::in | std::ios::out
        | std::ios::binary);

    /* find start of DOL */
    mStartDOL = read(0x0420, false);

    /* populate DOL table */
    uint32_t dol, ram, size;
    for (uint32_t offset = 0; offset < 0x47; offset += 0x04)
    {
        /* get section info */
        dol  = read(mStartDOL + offset, false);
        ram  = read(mStartDOL + offset + 0x48, false);
        size = read(mStartDOL + offset + 0x90, false);

        /* add section if non-zero size */
        if (size > 0)
        {
            mDOLtable.push_back(IsoSection(dol, ram, size));
        }
    }

    /* sort DOL table on RAM addresses */
    std::sort(mDOLtable.begin(), mDOLtable.end());

    /* get code start and end */
    mCodeStart = mDOLtable.front().RAMaddress;
    mCodeEnd = mDOLtable.back().RAMaddress + mDOLtable.back().size;
}

/* destructor - close file stream */
ISO::~ISO()
{
    /* close file stream */
    mFile->close();
    delete mFile;
}

/* find DOL offset corresponding to RAM address */
uint32_t ISO::dolOffset(uint32_t address) const
{
    /* check if pure iso offset */
    if (address < 0x80000000)
    {
        return address;
    }

    /* find section that contains this address */
    unsigned i = 0;
    while (address > mDOLtable[i].RAMaddress + mDOLtable[i].size) {++i;} 

    /* return offset */
    return mStartDOL + mDOLtable[i].DOLoffset
        + address - mDOLtable[i].RAMaddress;
}

/* issue warnings for unusual addresses */
void ISO::checkAddress(uint32_t address) const
{
    if (address > mCodeEnd)
    {
        std::cout << "\n" << std::hex << address << ": this address "
            "is past the end of the DOL" << std::endl;
    }
    else if (address < 0x80000000)
    {
        std::cout << "\n" << std::hex << address << " is too low for RAM,"
            " will be interpreted\nas a pure offset in the iso file\n"
            << std::endl;
    }
    else if (address < mCodeStart)
    {
        std::cout << "\n" << std::hex << address << ": this address "
            "is before the start of the DOL" << std::endl;
    }
}

/* read 32-bit address */
uint32_t ISO::read(uint32_t address, bool userCall) const
{
    /* errors/warnings enabled when user making call */
    if (userCall) {checkAddress(address);}

    /* put stream at correct address */
    mFile->seekg(dolOffset(address));

    /* read 4 bytes, one at a time */
    IsoValue isoVal;
    mFile->read(reinterpret_cast<char*>(&isoVal.byte_1), sizeof(uint8_t));
    mFile->read(reinterpret_cast<char*>(&isoVal.byte_2), sizeof(uint8_t));
    mFile->read(reinterpret_cast<char*>(&isoVal.byte_3), sizeof(uint8_t));
    mFile->read(reinterpret_cast<char*>(&isoVal.byte_4), sizeof(uint8_t));

    /* return value */
    return isoVal.value();
}
    
/* read 32-bit address */
uint32_t ISO::read(std::string address) const
{
    return read(stoul(address, nullptr, 16));
}

/* write 32-bit value to 32-bit RAM address */
void ISO::write(uint32_t address, uint32_t value, bool userCall)
{
    /* errors/warnings enabled when user making call */
    if (userCall) {checkAddress(address);}

    /* put stream at correct address */
    mFile->seekp(dolOffset(address));

    /* account for endianess */    
    IsoValue isoVal = IsoValue(value);

    /* write value */
    mFile->write(reinterpret_cast<char*>(&isoVal.byte_1), sizeof(uint8_t));
    mFile->write(reinterpret_cast<char*>(&isoVal.byte_2), sizeof(uint8_t));
    mFile->write(reinterpret_cast<char*>(&isoVal.byte_3), sizeof(uint8_t));
    mFile->write(reinterpret_cast<char*>(&isoVal.byte_4), sizeof(uint8_t));
}

/* save the current state of the iso file */
void ISO::saveState(std::string fileName) const
{
    /* open save file */
    std::ofstream saveFile(fileName, std::ios::out | std::ios::binary
        | std::ios::trunc);

    /* write until end of code */
    mFile->seekg(std::ios::beg);
    uint32_t endAddr = dolOffset(mCodeEnd) + 4;
    uint8_t byte;

    /* copy data */
    for (uint32_t addr = 0; addr < endAddr; addr++)
    {
        mFile->read(reinterpret_cast<char*>(&byte), sizeof(uint8_t));
        saveFile.write(reinterpret_cast<char*>(&byte), sizeof(uint8_t));
    }
  
    /* close file */
    saveFile.close();
}

/* load code from save file */
void ISO::loadState(std::string fileName)
{
    /* open up binary file for reading */
    std::ifstream loadFile (fileName, std::ios::in | std::ios::binary);
    loadFile.seekg(std::ios::beg);
    mFile->seekg(std::ios::beg);

    uint8_t byte = 0;
    while (loadFile.read(reinterpret_cast<char*>(&byte), sizeof(uint8_t)))
    {
        mFile->write(reinterpret_cast<char*>(&byte), sizeof(uint8_t));
    }

    /* close file */
    loadFile.close();
}

/* inject code into iso */
void ISO::injectCode(const std::vector< std::pair<uint32_t, uint32_t> >&
code)
{
    /* loop through code and write each (address, value) pair */
    for (auto& line : code)
    {
        write(line.first, line.second);
    }
}

/* calculate check sum of iso file */
uint64_t ISO::checkSum()
{
    uint64_t sum = 0, addr = 0;
    uint8_t data[32768];
    size_t dataSize = 32768 * sizeof(uint8_t);

    mFile->seekg(0, std::ios::beg);

    while (mFile->read(reinterpret_cast<char*>(&data[0]), dataSize))
    {
        for (auto& d : data) { sum += d * addr++; }
    }
    return sum;
}

