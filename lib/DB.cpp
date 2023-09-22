#include "DB.h"

void Request::clear() {
  cols.clear();
  table1_cols.clear();
  table1 = nullptr;
  table2 = nullptr;
  is_sel = false;
  is_del = false;
  is_up = false;
  join = 0;
  j_col1 = nullptr;
  j_col2 = nullptr;
  is_where = false;
  cond.clear();
  cond_op.clear();
  set.clear();
}

int Request::UpdateRec(std::vector<int>& data) {
  for (int i = 0; i < data.size(); i++) {
    for (int j = 0; j < set.size(); j++) {
      char temp_t = set[j].first->type;
      if (temp_t == 'i') {
        int temp = std::stoi(*(set[j].second));
        set[j].first->data[i] = temp;
      } else if (temp_t == 'f') {
        float temp = std::stof(*(set[j].second));
        set[j].first->data[i] = temp;
      } else if (temp_t == 'b') {
        bool temp;
        if (*(set[j].second) == "TRUE") {
          temp = true;
        } else {
          temp = false;
        }
        set[j].first->data[i] = temp;
      } else if (temp_t == 'd') {
        double temp = std::stod(*(set[j].second));
        set[j].first->data[i] = temp;
      } else if (temp_t == 's') {
        std::string temp = *(set[j].second);
        set[j].first->data[i] = temp;
      }
    }
  }

  return static_cast<int>(data.size());
}

int Request::DeleteRec(std::vector<int>& data) {
  for (int i = (int)data.size() - 1; i >= 0; i--) {
    for (int j = 0; j < table1->cols.size(); j++) {
      if (data[i] + 1 != table1->cols[j].data.size()) {
        for (int k = data[i] + 1; k < table1->cols[j].data.size(); k++) {
          table1->cols[j].data[k - 1] = table1->cols[j].data[k];
        }
      }
      table1->cols[j].data.pop_back();
    }
  }

  return static_cast<int>(data.size());
}

void Request::PrintRec(std::vector<int>& data, std::string& res) {
  if (table1_cols.empty()) {
    for (int i = 0; i < cols.size(); i++) {
      table1_cols.push_back(nullptr);
    }
    for (int i = 0; i < table1->cols.size(); i++) {
      for (int j = 0; j < cols.size(); j++) {
        if (table1->cols[i].name == *cols[j]) {
          table1_cols[j] = &(table1->cols[i]);
        }
      }
    }
  }

  for (int i = 0; i < data.size(); i++) {
    for (int j = 0; j < table1_cols.size(); j++) {
      if (table1_cols[j]->type == 'i') {
        res += std::to_string(std::get<int>(table1_cols[j]->data[data[i]]));
        res += " ";
      } else if (table1_cols[j]->type == 'f') {
        res += std::to_string(std::get<float>(table1_cols[j]->data[data[i]]));
        res += " ";
      } else if (table1_cols[j]->type == 'b') {
        if (std::get<bool>(table1_cols[j]->data[data[i]])) {
          res += "TRUE ";
        } else {
          res += "FALSE ";
        }
      } else if (table1_cols[j]->type == 'd') {
        res += std::to_string(std::get<double>(table1_cols[j]->data[data[i]]));
        res += " ";
      } else if (table1_cols[j]->type == 's') {
        res += std::get<std::string>(table1_cols[j]->data[data[i]]);
        res += " ";
      }
    }
    res += "\n";
  }

  return;
}

void Request::PrintRec(std::vector<std::pair<int,int>>& data, std::string& res) {
  std::vector<char> table_nums;
  bool all_cols = true;
  if (table1_cols.empty()) {
    all_cols = false;
    std::string table_name;
    std::string col_name;
    bool flag = true;
    for (int i = 0; i < cols.size(); i++) {
      for (int j = 0; j < cols[i]->size(); j++) {
        if ((*cols[i])[j] == '.') {
          flag = false;
          continue;
        }
        if (flag) {
          table_name += (*cols[i])[j];
        } else {
          col_name += (*cols[i])[j];
        }
      }
      if (table_name == table1->name) {
        table_nums.push_back(1);
        for (int j = 0; j < table1->cols.size(); j++) {
          if (col_name == table1->cols[j].name) {
            table1_cols.push_back(&(table1->cols[j]));
            break;
          }
        }
      } else {
        table_nums.push_back(2);
        for (int j = 0; j < table2->cols.size(); j++) {
          if (col_name == table2->cols[j].name) {
            table1_cols.push_back(&(table2->cols[j]));
            break;
          }
        }
      }
      flag = true;
      table_name.clear();
      col_name.clear();
    }
  }

  for (int i = 0; i < data.size(); i++) {
    if (all_cols) {
      for (int j = 0; j < table1->cols.size(); j++) {
        if (data[i].first == -1) {
          res += "NULL ";
          continue;
        }
        if (table1->cols[j].type == 'i') {
          res += std::to_string(std::get<int>(table1->cols[j].data[data[i].first]));
          res += " ";
        } else if (table1->cols[j].type == 'f') {
          res += std::to_string(std::get<float>(table1->cols[j].data[data[i].first]));
          res += " ";
        } else if (table1->cols[j].type == 'b') {
          if (std::get<bool>(table1->cols[j].data[data[i].first])) {
            res += "TRUE ";
          } else {
            res += "FALSE ";
          }
        } else if (table1->cols[j].type == 'd') {
          res += std::to_string(std::get<double>(table1->cols[j].data[data[i].first]));
          res += " ";
        } else if (table1->cols[j].type == 's') {
          res += std::get<std::string>(table1->cols[j].data[data[i].first]);
          res += " ";
        }
      }
      for (int j = 0; j < table2->cols.size(); j++) {
        if (data[i].second == -1) {
          res += "NULL ";
          continue;
        }
        if (table2->cols[j].type == 'i') {
          res += std::to_string(std::get<int>(table2->cols[j].data[data[i].second]));
          res += " ";
        } else if (table2->cols[j].type == 'f') {
          res += std::to_string(std::get<float>(table2->cols[j].data[data[i].second]));
          res += " ";
        } else if (table2->cols[j].type == 'b') {
          if (std::get<bool>(table2->cols[j].data[data[i].second])) {
            res += "TRUE ";
          } else {
            res += "FALSE ";
          }
        } else if (table2->cols[j].type == 'd') {
          res += std::to_string(std::get<double>(table2->cols[j].data[data[i].second]));
          res += " ";
        } else if (table2->cols[j].type == 's') {
          res += std::get<std::string>(table2->cols[j].data[data[i].second]);
          res += " ";
        }
      }
    } else {
      for (int j = 0; j < table1_cols.size(); j++) {
        if (table_nums[j] == 1) {
          if (data[i].first == -1) {
            res += "NULL ";
            continue;
          }
          if (table1_cols[j]->type == 'i') {
            res += std::to_string(std::get<int>(table1_cols[j]->data[data[i].first]));
            res += " ";
          } else if (table1_cols[j]->type == 'f') {
            res += std::to_string(std::get<float>(table1_cols[j]->data[data[i].first]));
            res += " ";
          } else if (table1_cols[j]->type == 'b') {
            if (std::get<bool>(table1_cols[j]->data[data[i].first])) {
              res += "TRUE ";
            } else {
              res += "FALSE ";
            }
          } else if (table1_cols[j]->type == 'd') {
            res += std::to_string(std::get<double>(table1_cols[j]->data[data[i].first]));
            res += " ";
          } else if (table1_cols[j]->type == 's') {
            res += std::get<std::string>(table1_cols[j]->data[data[i].first]);
            res += " ";
          }
        } else {
          if (data[i].second == -1) {
            res += "NULL ";
            continue;
          }
          if (table1_cols[j]->type == 'i') {
            res += std::to_string(std::get<int>(table1_cols[j]->data[data[i].second]));
            res += " ";
          } else if (table1_cols[j]->type == 'f') {
            res += std::to_string(std::get<float>(table1_cols[j]->data[data[i].second]));
            res += " ";
          } else if (table1_cols[j]->type == 'b') {
            if (std::get<bool>(table1_cols[j]->data[data[i].second])) {
              res += "TRUE ";
            } else {
              res += "FALSE ";
            }
          } else if (table1_cols[j]->type == 'd') {
            res += std::to_string(std::get<double>(table1_cols[j]->data[data[i].second]));
            res += " ";
          } else if (table1_cols[j]->type == 's') {
            res += std::get<std::string>(table1_cols[j]->data[data[i].second]);
            res += " ";
          }
        }
      }
    }
    res += "\n";
  }

  return;
}

