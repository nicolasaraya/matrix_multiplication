#include "biclique_boolean.hpp"
#include "Utils.hpp"
#include "matrix_boolean.hpp"

#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <cassert>
#include <iostream>
#include <cstdint>
#include <sstream>
#include <algorithm>

Biclique::Biclique(void)
{
  csr = new std::vector<csr_biclique*>();
  csc = new std::vector<csc_biclique*>();
  marks = new std::vector<std::pair<uint32_t, std::vector<uint32_t>>>();
}

Biclique::Biclique(std::string path) : Biclique()
{
  setFile(path);

  if (utils::validateExtension(path, "txt")) {
    binaryFormat = false;
  } else if (utils::validateExtension(path, "bin")) {
    binaryFormat = true;
  }
}

Biclique::Biclique(std::vector<Inters_Bicl>* intersections) : Biclique()
{
  TIMERSTART(BUILD_CSR_BICLIQUE_FROM_INTERS);

  std::map<uint32_t, std::vector<uint32_t>> tempMark;
  for (auto bic : *intersections) {
    if (bic.S.empty() or bic.C.empty()) {
      continue;
    }

    auto b = new csr_biclique();
    csr->push_back(b);


    for (size_t i = 0; i < bic.S.size(); i++) {
      b->row_id->push_back(bic.S[i]);
      tempMark[b->row_id->back()].push_back(csr->size()-1);
    }
    for (size_t i = 0; i < bic.C.size(); i++) {
      b->col_ind->push_back(bic.C[i]);
    }
    if (max_row < b->row_id->back()) max_row = b->row_id->back();
    num_edges += bic.C.size() * bic.S.size();
  }

  for (auto i : tempMark) {
    marks->emplace_back(i.first, i.second);
  }

  delete intersections;
  std::cout << "Bicliques readed: " << csr->size() << std::endl;
  std::cout << "Edges in bicliques: " << num_edges << std::endl;
  
  
  TIMERSTOP(BUILD_CSR_BICLIQUE_FROM_INTERS);
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
  if (max_row < bic->row_id->back()) {
    max_row = bic->row_id->back();
  }
  num_edges += bic->row_id->size() * bic->col_ind->size();
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
  if (binaryFormat) {
    make_csr_bin();
    return;
  }

  if (not csr->empty()) {
    std::cout << "CSR biclique already created." << std::endl;
    return;
  }

  TIMERSTART(BUILD_CSR_BICLIQUE);
  std::ifstream file;
  file.open(path);
  assert(file.is_open());

  std::string s;

  std::map<uint32_t, std::vector<uint32_t>> tempMark;
  //getline(file, s); //skip first line

  while (getline(file, s)) {
    if (s.find(":") == std::string::npos) {
      continue;
    }
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
      b->row_id->push_back(atoll(S[i].c_str()));
      tempMark[b->row_id->back()].push_back(csr->size()-1);
    }
    for (size_t i = 0; i < C.size(); i++) {
      b->col_ind->push_back(atoll(C[i].c_str()));
    }
    if (max_row < b->row_id->back()) {
      max_row = b->row_id->back();
    }
    num_edges += C.size() * S.size();
  }
  file.close();

  for (auto i : tempMark) {
    marks->emplace_back(i.first, i.second);
  }

  std::cout << "Bicliques readed: " << csr->size() << std::endl;
  std::cout << "Edges in bicliques: " << num_edges << std::endl;
  
  TIMERSTOP(BUILD_CSR_BICLIQUE);
}

