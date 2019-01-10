/*
 *  json.h
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#pragma once

#include <string>
#include <vector>

typedef std::pair<std::string, std::string> KeyValue;
typedef std::vector<KeyValue> JsonContent;
typedef std::pair<const char *, const char *> DictionaryEntry;
typedef std::vector<DictionaryEntry> Dictionary;

bool parseToContent(const std::string &raw, JsonContent &content, const Dictionary &dictionary);
bool toJSON(const JsonContent &content, std::string &json);

