#pragma once

#include <cstdlib>
#include <fstream>
#include <string>

class dotenv {
private:
  static std::string trim(std::string s) {
    const char *whitespace = " \t\n\r\f\v";

    size_t first = s.find_first_not_of(whitespace);
    if (first == std::string::npos) return "";

    size_t last = s.find_last_not_of(whitespace);
    return s.substr(first, (last - first) + 1);
  }

public:
  static void init(const char *filename = ".env") {
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
      trim(line);
      if (line.empty() || line.front() == '#') continue;

      auto eq_pos = line.find('=');
      if (eq_pos == std::string::npos) continue;

      std::string key = trim(line.substr(0, eq_pos));
      std::string value = trim(line.substr(eq_pos + 1));

      if (value.size() >= 2) {
        char last = value.back();
        char first = value.front();

        if ((first == last) && (first == '"' || first == '\'')) {
          value = value.substr(1, value.size() - 2);
        }
      }

      if (!key.empty()) setenv(key.c_str(), value.c_str(), 1);
    }
  }
};