void Biclique::make_csr_bin()
{
  if (not binaryFormat) {
    make_csr();
    return;
  }

  if (not csr->empty()) {
    std::cout << "CSR biclique already created." <<  std::endl;
    return;
  }

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
      b->row_id->push_back(value);
      tempMark[b->row_id->back()].push_back(csr->size()-1);
    }

    iss.read(reinterpret_cast<char*>(&size), sizeof(size));
    uint32_t C_size = size;

    std::vector<uint32_t> C;
    while (size--) {
      iss.read(reinterpret_cast<char*>(&value), sizeof(value));
      b->col_ind->push_back(value);
    }
    
    if (max_row < b->row_id->back()) max_row = b->row_id->back();
    num_edges += C_size * S_size;
  }

  for (auto i : tempMark) {
    marks->emplace_back(i.first, i.second);
  }
  std::cout << "Bicliques readed: " << csr->size() << std::endl;
  std::cout << "Edges in bicliques: " << num_edges << std::endl;

  TIMERSTOP(BUILD_CSR_BICLIQUE_BIN);
}

void Biclique::make_csc_usingPtr()
{
  if (not csc->empty()) {
    std::cout << "CSC biclique already created." << std::endl;
    return;
  }

  TIMERSTART(BUILD_CSC_BICLIQUE_USING_PTR);
  csc->reserve(csr->size());

  for (auto i : *csr) {
    auto b = new csc_biclique();
    delete b->col_id;
    delete b->row_ind;
    csc->push_back(b);
    //b->values = i->values;
    b->row_ind = i->row_id;
    b->col_id = i->col_ind;
    b->shared = true;

    if (max_col < b->col_id->back()) max_col = b->col_id->back();
  }

  assert(csc->size() == csr->size());
  TIMERSTOP(BUILD_CSC_BICLIQUE_USING_PTR);
  return;
}


void Biclique::make_csc() 
{
  assert(csr != nullptr);

  if (not csr->empty()) {
    make_csc_usingPtr();
    return;
  }

  TIMERSTART(BUILD_CSC_BICLIQUE);

  // TODO: here we need read again the biclique from file

  // for (auto i : *csr) {
  //   auto b = new csc_biclique();
  //   csc->push_back(b);
  //   //b->values = i->values;
  //   b->row_ind = i->row_id;
  //   b->col_id = i->col_ind;

  //   if (max_col < b->col_id->back()) max_col = b->col_id->back();
  // }

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
      it = nullptr;
    }
    csr->clear();
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
      it = nullptr;
    }
    csc->clear();
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

void Biclique::saveTxt() const
{
  return saveTxt(path);
}
void Biclique::saveTxt(std::string pathFile) const
{
  std::cout << "Saving: " << pathFile << std::endl;

  std::ofstream file;
  file.open(pathFile, std::ofstream::out | std::ofstream::trunc); // limpia el contenido del fichero

  size_t edges = 0;
  size_t nodes = 0;
  size_t bicliques = 0;

  for (auto &bic : *csr) {
    if (bic->row_id->empty() or bic->col_ind->empty()) {
      continue;
    }
    bicliques++;
    file << "S:";
    for (auto &s : *(bic->row_id)) {
      file << " " << s;
    }
    file << std::endl << "C:";
    for (auto &c : *(bic->col_ind)) {
      file << " " << c;
    }

    edges += bic->row_id->size() * bic->col_ind->size();
    nodes += bic->row_id->size();
    file << std::endl;
  }

  
  std::cout << "Edges: " << edges << std::endl;
  std::cout << "Nodes: " << nodes << std::endl;
  std::cout << "Bicliques: " << bicliques << std::endl;

  file.close();
}

void Biclique::saveBin() const
{
  return saveBin(path);
}

void Biclique::saveBin(std::string pathFile) const
{
  std::cout << "Saving: " << pathFile << std::endl;

  std::ofstream file;
  file.open(pathFile, std::ofstream::out | std::ofstream::trunc | std::ofstream::binary); 

  size_t edges = 0;
  size_t nodes = 0;
  size_t bicliques = 0;

  for (auto &bic : *csr) {
    if (bic->row_id->empty() or bic->col_ind->empty()) {
      continue;
    }
    bicliques++;
    uint32_t S_size = bic->row_id->size();
    file.write(reinterpret_cast<const char*>(&S_size), sizeof(S_size));
    file.write(reinterpret_cast<const char*>(bic->row_id->data()), S_size * sizeof(uint32_t));

    uint32_t C_size = bic->col_ind->size();
    file.write(reinterpret_cast<const char*>(&C_size), sizeof(C_size));
    file.write(reinterpret_cast<const char*>(bic->col_ind->data()), C_size * sizeof(uint32_t));

    edges += bic->row_id->size() * bic->col_ind->size();
    nodes += bic->row_id->size();
    file << std::endl;
  }

  std::cout << "Edges: " << edges << std::endl;
  std::cout << "Nodes: " << nodes << std::endl;
  std::cout << "Bicliques: " << bicliques << std::endl;
}

