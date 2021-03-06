#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include <fstream>
#include <iterator>

#include "Arguments.h"
#include "../Global.h"

typedef std::istreambuf_iterator<std::string> fileIterator;

namespace CMDparser
{
    /* return a list of tokens from the command line arguments */
    TokenList getTokens(int, const char**);

    /* parse command line options */
    void parseMetaOptions(TokenList&);

    /* check if user passed in option */
    bool optionExists(TokenList&, std::string);
}

namespace ConfigParser
{
    /* parse config file */
    void parse(Arguments&);

    /* parse single line of file, add to token list */
    void parseLine(std::string, TokenList&);

    /* convert file to vector of strings */
    TokenList getTokens(std::string);

    /* store all variables in file */
    void storeAllVariables(TokenList&, Arguments&);

    /* store a single variable */
    void storeVariable(Arguments&, std::string, TokenList);

    /* store static overwrite given, default call, need to handle errors */
    void storeStaticOverwrite(Arguments&, std::string, TokenList);

    /* store the memory regions variable */
    void storeMemRegions(Arguments&, TokenList);

    /* store the fixed symbols variable */
    void storeFixedSymbols(Arguments&, TokenList);

    void storeDirectBranches(Arguments&, TokenList);

    void storeLinkerSymbols(Arguments&, TokenList);

    /* verify corect arguments were given in config file */
    void checkArgs(Arguments&);
}

#endif
