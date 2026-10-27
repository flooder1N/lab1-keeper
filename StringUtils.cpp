#include "StringUtils.h"
#include <cstring>

char* copyString(const char* source) {
    if (!source) source = "";
    size_t n = std::strlen(source) + 1;
    char* result = new char[n];
    std::memcpy(result, source, n);
    return result;
}
