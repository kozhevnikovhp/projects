/*
 *  verbosity.cpp
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#include "verbosity.h"

int g_VerbosityLevel = 0;

Verbose::Verbose(int verboseLevel, const std::string &name)
{
    verboseLevel_ = verboseLevel;
    if (g_VerbosityLevel >= verboseLevel_)
    {
        name_ = name;
        printf("\t------>> Enter %s ------>>\n", name_.c_str());
    }
}

//virtual
Verbose::~Verbose()
{
    if (g_VerbosityLevel >= verboseLevel_)
        printf("\t<<------ Leave %s <<------\n", name_.c_str());
}
