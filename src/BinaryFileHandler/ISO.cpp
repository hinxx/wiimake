#include "Global.h"
#include "ISO.h"
#include "DOL.h"
#include "BinaryFile.h"

#include <stdint.h>
#include <vector>
#include <fstream>
#include <algorithm>
#include <iostream>

// constructor from file path
ISO::ISO(std::string path) : BinaryFile(path)
{
    // verify iso
    RUNTIME_ERROR(read(0x1c) != 0xc2339f3d,
        "not valid Wii/Gamecube ISO");

    // find start of DOL
    uint32_t dol_start = read(0x0420);

    // populate dol table
    populateDolTable(dol_start);
}

// inject code into iso
void ISO::injectCode(const std::vector< std::pair<uint32_t, uint32_t> >& code)
{
    // loop through code and write each (address, value) pair
    for (auto& line : code)
    {
        write(dolOffset(line.first), line.second);
    }
}

// merge dol into this iso, overwriting a given region in the iso file
void ISO::mergeDOL(DOL& dol, uint32_t region_start, uint32_t region_end)
{
    // store code in DOL
    dol.getSectionCode();

    // find start of DOL
    uint32_t dol_start = read(0x0420);

    // keep track of current position in iso file
    uint32_t file_pos = region_start;

    // create temporary vector of sections
    std::vector<DolSection> all_dol_sections = dol.dol_table.table;

    // add dummy section for the bss
    DolSection bss (0, dol.dol_table.bss_address, dol.dol_table.bss_size);
    bss.code = std::vector<uint32_t>(bss.size / 4, 0);
    all_dol_sections.push_back(bss);

    // write each section of DOL file
    for (auto& dol_section : all_dol_sections)
    {
        // find first available data section in iso
        unsigned i = 0;
        while (dol_table.table[i].size > 0)
        {
            ++i;
        }

        // copy over section information
        dol_table.table[i].start_address = dol_section.start_address;
        dol_table.table[i].size = dol_section.size;
        dol_table.table[i].code = dol_section.code;

        // put this section at current position in file
        dol_table.table[i].offset = file_pos;

        // write new dol section
        uint32_t table_start = read(0x0420);
        write(table_start + 0x04 * i, dol_table.table[i].offset - dol_start);
        write(table_start + 0x04 * i + 0x48, dol_table.table[i].start_address);
        write(table_start + 0x04 * i + 0x90, dol_table.table[i].size);

        // write code to file
        for (auto& instruction : dol_section.code)
        {
            write(file_pos, instruction);
            file_pos += 0x04;
        }

        // add buffer in between sections
        file_pos += 0x08;
    }
        
    // check if wrote past the end of the region
    RUNTIME_ERROR(file_pos > region_end, "provided region too small for dol");
}