int Request::Cond(bool is_true, Column* col, std::string* str1, std::string* str2, std::vector<int>& data) {
  std::vector<int> temp;
  bool res = true;
  for (int i = 0; i < data.size(); i++) {
    if (str1 == nullptr) {
      if (col->type == 'i') {
        if (std::get<int>(col->data[data[i]]) == NULL) {
          res = true;
        } else {
          res = false;
        }
      } else if (col->type == 'f') {
        if (std::get<float>(col->data[data[i]]) == NULL) {
          res = true;
        } else {
          res = false;
        }
      } else if (col->type == 'b') {
        if (std::get<bool>(col->data[data[i]]) == NULL) {
          res = true;
        } else {
          res = false;
        }
      } else if (col->type == 'd') {
        if (std::get<double>(col->data[data[i]]) == NULL) {
          res = true;
        } else {
          res = false;
        }
      } else if (col->type == 's') {
        if (std::get<std::string>(col->data[data[i]]).empty()) {
          res = true;
        } else {
          res = false;
        }
      }
    } else {
      int temp_i;
      float temp_f;
      bool temp_b;
      double temp_d;
      std::string temp_s;

      if (col->type == 'i') {
        temp_i = std::get<int>(col->data[data[i]]);
      } else if (col->type == 'f') {
        temp_f = std::get<float>(col->data[data[i]]);
      } else if (col->type == 'b') {
        temp_b = std::get<bool>(col->data[data[i]]);
      } else if (col->type == 'd') {
        temp_d = std::get<double>(col->data[data[i]]);
      } else if (col->type == 's') {
        temp_s = std::get<std::string>(col->data[data[i]]);
      }

      if (col->type == 'i') {
        if (*str1 == "=") {
          if (temp_i == std::stoi(*str2)) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == "<") {
          if (temp_i < std::stoi(*str2)) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == ">") {
          if (temp_i > std::stoi(*str2)) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == "<=") {
          if (temp_i <= std::stoi(*str2)) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == ">=") {
          if (temp_i >= std::stoi(*str2)) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == "!=" || *str1 == "<>") {
          if (temp_i != std::stoi(*str2)) {
            res = true;
          } else {
            res = false;
          }
        }
      } else if (col->type == 'f') {
        if (*str1 == "=") {
          if (temp_f == std::stof(*str2)) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == "<") {
          if (temp_f < std::stof(*str2)) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == ">") {
          if (temp_f > std::stof(*str2)) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == "<=") {
          if (temp_f <= std::stof(*str2)) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == ">=") {
          if (temp_f >= std::stof(*str2)) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == "!=" || *str1 == "<>") {
          if (temp_f != std::stof(*str2)) {
            res = true;
          } else {
            res = false;
          }
        }
      } else if (col->type == 'b') {
        if (*str1 == "=") {
          if (*str2 == "TRUE") {
            if (temp_b) {
              res = true;
            } else {
              res = false;
            }
          } else {
            if (temp_b) {
              res = false;
            } else {
              res = true;
            }
          }
        } else if (*str1 == "!=" || *str1 == "<>") {
          if (*str2 == "TRUE") {
            if (temp_b) {
              res = false;
            } else {
              res = true;
            }
          } else {
            if (temp_b) {
              res = true;
            } else {
              res = false;
            }
          }
        }
      } else if (col->type == 'd') {
        if (*str1 == "=") {
          if (temp_d == std::stod(*str2)) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == "<") {
          if (temp_d < std::stod(*str2)) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == ">") {
          if (temp_d > std::stod(*str2)) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == "<=") {
          if (temp_d <= std::stod(*str2)) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == ">=") {
          if (temp_d >= std::stod(*str2)) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == "!=" || *str1 == "<>") {
          if (temp_d != std::stod(*str2)) {
            res = true;
          } else {
            res = false;
          }
        }
      } else if (col->type == 's') {
        if (*str1 == "=") {
          if (temp_s == *str2) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == "<") {
          if (temp_s < *str2) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == ">") {
          if (temp_s > *str2) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == "<=") {
          if (temp_s <= *str2) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == ">=") {
          if (temp_s >= *str2) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == "!=" || *str1 == "<>") {
          if (temp_s != *str2) {
            res = true;
          } else {
            res = false;
          }
        }
      }
    }

    if (is_true) {
      if (res) {
        temp.push_back(data[i]);
      }
    } else if (!res) {
      temp.push_back(data[i]);
    }
  }

  data.clear();
  for (int i = 0; i < temp.size(); i++) {
    data.push_back(temp[i]);
  }

  return 0;
}

