#include <string_view>

#ifdef _WIN64
#define EXPORT __declspec(dllexport) 
#else
#define EXPORT
#endif

EXPORT void empty();
EXPORT size_t by_view(std::string_view sv);
EXPORT size_t by_split(const char* str, size_t length);
EXPORT size_t by_split_length_first(size_t length, const char* str);
