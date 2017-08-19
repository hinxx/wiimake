#ifndef WIIMAKE_ISO_H
#define WIIMAKE_ISO_H

#include "DOL.h"
#include "Arguments.h"

#include <stdint.h>
#include <fstream>
#include <vector>
#include <string>

class ISO : public BinaryFile
{
private:

public:

    // constructor file path
    ISO(std::string);

    // inject code into iso
    void injectCode(const std::vector< std::pair<uint32_t, uint32_t> >&);

    // merge dol into this iso, overwriting a given region in the iso file
    void mergeDOL(DOL&, uint32_t, uint32_t);

};

#endif
