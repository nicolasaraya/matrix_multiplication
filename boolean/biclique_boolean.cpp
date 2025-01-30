#include <biclique_boolean.hpp>
#include <Utils.hpp>

#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <cassert>
#include <iostream>
#include <cstdint>

namespace Boolean
{
  Biclique::Biclique(std::string path)
  {
    setFile(path);
    csr = new std::vector<csr_biclique*>();
    csc = new std::vector<csc_biclique*>();
    marks = new std::vector<std::pair<uint32_t, std::vector<uint32_t>>>();
    make_csr();
    make_csc();
  }

  Biclique::~Biclique()
  {
    if (csc) {
      delete_csc();
      delete csc;
    }
    if (csr) {
      delete_csr();
      delete csr;
    }
    delete marks;
  }

  void Biclique::setFile(std::string path)
  {
    this->path = path; 
  }

  void Biclique::make_csr()
  {
    TIMERSTART(BUILD_CSR_BICLIQUE);

    std::ifstream file;
    file.open(path);
    assert(file.is_open());

    std::string s;

    std::map<uint32_t, std::vector<uint32_t>> tempMark;

    while (getline(file, s)) {
      auto b = new csr_biclique();
      csr->push_back(b);

      auto values = utils::splitString(s, ";");
      auto S = utils::splitString(values[0], " ");
      auto C = utils::splitString(values[1], " ");

      for (size_t i = 0; i < S.size(); i++) {
        b->row_id.push_back(atoll(S[i].c_str()));
        tempMark[b->row_id.back()].push_back(csr->size()-1);
      }
      for (size_t i = 0; i < C.size(); i++) {
        auto temp = C[i];
        temp.erase(temp.begin());
        temp.pop_back();
        auto sp = utils::splitString(temp, ",");
        b->col_ind.push_back(atoll(sp[0].c_str()));
        //b->values.push_back(atoll(sp[1].c_str()));
      }
      if (max_row < b->row_id.back()) max_row = b->row_id.back();
      num_edges += C.size() * S.size();;

    }
    file.close();

    for (auto i : tempMark) {
      marks->emplace_back(i.first, i.second);
    }

    std::cout << "edges in bicliques: " << num_edges << std::endl;

    TIMERSTOP(BUILD_CSR_BICLIQUE);
  }

  void Biclique::make_csr_bin()
  {
    return;
  }

  void Biclique::make_csc() 
  {
    assert(csr != nullptr);

    TIMERSTART(BUILD_CSC_BICLIQUE);

    for (auto i : *csr) {
      auto b = new csc_biclique();
      csc->push_back(b);
      //b->values = i->values;
      b->row_ind = i->row_id;
      b->col_id = i->col_ind;

      if (max_col < b->col_id.back()) max_col = b->col_id.back();
    }

    TIMERSTOP(BUILD_CSC_BICLIQUE);
    return;
  }

  void Biclique::print_csr()
  {
    for (auto i : *csr) {
      if (i != nullptr) i->print();
      std::cout << std::endl;
    }
  }

  void Biclique::print_csc()
  {
    for (auto i : *csc) {
      if (i != nullptr) i->print();
      std::cout << std::endl;
    }
  }

  std::vector<csr_biclique*>* Biclique::get_csr()
  {
    return csr;
  }

  std::vector<csc_biclique*>* Biclique::get_csc()
  {
    return csc;
  }

  std::vector<std::pair<uint32_t, std::vector<uint32_t>>>* Biclique::get_marks()
  {
    return marks;
  }

  std::vector<uint32_t>* Biclique::get_indexes(uint32_t id)
  {
    auto search = binary_search(0, marks->size() - 1, id);
    if (search != UINT32_MAX) {
      return &(marks->at(search).second);
    }
    return nullptr;
  }

  uint32_t Biclique::binary_search (uint32_t l, uint32_t r, uint32_t id) 
  {
    if (r >= l) {
      uint32_t mid = l + (r - l) / 2;

      if (marks->at(mid).first == id){
        return mid;
      }
      if (marks->at(mid).first > id){
        if(mid == 0){
          return UINT32_MAX;
        }
        return binary_search(l, mid - 1, id);
      }
      return binary_search(mid + 1, r, id);
    }
    return UINT32_MAX;
  }

  void Biclique::delete_csr()
  {
    for (auto it : *csr) {
      delete it;
    }
    delete csr;
    csr = nullptr;
  }

  void Biclique::delete_csc()
  {
    for (auto it : *csc) {
      delete it;
    }
    delete csc;
    csc = nullptr;
  }

  uint64_t Biclique::getNumEdges()
  {
    return num_edges;
  }

  uint32_t Biclique::maxCol()
  {
    return max_col;
  }

  uint32_t Biclique::maxRow()
  {
    return max_row;
  }

  uint32_t Biclique::maxDim(){
    if (max_col > max_row) {
      return max_col;
    }
    return max_row;
  }
};
