#include <iostream>

#include "lib\fileParser.cpp"
#include "lib\DB.cpp"

int main(int argc, char** argv) {
  std::filesystem::path path = argv[1];

  std::string res = parseFile(path);

  MyCoolDB db;
  std::cout << db.RequestDB(res);
}
