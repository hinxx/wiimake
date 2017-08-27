#include "Global.h"
#include "DOL.h"
#include "BinaryFile.h"

#include <stdint.h>
#include <vector>
#include <fstream>
#include <algorithm>
#include <iostream>

// constructor from file path
DOL::DOL(std::string path) : BinaryFile(path)
{
    // store dol start
    mDolStart = 0;

    // populate dol table
    populateDolTable();
}
