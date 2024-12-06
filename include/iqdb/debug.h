#ifndef IQDB_DEBUG_H
#define IQDB_DEBUG_H

#include <iostream>
#include <print>

namespace iqdb {

// The logging verbosity level. 0 = DEBUG, 1 = INFO, 2 = WARN, 3 = ERROR.
extern int debug_level;

template<typename... Args>
inline void LOG(std::string prefix, std::format_string<Args...> format, int level, Args... args) {
  if (level >= debug_level) {
    std::cerr << prefix;
    std::println(std::cerr, format, std::forward<Args>(args)...);
  }
}

template<typename... Args>
inline void DEBUG(std::format_string<Args...> format, Args... args) {
  LOG("[debug] ", format, 0, args...);
}

template<typename... Args>
inline void INFO(std::format_string<Args...> format, Args... args) {
  LOG("[info] ", format, 1, args...);
}

template<typename... Args>
inline void WARN(std::format_string<Args...>format , Args... args) {
  LOG("[warn] ", format, 2, args...);
}

template<typename... Args>
inline void ERROR(std::format_string<Args...> format, Args... args) {
  LOG("[error] ", format, 3, args...);
}

}

#endif // IQDB_DEBUG_H