int Request::Cond(bool is_true, Column* col, char table_num, std::string* str1, std::string* str2, std::vector<std::pair<int,int>>& data) {
  std::vector<std::pair<int,int>> temp;
  bool res = true;
  for (int i = 0; i < data.size(); i++) {
    if (str1 == nullptr) {
      if (table_num == 1) {
        if (data[i].first == -1) {
          res = true;
        } else {
          res = false;
        }
      } else {
        if (data[i].second == -1) {
          res = true;
        } else {
          res = false;
        }
      }
    } else {
      int temp_i;
      float temp_f;
      bool temp_b;
      double temp_d;
      std::string temp_s;

      int rec = 0;
      if (table_num == 1) {
        rec = data[i].first;
      } else {
        rec = data[i].second;
      }
      if (col->type == 'i') {
        temp_i = std::get<int>(col->data[rec]);
      } else if (col->type == 'f') {
        temp_f = std::get<float>(col->data[rec]);
      } else if (col->type == 'b') {
        temp_b = std::get<bool>(col->data[rec]);
      } else if (col->type == 'd') {
        temp_d = std::get<double>(col->data[rec]);
      } else if (col->type == 's') {
        temp_s = std::get<std::string>(col->data[rec]);
      }

      if (col->type == 'i') {
        if (*str1 == "=") {
          if (temp_i == std::stoi(*str2)) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == "<") {
          if (temp_i < std::stoi(*str2)) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == ">") {
          if (temp_i > std::stoi(*str2)) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == "<=") {
          if (temp_i <= std::stoi(*str2)) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == ">=") {
          if (temp_i >= std::stoi(*str2)) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == "!=" || *str1 == "<>") {
          if (temp_i != std::stoi(*str2)) {
            res = true;
          } else {
            res = false;
          }
        }
      } else if (col->type == 'f') {
        if (*str1 == "=") {
          if (temp_f == std::stof(*str2)) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == "<") {
          if (temp_f < std::stof(*str2)) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == ">") {
          if (temp_f > std::stof(*str2)) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == "<=") {
          if (temp_f <= std::stof(*str2)) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == ">=") {
          if (temp_f >= std::stof(*str2)) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == "!=" || *str1 == "<>") {
          if (temp_f != std::stof(*str2)) {
            res = true;
          } else {
            res = false;
          }
        }
      } else if (col->type == 'b') {
        if (*str1 == "=") {
          if (*str2 == "TRUE") {
            if (temp_b) {
              res = true;
            } else {
              res = false;
            }
          } else {
            if (temp_b) {
              res = false;
            } else {
              res = true;
            }
          }
        } else if (*str1 == "!=" || *str1 == "<>") {
          if (*str2 == "TRUE") {
            if (temp_b) {
              res = false;
            } else {
              res = true;
            }
          } else {
            if (temp_b) {
              res = true;
            } else {
              res = false;
            }
          }
        }
      } else if (col->type == 'd') {
        if (*str1 == "=") {
          if (temp_d == std::stod(*str2)) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == "<") {
          if (temp_d < std::stod(*str2)) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == ">") {
          if (temp_d > std::stod(*str2)) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == "<=") {
          if (temp_d <= std::stod(*str2)) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == ">=") {
          if (temp_d >= std::stod(*str2)) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == "!=" || *str1 == "<>") {
          if (temp_d != std::stod(*str2)) {
            res = true;
          } else {
            res = false;
          }
        }
      } else if (col->type == 's') {
        if (*str1 == "=") {
          if (temp_s == *str2) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == "<") {
          if (temp_s < *str2) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == ">") {
          if (temp_s > *str2) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == "<=") {
          if (temp_s <= *str2) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == ">=") {
          if (temp_s >= *str2) {
            res = true;
          } else {
            res = false;
          }
        } else if (*str1 == "!=" || *str1 == "<>") {
          if (temp_s != *str2) {
            res = true;
          } else {
            res = false;
          }
        }
      }
    }

    if (is_true) {
      if (res) {
        temp.push_back(data[i]);
      }
    } else if (!res) {
      temp.push_back(data[i]);
    }
  }

  data.clear();
  for (int i = 0; i < temp.size(); i++) {
    data.push_back(temp[i]);
  }

  return 0;
}

