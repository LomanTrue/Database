#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <variant>
#include <filesystem>

class Column;
class Table;
class MyCoolDB;

class Request {
 private:
  std::vector<std::string*> cols; //if * - nullptr;
  std::vector<Column*> table1_cols;
  Table* table1;
  Table* table2;
  bool is_sel;
  bool is_del;
  bool is_up;
  char join;
  std::string* j_col1;
  std::string* j_col2;
  bool is_where;
  std::vector<std::pair<bool, std::vector<std::string*>>> cond;
  std::vector<char> cond_op; //AND - 1, OR - 2
  std::vector<std::pair<Column*, std::string*>> set;

  void clear();
  int UpdateRec(std::vector<int>& data);
  int DeleteRec(std::vector<int>& data);
  void PrintRec(std::vector<int>& data, std::string& res);
  void PrintRec(std::vector<std::pair<int,int>>& data, std::string& res);
  int Cond(bool is_true, Column* col, std::string* str1, std::string* str2, std::vector<int>& data);
  int Cond(bool is_true, Column* col, char table_num, std::string* str1, std::string* str2, std::vector<std::pair<int,int>>& data);
  int Where(std::vector<std::vector<int>>& data, std::string& res);
  int Where(std::vector<std::vector<std::pair<int,int>>>& data);

 public:
  Request() {
    table1 = nullptr;
    table2 = nullptr;
    is_sel = false;
    is_del = false;
    is_up = false;
    join = 0;
    j_col1 = nullptr;
    j_col2 = nullptr;
    is_where = false;
  }

  int DoReq(std::string& res);

  friend class Table;
  friend class MyCoolDB;
};

class Column {
 private:
  char type;
  std::string name;
  std::vector<std::variant<int,float,bool,double,std::string>> data;

 public:
  void WriteColFile(std::ofstream& file);
  int ReadColumnFile(std::string& str);

  friend class Request;
  friend class Table;
  friend class MyCoolDB;
};

class Table {
 private:
  std::string name;
  uint32_t p_key;
  std::vector<Column> cols;

 public:
  Table() {
    name = "";
    p_key = 0;
  }

  Table(std::string name_) {
    name = name_;
    p_key = 0;
  }

  int CreateTable(std::vector<std::string>& data, int* i);
  void WriteTableFile(std::ofstream& file);
  int ReadTableFile(std::ifstream& file);

  friend class MyCoolDB;
  friend class Request;
};

class MyCoolDB {
 private:
  std::vector<Table> tables;

  int WriteFile(std::filesystem::path file_name);
  int ReadFile(std::filesystem::path file_name);
  int Parse(std::vector<std::string>& data, int* i, std::string& res);

 public:
  std::string RequestDB(std::string& req_str);
};
