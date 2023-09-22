#include <iostream>
#include <filesystem>
#include <fstream>

std::string parseFile(std::filesystem::path path) {
  std::string res;
  std::ifstream file(path, std::ios::binary);

  if (!file.is_open()) {
    throw std::exception();
  } else {
    char cur[1];
    file.read(cur, 1);
    while (!file.eof()) {
      res += cur[0];
      file.read(cur, 1);
    }
  }

  file.close();

  return res;
}