int Request::Where(std::vector<std::vector<int>>& data, std::string& res) {
  Column* col;
  for (int i = 0; i < cond.size(); i++) {
    bool flag = true;
    for (int j = 0; j < table1->cols.size(); j++) {
      if (table1->cols[j].name == *(cond[i].second[0])) {
        flag = false;
        col = &(table1->cols[j]);
        break;
      }
    }
    if (flag) {
      res += "Wrong Condition";
      return 1;
    }
    if (cond[i].second.size() == 2) {
      Cond(cond[i].first, col, nullptr, cond[i].second[1], data[i]);
    } else {
      Cond(cond[i].first, col, cond[i].second[1], cond[i].second[2], data[i]);
    }
  }
  if (cond.size() != 1) {
    for (int i = 0; i < cond_op.size(); i++) {
      if (cond_op[i] == 1) {
        std::vector<int> temp;
        int cur_i1 = 0;
        int cur_i2 = 0;
        while (cur_i1 != data[i].size() && cur_i2 != data[i + 1].size()) {
          if (data[i][cur_i1] == data[i + 1][cur_i2]) {
            temp.push_back(data[i][cur_i1]);
            cur_i1++;
            cur_i2++;
            continue;
          }
          if (data[i][cur_i1] < data[i + 1][cur_i2]) {
            cur_i1++;
          } else {
            cur_i2++;
          }
        }
        data[i].clear();
        for (int j = 0; j < temp.size(); j++) {
          data[i].push_back(temp[j]);
        }
        for (int j = i + 2; j < data.size(); j++) {
          data[j - 1].clear();
          for (int k = 0; k < data[j].size(); k++) {
            data[j - 1].push_back(data[j][k]);
          }
        }
        data.pop_back();
        for (int j = i + 1; j < cond_op.size(); j++) {
          cond_op[j - 1] = cond_op[j];
        }
        cond_op.pop_back();
        i -= 1;
      }
    }
    for (int i = 0; i < cond_op.size(); i++) {
      std::vector<int> temp;
      int cur_i1 = 0;
      int cur_i2 = 0;
      while (cur_i1 != data[i].size() && cur_i2 != data[i + 1].size()) {
        if (data[i][cur_i1] == data[i + 1][cur_i2]) {
          temp.push_back(data[i][cur_i1]);
          cur_i1++;
          cur_i2++;
          continue;
        }
        if (data[i][cur_i1] < data[i + 1][cur_i2]) {
          temp.push_back(data[i][cur_i1]);
          cur_i1++;
        } else {
          temp.push_back(data[i + 1][cur_i2]);
          cur_i2++;
        }
      }
      if (cur_i1 == data[i].size() && cur_i2 != data[i + 1].size()) {
        for (int j = cur_i2; j < data[i + 1].size(); j++) {
          temp.push_back(data[i + 1][j]);
        }
      } else if (cur_i1 != data[i].size() && cur_i2 == data[i + 1].size()) {
        for (int j = cur_i1; j < data[i].size(); j++) {
          temp.push_back(data[i][j]);
        }
      }
      data[i].clear();
      for (int j = 0; j < temp.size(); j++) {
        data[i].push_back(temp[j]);
      }
      for (int j = i + 2; j < data.size(); j++) {
        data[j - 1].clear();
        for (int k = 0; k < data[j].size(); k++) {
          data[j - 1].push_back(data[j][k]);
        }
      }
      data.pop_back();
      for (int j = i + 1; j < cond_op.size(); j++) {
        cond_op[j - 1] = cond_op[j];
      }
      cond_op.pop_back();
      i -= 1;
    }
  }

  return 0;
}

int Request::Where(std::vector<std::vector<std::pair<int,int>>>& data) {
  std::vector<char> table_nums;
  std::string table_name;
  std::string col_name;
  bool flag = true;
  for (int i = 0; i < cond.size(); i++) {
    for (int j = 0; j < cond[i].second[0]->size(); j++) {
      if ((*cond[i].second[0])[j] == '.') {
        flag = false;
        continue;
      }
      if (flag) {
        table_name += (*cond[i].second[0])[j];
      } else {
        col_name += (*cond[i].second[0])[j];
      }
    }
    (*cond[i].second[0]) = col_name;
    Column* col;
    if (table1->name == table_name) {
      table_nums.push_back(1);
      for (int j = 0; j < table1->cols.size(); j++) {
        if (table1->cols[j].name == col_name) {
          col = &(table1->cols[j]);
          break;
        }
      }
      if (cond[i].second.size() == 2) {
        Cond(cond[i].first, col, 1, nullptr, cond[i].second[1], data[i]);
      } else {
        Cond(cond[i].first, col, 1, cond[i].second[1], cond[i].second[2], data[i]);
      }
    } else {
      table_nums.push_back(2);
      for (int j = 0; j < table2->cols.size(); j++) {
        if (table2->cols[j].name == col_name) {
          col = &(table2->cols[j]);
          break;
        }
      }
      if (cond[i].second.size() == 2) {
        Cond(cond[i].first, col, 2, nullptr, cond[i].second[1], data[i]);
      } else {
        Cond(cond[i].first, col, 2, cond[i].second[1], cond[i].second[2], data[i]);
      }
    }
    table_name.clear();
    col_name.clear();
    flag = true;
  }

  if (cond.size() != 1) {
    for (int i = 0; i < cond_op.size(); i++) {
      if (cond_op[i] == 1) {
        int cur_i1 = 0;
        int cur_i2 = 0;
        std::vector<std::pair<int,int>> temp;
        while (cur_i1 != data[i].size() && cur_i2 != data[i + 1].size()) {
          if (data[i][cur_i1].first == data[i + 1][cur_i2].first && data[i][cur_i1].second == data[i + 1][cur_i2].second) {
            temp.push_back(data[i][cur_i1]);
            cur_i1++;
            cur_i2++;
            continue;
          }
          if (join == 2) {
            if (data[i][cur_i1].second < data[i + 1][cur_i2].second) {
              cur_i1++;
            } else if (data[i][cur_i1].second > data[i + 1][cur_i2].second) {
              cur_i2++;
            } else if (data[i][cur_i1].first < data[i + 1][cur_i2].first) {
              cur_i1++;
            } else {
              cur_i2++;
            }
          } else {
            if (data[i][cur_i1].first < data[i + 1][cur_i2].first) {
              cur_i1++;
            } else if (data[i][cur_i1].first > data[i + 1][cur_i2].first) {
              cur_i2++;
            } else if (data[i][cur_i1].second < data[i + 1][cur_i2].second) {
              cur_i1++;
            } else {
              cur_i2++;
            }
          }
        }

        data[i].clear();
        for (int j = 0; j < temp.size(); j++) {
          data[i].push_back(temp[j]);
        }
        for (int j = i + 2; j < data.size(); j++) {
          data[j - 1].clear();
          for (int k = 0; k < data[j].size(); k++) {
            data[j - 1].push_back(data[j][k]);
          }
        }
        data.pop_back();
        for (int j = i + 1; j < cond_op.size(); j++) {
          cond_op[j - 1] = cond_op[j];
        }
        cond_op.pop_back();
        i -= 1;
      }
    }

    for (int i = 0; i < cond_op.size(); i++) {
      if (cond_op[i] == 2) {
        int cur_i1 = 0;
        int cur_i2 = 0;
        std::vector<std::pair<int,int>> temp;
        while (cur_i1 != data[i].size() && cur_i2 != data[i + 1].size()) {
          if (data[i][cur_i1].first == data[i + 1][cur_i2].first && data[i][cur_i1].second == data[i + 1][cur_i2].second) {
            temp.push_back(data[i][cur_i1]);
            cur_i1++;
            cur_i2++;
            continue;
          }
          if (join == 2) {
            if (data[i][cur_i1].second < data[i + 1][cur_i2].second) {
              temp.push_back(data[i][cur_i1]);
              cur_i1++;
            } else if (data[i][cur_i1].second > data[i + 1][cur_i2].second) {
              temp.push_back(data[i + 1][cur_i2]);
              cur_i2++;
            } else if (data[i][cur_i1].first < data[i + 1][cur_i2].first) {
              temp.push_back(data[i][cur_i1]);
              cur_i1++;
            } else {
              temp.push_back(data[i + 1][cur_i2]);
              cur_i2++;
            }
          } else {
            if (data[i][cur_i1].first < data[i + 1][cur_i2].first) {
              temp.push_back(data[i][cur_i1]);
              cur_i1++;
            } else if (data[i][cur_i1].first > data[i + 1][cur_i2].first) {
              temp.push_back(data[i + 1][cur_i2]);
              cur_i2++;
            } else if (data[i][cur_i1].second < data[i + 1][cur_i2].second) {
              temp.push_back(data[i][cur_i1]);
              cur_i1++;
            } else {
              temp.push_back(data[i + 1][cur_i2]);
              cur_i2++;
            }
          }
        }

        if (cur_i1 < data[i].size()) {
          for (int j = cur_i1; j < data[i].size(); j++) {
            temp.push_back(data[i][j]);
          }
        } else if (cur_i2 < data[i + 1].size()) {
          for (int j = cur_i2; j < data[i + 1].size(); j++) {
            temp.push_back(data[i + 1][j]);
          }
        }

        data[i].clear();
        for (int j = 0; j < temp.size(); j++) {
          data[i].push_back(temp[j]);
        }
        for (int j = i + 2; j < data.size(); j++) {
          data[j - 1].clear();
          for (int k = 0; k < data[j].size(); k++) {
            data[j - 1].push_back(data[j][k]);
          }
        }
        data.pop_back();
        for (int j = i + 1; j < cond_op.size(); j++) {
          cond_op[j - 1] = cond_op[j];
        }
        cond_op.pop_back();
        i -= 1;
      }
    }
  }

  return 1;
}

