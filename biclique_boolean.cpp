#include "biclique_boolean.hpp"
#include "Utils.hpp"

#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <cassert>
#include <iostream>
#include <cstdint>
#include <sstream>

Biclique::Biclique(void)
{
  csr = new std::vector<csr_biclique*>();
  csc = new std::vector<csc_biclique*>();
  marks = new std::vector<std::pair<uint32_t, std::vector<uint32_t>>>();
}

Biclique::Biclique(std::string path)
{
  setFile(path);
  csr = new std::vector<csr_biclique*>();
  csc = new std::vector<csc_biclique*>();
  marks = new std::vector<std::pair<uint32_t, std::vector<uint32_t>>>();
  if (utils::validateExtension(path, "txt")) {
    make_csr();
  } else if (utils::validateExtension(path, "bin")) {
    make_csr_bin();
  }
  make_csc();
}

Biclique::~Biclique()
{
  if (csc) {
    delete_csc();
  }
  if (csr) {
    delete_csr();
  }
  delete marks;
  marks = nullptr;
}

void Biclique::setFile(std::string path)
{
  this->path = path; 
}

void Biclique::add_csr(csr_biclique* bic)
{
  csr->push_back(bic);
  if (max_row < bic->row_id.back()) {
    max_row = bic->row_id.back();
  }
  num_edges += bic->row_id.size() * bic->col_ind.size();
}

void Biclique::addBiclique(Biclique* bic)
{
  size_t currentSize = csr->size();
  
  auto bMarksIt = bic->get_marks()->begin();
  auto localMarksIt = marks->begin();
  auto newMarks = new std::vector<std::pair<uint32_t, std::vector<uint32_t>>>();

  while (bMarksIt != bic->get_marks()->end() and localMarksIt != marks->end()) {
    auto bId = (*bMarksIt).first;
    auto aId = (*localMarksIt).first;
    if (aId == bId) {
      newMarks->emplace_back(*localMarksIt);
      for (auto&j : bMarksIt->second) {
        newMarks->back().second.push_back(currentSize + j);
      }
      bMarksIt++;
      localMarksIt++;
    } else if (aId < bId) {
      newMarks->emplace_back(*localMarksIt);
      localMarksIt++;
    } else if (aId > bId) {
      newMarks->emplace_back(bId, std::vector<uint32_t>());
      for (auto&j : bMarksIt->second) {
        newMarks->back().second.push_back(currentSize + j);
      }
      bMarksIt++;
    }
  }

  while (bMarksIt != bic->get_marks()->end()) {
    newMarks->emplace_back(bMarksIt->first, std::vector<uint32_t>());
    for (auto&j : bMarksIt->second) {
      newMarks->back().second.push_back(currentSize + j);
    }
    bMarksIt++;
  }

  while (localMarksIt != marks->end()) {
    newMarks->emplace_back(*localMarksIt);
    localMarksIt++;
  }

  delete marks;
  marks = newMarks;
  marks->shrink_to_fit();
  csr->insert(csr->end(), bic->csr->begin(), bic->csr->end());
  bic->csr->clear();
}

void Biclique::update_marks(std::map<uint32_t, std::vector<uint32_t>>& tempMark)
{
  for (auto i : tempMark) {
    marks->emplace_back(i.first, i.second);
  }
  std::cout << "Edges in bicliques: " << num_edges << std::endl;
}

void Biclique::update_marks(std::vector<std::pair<uint32_t, std::vector<uint32_t>>>* newMarks)
{
  marks = newMarks;
}

void Biclique::make_csr()
{
  TIMERSTART(BUILD_CSR_BICLIQUE);

  std::ifstream file;
  file.open(path);
  assert(file.is_open());

  std::string s;

  std::map<uint32_t, std::vector<uint32_t>> tempMark;
  //getline(file, s); //skip first line

  while (getline(file, s)) {
    //auto values = utils::splitString(s, ";");
    s.erase(0, 1); //remove S 
    s.erase(0, 1); //remove :
    auto S = utils::splitString(s, " ");

    getline(file, s);
    s.erase(0, 1); //remove C 
    s.erase(0, 1); //remove :
    auto C = utils::splitString(s, " ");

    if (S.empty() or C.empty()) {
      continue;
    }

    auto b = new csr_biclique();
    csr->push_back(b);

    for (size_t i = 0; i < S.size(); i++) {
      b->row_id.push_back(atoll(S[i].c_str()));
      tempMark[b->row_id.back()].push_back(csr->size()-1);
    }
    for (size_t i = 0; i < C.size(); i++) {
      b->col_ind.push_back(atoll(C[i].c_str()));
    }
    if (max_row < b->row_id.back()) max_row = b->row_id.back();
    num_edges += C.size() * S.size();
  }
  file.close();

  for (auto i : tempMark) {
    marks->emplace_back(i.first, i.second);
  }

  std::cout << "Edges in bicliques: " << num_edges << std::endl;
  
  TIMERSTOP(BUILD_CSR_BICLIQUE);
}

