/*
 *  json.cpp
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#include <string.h>
#include <ctype.h>
#include <algorithm>

#include "json.h"
#include "misc.h"

bool parseToContent(const std::string &raw, JsonContent &content, const Dictionary &dictionary)
{
    //printf("raw: %s\n", raw.c_str());
    std::string key, value;
    // TODO: get rid of strdup, strtok etc, use std::string and std::iostream stuff instead
    char *pszInput = strdup(raw.c_str()); // TODO: do something better to avoid strdup/free
    const char *pszSeparator = ":\t";
    // replace two subsequent blanks with tab - new separator
    for (unsigned int i = 1; i < strlen(pszInput); ++i)
        if (pszInput[i-1] == ' ' && pszInput[i] == ' ')
            pszInput[i-1] = pszInput[i] = '\n';

    char *pszToken = strtok(pszInput, pszSeparator);
    while (pszToken)
    {
        //printf("token %s length = %d\n", pszToken, strlen(pszToken));
        key = pszToken;
        trimBlanks(key);
        if (!key.empty())
        {
            //printf("key = %s\n", key.c_str());
            pszToken = strtok(nullptr, "\n\r\t");
            if (pszToken)
            {
                //printf("\ttoken %s length = %d\n", pszToken, strlen(pszToken));

                // find key value in dictionary. It is assumed that the dictionary would be very short, 1-5 items, so linear search is implemented
                const char *pszFoundInDictionary = nullptr;
                for (auto &dictionaryEntry : dictionary)
                {
                    if (key.find(dictionaryEntry.first) != std::string::npos)
                    {
                        pszFoundInDictionary = dictionaryEntry.second;
                        break;
                    }
                }
                if (pszFoundInDictionary)
                {
                    //printf("\tpszFoundInDictionary = %s\n", pszFoundInDictionary);
                    key = pszFoundInDictionary;
                    value = pszToken;
                    trimBlanks(value);
                    //printf("\tvalue = %s\n", value.c_str());
                    content.emplace_back(KeyValue(key, value));
                }
            }
        }
        pszToken = strtok(nullptr, pszSeparator);
    }

    free(pszInput);
    return true;
}

bool toJSON(const JsonContent &content, std::string &json)
{
    bool bFirst = true;

    json = "{";
    for (auto &entry : content)
    {
        const std::string &key = entry.first;
        const std::string &value = entry.second;
        if (!bFirst)
            json += ",";
        json += "\n\t";
        json += "\"";
        json += key;
        json += "\" : \"";
        json += value;
        json += "\"";
        bFirst = false;
    }
    json += "\n}\n";

    return true;
}