int Request::DoReq(std::string& res) {
  std::vector<std::string> col_names;
  if (cols[0] == nullptr) {
    if (table2 == nullptr) {
      cols.clear();
      for (int i = 0; i < table1->cols.size(); i++) {
        col_names.push_back(table1->cols[i].name);
        cols.push_back(&col_names[i]);
        table1_cols.push_back(&(table1->cols[i]));
      }
    } else {
      cols.clear();
      for (int i = 0; i < table1->cols.size(); i++) {
        table1_cols.push_back(&(table1->cols[i]));
      }
      for (int i = 0; i < table2->cols.size(); i++) {
        table1_cols.push_back(&(table2->cols[i]));
      }
    }
  }

  if (is_up) {
    if (is_where) {
      std::vector<std::vector<int>> data;
      std::vector<int> temp;
      for (int i = 0; i < table1->cols[0].data.size(); i++) {
        temp.push_back(i);
      }
      for (int i = 0; i < cond.size(); i++) {
        data.push_back(temp);
      }
      Where(data, res);
      temp.clear();
      for (int i = 0; i < data[0].size(); i++) {
        temp.push_back(data[0][i]);
      }
      res += "Updated ";
      res += std::to_string(UpdateRec(temp));
      res += " records";
    } else {
      std::vector<int> data;
      for (int i = 0; i < table1->cols[0].data.size(); i++) {
        data.push_back(i);
      }
      res += "Updated ";
      res += std::to_string(UpdateRec(data));
      res += " records";
    }
  } else if (is_del) {
    if (is_where) {
      std::vector<std::vector<int>> data;
      std::vector<int> temp;
      for (int i = 0; i < table1->cols[0].data.size(); i++) {
        temp.push_back(i);
      }
      for (int i = 0; i < cond.size(); i++) {
        data.push_back(temp);
      }
      Where(data, res);
      temp.clear();
      for (int i = 0; i < data[0].size(); i++) {
        temp.push_back(data[0][i]);
      }
      res += "Deleted ";
      res += std::to_string(DeleteRec(temp));
      res += " records";
    } else {
      std::vector<int> data;
      for (int i = 0; i < table1->cols[0].data.size(); i++) {
        data.push_back(i);
      }
      res += "Deleted ";
      res += std::to_string(DeleteRec(data));
      res += " records";
    }
  } else if (is_sel) {
    if (join != 0) {
      std::vector<std::pair<int,int>> data;
      std::string table_name;
      std::string col_name;
      Column* col1;
      Column* col2;
      bool flag = true;
      for (int i = 0; i < j_col1->size(); i++) {
        if ((*j_col1)[i] == '.') {
          flag = false;
          continue;
        }
        if (flag) {
          table_name += (*j_col1)[i];
        } else {
          col_name += (*j_col1)[i];
        }
      }
      if (table1->name == table_name) {
        for (int i = 0; i < table1->cols.size(); i++) {
          if (table1->cols[i].name == col_name) {
            col1 = &(table1->cols[i]);
            break;
          }
        }
      } else {
        for (int i = 0; i < table2->cols.size(); i++) {
          if (table2->cols[i].name == col_name) {
            col2 = &(table2->cols[i]);
            break;
          }
        }
      }

      flag = true;
      table_name.clear();
      col_name.clear();
      for (int i = 0; i < j_col2->size(); i++) {
        if ((*j_col2)[i] == '.') {
          flag = false;
          continue;
        }
        if (flag) {
          table_name += (*j_col2)[i];
        } else {
          col_name += (*j_col2)[i];
        }
      }
      if (table1->name == table_name) {
        for (int i = 0; i < table1->cols.size(); i++) {
          if (table1->cols[i].name == col_name) {
            col1 = &(table1->cols[i]);
            break;
          }
        }
      } else {
        for (int i = 0; i < table2->cols.size(); i++) {
          if (table2->cols[i].name == col_name) {
            col2 = &(table2->cols[i]);
            break;
          }
        }
      }
      if (join == 1) {
        for (int i = 0; i < col1->data.size(); i++) {
          data.push_back(std::make_pair(i,-1));
          for (int j = 0; j < col2->data.size(); j++) {
            if (col1->data[i] == col2->data[j]) {
              if (data[(int)data.size() - 1].second == -1) {
                data.pop_back();
              }
              data.push_back(std::make_pair(i,j));
            }
          }
        }
      } else if (join == 2) {
        for (int i = 0; i < col2->data.size(); i++) {
          data.push_back(std::make_pair(-1,i));
          for (int j = 0; j < col1->data.size(); j++) {
            if (col2->data[i] == col1->data[j]) {
              if (data[(int)data.size() - 1].first == -1) {
                data.pop_back();
              }
              data.push_back(std::make_pair(j,i));
            }
          }
        }
      } else {
        for (int i = 0; i < col1->data.size(); i++) {
          for (int j = 0; j < col2->data.size(); j++) {
            if (col1->data[i] == col2->data[j]) {
              data.push_back(std::make_pair(i,j));
            }
          }
        }
      }
      if (is_where) {
        std::vector<std::vector<std::pair<int,int>>> temp;
        for (int i = 0; i < cond.size(); i++) {
          temp.push_back(data);
        }
        Where(temp);
        data.clear();
        for (int i = 0; i < temp[0].size(); i++) {
          data.push_back(temp[0][i]);
        }
      }
      PrintRec(data, res);
    } else if (is_where) {
      std::vector<std::vector<int>> data;
      std::vector<int> temp;
      for (int i = 0; i < table1->cols[0].data.size(); i++) {
        temp.push_back(i);
      }
      for (int i = 0; i < cond.size(); i++) {
        data.push_back(temp);
      }
      Where(data, res);
      temp.clear();
      for (int i = 0; i < data[0].size(); i++) {
        temp.push_back(data[0][i]);
      }
      PrintRec(temp, res);
    } else {
      std::vector<int> data;
      for (int i = 0; i < table1->cols[0].data.size(); i++) {
        data.push_back(i);
      }
      PrintRec(data, res);
    }
  }

  clear();
  res += "\n";
  return 0;
}

