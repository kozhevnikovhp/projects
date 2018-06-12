#include "json.h"

#include <string.h>
#include <ctype.h>
#include <algorithm>

// trim from start (in place) (leading spaces)
static inline void ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c) { return !isspace(c); }));
}

// trim from end (in place)
static inline void rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int c) { return !isspace(c); }).base(), s.end());
}

bool parseToContent(const std::string &raw, JsonContent &content, const Dictionary &dictionary)
{
    //printf("raw: %s\n", raw.c_str());
    std::string key, value;
    char *pszInput = strdup(raw.c_str()); // TODO: do something better to avoid strdup/free
    const char *pszSeparator = ":\t";
    // replace two subsequent blanks with tab - new separator
    for (int i = 1; i < strlen(pszInput); ++i)
        if (pszInput[i-1] == ' ' && pszInput[i] == ' ')
            pszInput[i-1] = pszInput[i] = '\n';

    char *pszToken = strtok(pszInput, pszSeparator);
    while (pszToken)
    {
        //printf("token %s length = %d\n", pszToken, strlen(pszToken));
        key = pszToken;
        ltrim(key);
        rtrim(key);
        if (!key.empty())
        {
            //printf("key = %s\n", key.c_str());
            pszToken = strtok(NULL, "\n\r\t");
            if (pszToken)
            {
                //printf("\ttoken %s length = %d\n", pszToken, strlen(pszToken));

                // find key value in dictionary. It is assumed that the dictionary would be very short, 1-5 items, so linear search is implemented
                const char *pszFoundInDictionary = NULL;
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
                    ltrim(value);
                    rtrim(value);
                    //printf("\tvalue = %s\n", value.c_str());
                    content.emplace_back(KeyValue(key, value));
                }
            }
        }
        pszToken = strtok(NULL, pszSeparator);
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

