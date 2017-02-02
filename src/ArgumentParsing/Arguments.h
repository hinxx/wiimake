#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <stdint.h>
#include <vector>
#include <string>
#include <utility>

/* single region of memory (struct used for sorting) */
struct MemRegion
{
    uint32_t start, end;

    MemRegion(uint32_t a, uint32_t b) : start(a), end(b) {}

    MemRegion(std::string a, std::string b) :
        start(stoul(a, nullptr, 16)), end(stoul(b, nullptr, 16)) {}
    
    bool operator<(const MemRegion& other)
    {
        return end - start < other.end - other.start;
    }

    bool operator==(const MemRegion& other)
    {
        return end - start == other.end - other.start;
    }
};

struct Arguments
{
    /* name of library/save/load file */
    std::string name;

    /* address to read from iso */
    std::string address;

    /* configuration file */
    std::string configFile;
    
    /* save temporary files */
    bool saveTemps;

    /* entry symbol */
    std::string entry = "";

    /* inject point */
    uint32_t injectAddress = 0, originalInstruction = 0;
    
    /* source and lib files, include paths */
    std::vector<std::string> sources, libs, includePaths;

    /* compiler/linker flags */
    std::vector<std::string> compileFlags, linkFlags;

    /* available memory regions */
    std::vector<MemRegion> memRegions;

    /* static overwrites to make */
    std::vector< std::pair<uint32_t, uint32_t> > staticOverwrites;
};

#endif
