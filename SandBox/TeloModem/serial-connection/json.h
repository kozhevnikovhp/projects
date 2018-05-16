#ifndef JSON_H
#define JSON_H

#include <string>
#include <vector>

typedef std::pair<std::string, std::string> KeyValue;
typedef std::vector<KeyValue> JsonContent;
typedef std::vector<const char *> Dictionary;

bool parseToContent(const std::string &raw, JsonContent &content, const Dictionary &dictionary);
bool toJSON(const JsonContent &content, std::string &json);

#endif // JSON_H