void Column::WriteColFile(std::ofstream& file) {
  file << type << ' ' << name << ' ';
  for (int i = 0; i < data.size(); i++) {
    if (type == 'i') {
      file << std::get<int>(data[i]) << ' ';
    } else if (type == 'f') {
      file << std::get<float>(data[i]) << ' ';
    } else if (type == 'b') {
      file << std::get<bool>(data[i]) << ' ';
    } else if (type == 'd') {
      file << std::get<double>(data[i]) << ' ';
    } else if (type == 's') {
      file << '\'' << std::get<std::string>(data[i]) << '\'' << ' ';
    }
  }
  file << '\n';
}

int Column::ReadColumnFile(std::string& str) {
  int cur_i = 0;
  type = str[cur_i];
  cur_i += 2;
  while (str[cur_i] != ' ') {
    name += str[cur_i];
    cur_i += 1;
  }
  cur_i += 1;
  std::string data_str;
  bool flag = true;
  for (int i = cur_i; i < str.size(); i++) {
    if (str[i] == '\'') {
      if (flag) {
        flag = false;
      } else {
        flag = true;
      }
    }
    if (str[i] == ' ' && flag) {
      if (type == 'i') {
        int temp = std::stoi(data_str);
        data.push_back(temp);
      } else if (type == 'f') {
        float temp = std::stof(data_str);
        data.push_back(temp);
      } else if (type == 'b') {
        bool temp;
        if (data_str == "1") {
          temp = true;
        } else {
          temp = false;
        }
        data.push_back(temp);
      } else if (type == 'd') {
        double temp = std::stod(data_str);
        data.push_back(temp);
      } else if (type == 's') {
        data_str = data_str.substr(1);
        data_str.pop_back();
        data.push_back(data_str);
      }
      data_str.clear();
    } else {
      data_str += str[i];
    }
  }

  return 0;
}

int Table::CreateTable(std::vector<std::string>& data, int* i) {
  bool flag = true;
  while (flag) {
    Column temp_col;
    temp_col.name = data[*i];
    if (temp_col.name[0] == '(') {
      temp_col.name = temp_col.name.substr(1);
    }
    *i += 1;

    if (data[*i][0] == 'I') {
      temp_col.type = 'i';
    } else if (data[*i][0] == 'F') {
      temp_col.type = 'f';
    } else if (data[*i][0] == 'B') {
      temp_col.type = 'b';
    } else if (data[*i][0] == 'D') {
      temp_col.type = 'd';
    } else if (data[*i][0] == 'V') {
      temp_col.type = 's';
    }

    if (data[*i][(int)data[*i].size() - 1] == ';') {
      flag = false;
    } else if (data[*i + 1] == "PRIMARY") {
      p_key = cols.size();
      *i += 2;
      if (data[*i][(int)data[*i].size() - 1] == ';') {
        flag = false;
      }
    }

    cols.push_back(temp_col);
    *i += 1;
  }
  return 0;
}

void Table::WriteTableFile(std::ofstream& file) {
  file << name << '\n';
  file << p_key << '\n';
  for (int i = 0; i < cols.size(); i++) {
    cols[i].WriteColFile(file);
  }
  file << '\n';
}

int Table::ReadTableFile(std::ifstream& file) {
  std::string str;
  if (!std::getline(file,str)) {
    return 1;
  };
  name = str;
  if (!std::getline(file,str)) {
    return 1;
  };
  p_key = std::stoi(str);
  std::getline(file,str);
  while (!str.empty()) {
    Column col;
    col.ReadColumnFile(str);
    cols.push_back(col);
    std::getline(file,str);
  }

  return 0;
}

int MyCoolDB::WriteFile(std::filesystem::path file_name) {
  std::ofstream file;
  file.open(file_name);
  file << "MyCoolDB" << '\n';
  file << '\n';

  for (int i = 0; i < tables.size(); i++) {
    tables[i].WriteTableFile(file);
  }
  file.close();

  return 0;
}

int MyCoolDB::ReadFile(std::filesystem::path file_name) {
  std::ifstream file;
  file.open(file_name);
  std::string str;
  if (std::getline(file, str)) {
    if (str != "MyCoolDB") {
      return 1;
    }
  } else {
    return 1;
  }
  if (!std::getline(file, str)) {
    return 0;
  }

  while (!file.eof()) {
    Table table;
    if (!table.ReadTableFile(file)) {
      tables.push_back(table);
    }
  }

  file.close();
  return 0;
}

