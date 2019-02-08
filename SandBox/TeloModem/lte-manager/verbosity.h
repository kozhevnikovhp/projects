/*
 *  verbosity.h
 *  
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#pragma once

#include <string>

extern int g_VerbosityLevel;

class Verbose
{
public:
    Verbose(int verboseLevel, const std::string &name);
    virtual ~Verbose();

private:
    int verboseLevel_;
    std::string name_;
};


#define VERBOSE(LEVEL, NAME) Verbose(LEVEL, NAME)
// define it "null"empty" to avoid unnecessary object construction-destruction and string operations when it is not necessary (in production)
// otherwise, verbosity is controlled by -v, -vv, -vvv options
//#define VERBOSE(LEVEL, NAME)