csr_matrix* Biclique::toMatrix()
{
  TIMERSTART(BUILD_MATRIX_FROM_BICLIQUES);
  csr_matrix* matrix = new csr_matrix();
  matrix->row_ptr.push_back(0);

  for (auto mark : *marks) {
    uint32_t row = mark.first;
    std::vector<uint32_t> adj;
    for (auto bic : mark.second) {
      auto biclique = csr->at(bic);
      assert(biclique->col_ind->size() > 0);
      adj.insert(adj.end(), biclique->col_ind->begin(), biclique->col_ind->end());
    }
    std::sort(adj.begin(), adj.end());
    adj.erase(std::unique(adj.begin(), adj.end()), adj.end());
    if (not matrix->row_id.empty()) {
      assert(row > matrix->row_id.back());
    }
    matrix->row_id.push_back(row);
    matrix->col_ind.insert(matrix->col_ind.end(), adj.begin(), adj.end());
    matrix->row_ptr.push_back(matrix->col_ind.size());
    if (row > matrix->max_row) {
      matrix->max_row = row;
    }
  }

  assert(marks->size() == matrix->nRows());
  matrix->col_ind.shrink_to_fit();
  matrix->row_id.shrink_to_fit();
  matrix->row_ptr.shrink_to_fit();

  std::cout << "Matrix generated from bicliques: " << std::endl;
  std::cout << "  - Rows: " << matrix->nRows() << std::endl;
  std::cout << "  - Edges: " << matrix->nEdges() << std::endl;

  TIMERSTOP(BUILD_MATRIX_FROM_BICLIQUES);
  return matrix;
}

csr_matrix* Biclique::csrFromInters(std::vector<Inters_Bicl>* inters)
{
  TIMERSTART(BUILD_CSR_MATRIX_FROM_INTERS_USING_HEAPS);
  auto* res = new csr_matrix();
  PQ_Row_Bic Hr(Inters_Bicl::Row_Comp(), std::move(*inters));
  PQ_Col_Bic Hc;
  delete inters;

  while (not Hr.empty()) {
    if (Hr.top().S.empty() or Hr.top().C.empty()) {
      Hr.pop();
      continue;
    }

    uint32_t currentRow = Hr.top().getRow();
    Hc.push(std::move(Hr.top()));
    Hr.pop();

    if (Hr.empty() or (Hr.top().getRow() !=  currentRow)) {

      res->row_id.push_back(currentRow);
      res->row_ptr.push_back(res->col_ind.size());

      std::vector<uint32_t> tempCols;

      while (not Hc.empty()) {
        auto bic = std::move(Hc.top());
        Hc.pop();
        if (tempCols.empty() or tempCols.back() != bic.getCol()) {
          tempCols.push_back(bic.getCol());
        }
        bic.c_idx++;
        if (bic.c_idx < bic.C.size()) {
          Hc.push(std::move(bic));
        } else {
          bic.c_idx = 0;
          bic.s_idx++;
          if (bic.s_idx < bic.S.size()) {
            Hr.push(std::move(bic));
          }
        }
      }
      res->col_ind.insert(res->col_ind.end(), tempCols.begin(), tempCols.end());
      res->row_ptr.push_back(res->col_ind.size());
    }
  }

  TIMERSTOP(BUILD_CSR_MATRIX_FROM_INTERS_USING_HEAPS);
  return res;
}
