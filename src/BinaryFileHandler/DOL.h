#ifndef WIIMAKE_DOL_H
#define WIIMAKE_DOL_H

#include "BinaryFile.h"

#include <stdint.h>
#include <fstream>
#include <vector>
#include <string>

class DOL : public BinaryFile
{
private:

public:

    // constructor from file path
    DOL(std::string);
   
};

#endif