void Biclique::make_csr_bin()
{
  TIMERSTART(BUILD_CSR_BICLIQUE_BIN);

  std::ifstream file;
  file.open(path, std::ios::in | std::ios::binary);
  assert(file.is_open());

  file.seekg(0, std::ios::end);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<char> buffer(size);
  if (file.read(buffer.data(), size)) {
      std::cout << size << " bytes readed from " << path << std::endl;
  }
  file.close();

  std::string blob(buffer.data(), buffer.size());
  std::istringstream iss(blob, std::ios::in | std::ios::binary);
  buffer.clear();

  std::map<uint32_t, std::vector<uint32_t>> tempMark;

  while (iss.peek() != EOF) {
    auto* b = new csr_biclique();
    csr->push_back(b);

    uint32_t size;
    iss.read(reinterpret_cast<char*>(&size), sizeof(size));
    
    uint32_t S_size = size;
    uint32_t value;
    while (size--) {
      iss.read(reinterpret_cast<char*>(&value), sizeof(value));
      b->row_id.push_back(value);
      tempMark[b->row_id.back()].push_back(csr->size()-1);
    }

    iss.read(reinterpret_cast<char*>(&size), sizeof(size));
    uint32_t C_size = size;

    std::vector<uint32_t> C;
    while (size--) {
      iss.read(reinterpret_cast<char*>(&value), sizeof(value));
      b->col_ind.push_back(value);
    }
    
    if (max_row < b->row_id.back()) max_row = b->row_id.back();
    num_edges += C_size * S_size;
  }

  for (auto i : tempMark) {
    marks->emplace_back(i.first, i.second);
  }

  std::cout << "Edges in bicliques: " << num_edges << std::endl;

  TIMERSTOP(BUILD_CSR_BICLIQUE_BIN);
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
  assert(not csr->empty());
  size_t count = 0; 
  for (auto i : *csr) {
    std::cout << "Biclique " << ++count << ": " << std::endl;
    if (i != nullptr) i->print();
    std::cout << std::endl;
  }
}

void Biclique::print_csc()
{
  size_t count = 0; 
  for (auto i : *csc) {
    std::cout << "Biclique " << ++count;
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

uint32_t Biclique::binary_search(uint32_t l, uint32_t r, uint32_t id) 
{
  if (r >= l) {
    uint32_t mid = l + (r - l) / 2;

    if (marks->at(mid).first == id) {
      return mid;
    }
    if (marks->at(mid).first > id) {
      if (mid == 0) {
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
  if (csr) {
    for (auto it : *csr) {
      delete it;
    }
    delete csr;
    csr = nullptr;
  }
  return;
}

void Biclique::delete_csc()
{
  if (csc) {
    for (auto it : *csc) {
      delete it;
    }
    delete csc;
    csc = nullptr;
  }
  return;
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

uint32_t Biclique::maxDim()
{
  return (max_col > max_row) ? max_col : max_row;
}

void Biclique::printMarks()
{
  for (auto& i : *marks) {
    std::cout << i.first << ":";
    for (auto& j : i.second) {
      std::cout << " " << j;
    }
    std::cout << std::endl;
  }
}

void Biclique::saveTxt()
{
  return saveTxt(path);
}
void Biclique::saveTxt(std::string pathFile)
{
  std::cout << "Saving: " << pathFile << std::endl;

  path = pathFile;
  std::ofstream file;
  file.open(pathFile, std::ofstream::out | std::ofstream::trunc); // limpia el contenido del fichero

  size_t edges = 0;
  size_t nodes = 0;
  size_t bicliques = 0;

  for (auto &bic : *csr) {
    if (bic->row_id.empty() or bic->col_ind.empty()) {
      continue;
    }
    bicliques++;
    file << "S:";
    for (auto &s : bic->row_id) {
      file << " " << s;
    }
    file << std::endl << "C:";
    for (auto &c : bic->col_ind) {
      file << " " << c;
    }

    edges += bic->row_id.size() * bic->col_ind.size();
    nodes += bic->row_id.size();
    file << std::endl;
  }

  
  std::cout << "Edges: " << edges << std::endl;
  std::cout << "Nodes: " << nodes << std::endl;
  std::cout << "Bicliques: " << bicliques << std::endl;

  file.close();
}

void Biclique::saveBin()
{
  return saveBin(path);
}

void Biclique::saveBin(std::string pathFile)
{
  std::cout << "Saving: " << pathFile << std::endl;

  path = pathFile;
  std::ofstream file;
  file.open(pathFile, std::ofstream::out | std::ofstream::trunc | std::ofstream::binary); 

  size_t edges = 0;
  size_t nodes = 0;
  size_t bicliques = 0;

  for (auto &bic : *csr) {
    if (bic->row_id.empty() or bic->col_ind.empty()) {
      continue;
    }
    bicliques++;
    uint32_t S_size = bic->row_id.size();
    file.write(reinterpret_cast<const char*>(&S_size), sizeof(S_size));
    file.write(reinterpret_cast<const char*>(bic->row_id.data()), S_size * sizeof(uint32_t));

    uint32_t C_size = bic->col_ind.size();
    file.write(reinterpret_cast<const char*>(&C_size), sizeof(C_size));
    file.write(reinterpret_cast<const char*>(bic->col_ind.data()), C_size * sizeof(uint32_t));

    edges += bic->row_id.size() * bic->col_ind.size();
    nodes += bic->row_id.size();
    file << std::endl;
  }

  std::cout << "Edges: " << edges << std::endl;
  std::cout << "Nodes: " << nodes << std::endl;
  std::cout << "Bicliques: " << bicliques << std::endl;
}
