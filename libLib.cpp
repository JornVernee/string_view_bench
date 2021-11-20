#include "lib.h"

EXPORT void empty() {}
EXPORT size_t by_view(std::string_view sv) { return sv.size(); }
EXPORT size_t by_split(const char* str, size_t length) { return length; }
EXPORT size_t by_split_length_first(size_t length, const char* str) { return length; }