int MyCoolDB::Parse(std::vector<std::string>& data, int* i, std::string& res) {
  Request req;
  std::pair<bool, std::vector<std::string*>> cond_p;
  cond_p.first = true;
  bool is_where = false;
  while (*i != data.size()) {
    if (data[*i] == "CREATE" && data[*i + 1] == "TABLE") {
      *i += 2;
      Table table(data[*i]);
      *i += 1;
      table.CreateTable(data, i);
      tables.push_back(table);
      res += "Table \"" + table.name + "\" created" + '\n';
    } else if (data[*i] == "DROP" && data[*i + 1] == "TABLE") {
      *i += 2;
      if (data[*i][(int)data[*i].size() - 1] == ';') {
        data[*i].pop_back();
      }
      bool flag = false;
      for (int j = 0; j < tables.size(); j++) {
        if (tables[j].name == data[*i]) {
          flag = true;
          for (int k = j + 1; k < tables.size(); k++) {
            tables[k - 1] = tables[k];
          }
          tables.pop_back();
          break;
        }
      }
      if (flag) {
        res += "Table \"";
        res += data[*i];
        res += "\" deleted\n";
      } else {
        res += "Table \"";
        res += data[*i];
        res += "\" not found\n";
      }
      *i += 1;
    } else if (data[*i] == "INSERT" && data[*i + 1] == "INTO") {
      *i += 2;
      Table* cur_table = nullptr;
      for (int j = 0; j < tables.size(); j++) {
        if (tables[j].name == data[*i]) {
          cur_table = &tables[j];
          break;
        }
      }
      if (cur_table == nullptr) {
        res += "Table ";
        res += data[*i];
        res += " not found\n";
        return 1;
      }
      *i += 1;
      std::vector<int> num_i;

      bool flag = true;
      while (flag) {
        if (data[*i][0] == '(') {
          data[*i] = data[*i].substr(1);
        }
        if (data[*i][(int)data[*i].size() - 1] == ')') {
          data[*i].pop_back();
          flag = false;
        }
        if (data[*i][(int)data[*i].size() - 1] == ',') {
          data[*i].pop_back();
        }
        for (int j = 0; j < cur_table->cols.size(); j++) {
          if (cur_table->cols[j].name == data[*i]) {
            num_i.push_back(j);
            break;
          }
        }

        *i += 1;
      }

      *i += 1;

      int num_i_cur = 0;
      flag = true;
      while (flag) {
        if (data[*i][0] == '(') {
          data[*i] = data[*i].substr(1);
        }
        if (data[*i][(int)data[*i].size() - 1] == ';') {
          data[*i].pop_back();
          flag = false;
        }
        while (data[*i][(int)data[*i].size() - 1] == ')' || data[*i][(int)data[*i].size() - 1] == ',') {
          data[*i].pop_back();
        }
        if (cur_table->cols[num_i[num_i_cur]].type == 'i') {
          int temp = std::stoi(data[*i]);
          cur_table->cols[num_i[num_i_cur]].data.push_back(temp);
          num_i_cur++;
        } else if (cur_table->cols[num_i[num_i_cur]].type == 'f') {
          float temp = std::stof(data[*i]);
          cur_table->cols[num_i[num_i_cur]].data.push_back(temp);
          num_i_cur++;
        } else if (cur_table->cols[num_i[num_i_cur]].type == 'b') {
          bool temp;
          if (data[*i] == "TRUE") {
            temp = true;
          } else if (data[*i] == "FALSE") {
            temp = false;
          } else {
            res += "Wrong data\n";
            return 1;
          }
          cur_table->cols[num_i[num_i_cur]].data.push_back(temp);
          num_i_cur++;
        } else if (cur_table->cols[num_i[num_i_cur]].type == 'd') {
          double temp = std::stod(data[*i]);
          cur_table->cols[num_i[num_i_cur]].data.push_back(temp);
          num_i_cur++;
        } else if (cur_table->cols[num_i[num_i_cur]].type == 's') {
          std::string temp = data[*i].substr(1);
          temp.pop_back();
          cur_table->cols[num_i[num_i_cur]].data.push_back(temp);
          num_i_cur++;
        }
        if (num_i_cur == num_i.size()) {
          num_i_cur = 0;
        }
        *i += 1;
      }

      res += "Data inserted\n";
    } else if (data[*i] == "WRITE") {
      *i += 1;
      data[*i] = data[*i].substr(1);
      data[*i].pop_back();
      data[*i].pop_back();
      std::filesystem::path path = data[*i];
      if (WriteFile(path)) {
        return 1;
      } else {
      res += "Database wrote into a file \"";
      res += data[*i];
      res += "\"\n";
      }
      *i += 1;
    } else if (data[*i] == "READ") {
      *i += 1;
      data[*i] = data[*i].substr(1);
      data[*i].pop_back();
      data[*i].pop_back();
      std::filesystem::path path = data[*i];
      if (ReadFile(path)) {
        return 1;
      } else {
        res += "Database read from file \"";
        res += data[*i];
        res += "\"\n";
      }
      *i += 1;
    } else if (data[*i] == "SELECT") {
      if (is_where) {
        req.cond.push_back(cond_p);
        cond_p.first = true;
        cond_p.second.clear();
        is_where = false;
      }
      req.is_sel = true;
      *i += 1;
      if (data[*i] == "*") {
        req.cols.push_back(nullptr);
        *i += 1;
      } else {
        bool flag = true;
        while (flag) {
          if (data[*i][(int)data[*i].size() - 1] == ',') {
            data[*i].pop_back();
          } else {
            flag = false;
          }
          req.cols.push_back(&data[*i]);
          *i += 1;
        }
      }
    } else if (data[*i] == "FROM") {
      if (is_where) {
        req.cond.push_back(cond_p);
        cond_p.first = true;
        cond_p.second.clear();
        is_where = false;
      }
      *i += 1;
      bool flag = false;
      if (data[*i][(int)data[*i].size() - 1] == ';') {
        flag = true;
        data[*i].pop_back();
      }
      bool find = true;
      for (int j = 0; j < tables.size(); j++) {
        if (tables[j].name == data[*i]) {
          req.table1 = &tables[j];
          find = false;
          break;
        }
      }
      if (find) {
        res += "Table \"";
        res += data[*i];
        res += "\" not found\n";
        return 1;
      }
      if (flag) {
        req.DoReq(res);
      }
      *i += 1;
    } else if (data[*i] == "DELETE") {
      if (is_where) {
        req.cond.push_back(cond_p);
        cond_p.first = true;
        cond_p.second.clear();
        is_where = false;
      }
      *i += 1;
      req.is_del = true;
    } else if (data[*i] == "UPDATE") {
      if (is_where) {
        req.cond.push_back(cond_p);
        cond_p.first = true;
        cond_p.second.clear();
        is_where = false;
      }
      *i += 1;
      req.is_up = true;
      bool flag = true;
      for (int j = 0; j < tables.size(); j++) {
        if (tables[j].name == data[*i]) {
          flag = false;
          req.table1 = &tables[j];
          break;
        }
      }
      if (flag) {
        res += "Table \"";
        res += data[*i];
        res += "\" not found\n";
        return 1;
      }
      *i += 2;
      flag = true;
      std::pair<Column*, std::string*> temp_p;
      bool is_end = false;
      while (flag) {
        bool find = true;
        for (int j = 0; j < req.table1->cols.size(); j++) {
          if (req.table1->cols[j].name == data[*i]) {
            temp_p.first = &(req.table1->cols[j]);
            find = false;
            break;
          }
        }
        if (find) {
          res += "Column \"";
          res += data[*i];
          res += "\" not found\n";
          return 1;
        }
        *i += 2;
        if (data[*i][(int)data[*i].size() - 1] == ',') {
          flag = false;
          data[*i].pop_back();
        }
        if (data[*i][(int)data[*i].size() - 1] == ';') {
          flag = false;
          is_end = true;
          data[*i].pop_back();
        }
        temp_p.second = &data[*i];
        req.set.push_back(temp_p);
        *i += 1;
      }
      if (is_end) {
        req.DoReq(res);
      }
    } else if (data[*i] == "LEFT") {
      if (is_where) {
        req.cond.push_back(cond_p);
        cond_p.first = true;
        cond_p.second.clear();
        is_where = false;
      }
      req.join = 1;
      *i += 1;
    } else if (data[*i] == "RIGHT") {
      if (is_where) {
        req.cond.push_back(cond_p);
        cond_p.first = true;
        cond_p.second.clear();
        is_where = false;
      }
      req.join = 2;
      *i += 1;
    } else if (data[*i] == "INNER") {
      if (is_where) {
        req.cond.push_back(cond_p);
        cond_p.first = true;
        cond_p.second.clear();
        is_where = false;
      }
      req.join = 3;
      *i += 1;
    } else if (data[*i] == "JOIN") {
      if (is_where) {
        req.cond.push_back(cond_p);
        cond_p.first = true;
        cond_p.second.clear();
        is_where = false;
      }
      if (req.join == 0) {
        req.join = 3;
      }
      *i += 1;
      bool flag = true;
      for (int j = 0; j < tables.size(); j++) {
        if (tables[j].name == data[*i]) {
          req.table2 = &tables[j];
          flag = false;
          break;
        }
      }
      if (flag) {
        res += "Table \"";
        res += data[*i];
        res += "\" not found\n";
      }
      *i += 2;
      req.j_col1 = &data[*i];
      *i += 2;
      if (data[*i][(int)data[*i].size() - 1] == ';') {
        flag = true;
        data[*i].pop_back();
      }
      req.j_col2 = &data[*i];
      if (flag) {
        req.DoReq(res);
      }
      *i += 1;
    } else if (data[*i] == "WHERE") {
      req.is_where = true;
      is_where = true;
      *i += 1;
    } else if (data[*i] == "NOT") {
      cond_p.first = false;
      *i += 1;
    } else if (data[*i] == "NULL" || data[*i] == "NULL;") {
      cond_p.second.push_back(&data[*i]); //If NULL then req.cond[i].size = 2 else req.cond[i].size = 3
      if (data[*i] == "NULL;") {
        data[*i].pop_back();
        req.cond.push_back(cond_p);
        cond_p.first = true;
        cond_p.second.clear();
        req.DoReq(res);
        is_where = false;
      }
      *i += 1;
    } else if (data[*i] == "IS") {
      *i += 1;
    } else if (data[*i] == "OR") {
      req.cond.push_back(cond_p);
      cond_p.first = true;
      cond_p.second.clear();
      req.cond_op.push_back(2);
      *i += 1;
    } else if (data[*i] == "AND") {
      req.cond.push_back(cond_p);
      cond_p.first = true;
      cond_p.second.clear();
      req.cond_op.push_back(1);
      *i += 1;
    } else if (is_where) {
      bool flag = false;
      if (data[*i][(int)data[*i].size() - 1] == ';') {
        flag = true;
        data[*i].pop_back();
      }
      if (data[*i][0] == '\'') {
        data[*i] = data[*i].substr(1);
      }
      if (data[*i][(int)data[*i].size() - 1] == '\'') {
        data[*i].pop_back();
      }
      cond_p.second.push_back(&data[*i]);
      if (flag) {
        req.cond.push_back(cond_p);
        cond_p.first = true;
        cond_p.second.clear();
        req.DoReq(res);
        is_where = false;
      }
      *i += 1;
    } else {
      res += "Wrong operator\n";
      return 1;
    }
  }

  return 0;
}

std::string MyCoolDB::RequestDB(std::string& req_str) {
  std::vector<std::string> req;      //split string into words
  std::string cur_word;
  bool flag = true;
  for (int i = 0; i < req_str.size(); i++) {
    if (req_str[i] == '\'') {
      if (flag) {
        flag = false;
      } else {
        flag = true;
      }
    }
    if ((req_str[i] == ' ' || req_str[i] == '\n' || req_str[i] == '\r') && flag) {
      if (!cur_word.empty()) {
        req.push_back(cur_word);
        cur_word.clear();
      }
    } else {
      cur_word += req_str[i];
    }
  }
  if (!cur_word.empty()) {
    req.push_back(cur_word);
  }

  std::string res;
  int cur_i = 0;
  if (Parse(req, &cur_i, res)) {
    return res;
  }
  return res;
}