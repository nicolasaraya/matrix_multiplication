#ifndef BICLIQUE_BOOLEAN
#define BICLIQUE_BOOLEAN

#include "matrix_boolean.hpp"

#include <cstdint>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <queue>

//typedef std::shared_ptr<std::vector<uint32_t>> vector32;
typedef std::vector<uint32_t>* vector32;

struct Inters_Bicl
{
  std::vector<uint32_t> S; 
  std::vector<uint32_t> C;

  uint32_t s_idx = 0;
  uint32_t c_idx = 0;

  uint32_t getRow() const
  {
    return S[s_idx];
  };

  uint32_t getCol() const
  {
    return C[c_idx];
  };

  struct Col_Comp 
  {
    bool operator()(const Inters_Bicl& a, const Inters_Bicl& b) const 
    {
      return a.C[a.c_idx] > b.C[b.c_idx];
    }
  };


  struct Row_Comp
  {
    bool operator()(Inters_Bicl& a, Inters_Bicl& b) const 
    {
      return a.S[a.s_idx] > b.S[b.s_idx];
    }
  };
};

typedef std::priority_queue<Inters_Bicl, std::vector<Inters_Bicl>, Inters_Bicl::Col_Comp> PQ_Col_Bic;
typedef std::priority_queue<Inters_Bicl, std::vector<Inters_Bicl>, Inters_Bicl::Row_Comp> PQ_Row_Bic;

struct csr_biclique 
{
  vector32 col_ind = nullptr;
  vector32 row_id = nullptr;

  csr_biclique()
  {
    col_ind = new std::vector<uint32_t>();
    row_id = new std::vector<uint32_t>();
  }

  ~csr_biclique()
  {
    delete col_ind;
    delete row_id;
  }

  inline const uint32_t* colIndPtr() const noexcept { return col_ind->data(); }
  inline const uint32_t* rowIdPtr()  const noexcept { return row_id->data(); }

  inline size_t nCols()    const noexcept { return col_ind->size(); }
  inline size_t nRows()    const noexcept { return row_id->size(); }

  void print()
  {
    std::cout << "Biclique csr:" << std::endl; 
    std::cout << std::endl << "col_ind: ";
    for (auto &i : *col_ind) {
      std::cout << i << " ";
    }
    std::cout << std::endl << "row_id: ";
    for (auto &i : *row_id) {
      std::cout << i << " ";
    }
    std::cout << std::endl;
    return;
  }

};

struct csc_biclique
{
  vector32 row_ind = nullptr; 
  vector32 col_id = nullptr;
  bool shared = false;

  csc_biclique()
  {
    row_ind = new std::vector<uint32_t>();
    col_id = new std::vector<uint32_t>();
  }

  ~csc_biclique()
  {
    if (not shared) {
      delete row_ind;
      delete col_id;
    }
  }

  void print()
  {
    std::cout << "Biclique csc:" << std::endl; 
    std::cout << std::endl << "row_ind: ";
    for (auto &i : *row_ind) {
      std::cout << i << " ";
    }
    std::cout << std::endl << "col_id: ";
    for (auto &i : *col_id) {
      std::cout << i << " ";
    }
    std::cout << std::endl;
    return;
  }
};

class Biclique
{
  public:
    Biclique(void);
    Biclique(std::string path);
    Biclique(std::vector<Inters_Bicl>* inters);
    ~Biclique();
    void setFile(std::string path);
    void make_csr_bin();
    void make_csr();
    void make_csc();
    void make_csc_usingPtr();

    void add_csr(csr_biclique* bic);
    void update_marks(std::map<uint32_t, std::vector<uint32_t>>& tempMark);
    void update_marks(std::vector<std::pair<uint32_t, std::vector<uint32_t>>>* newMarks);

    void addBiclique(Biclique* bic);

    void print_csr();
    void print_csc();
    void printMarks();

    std::vector<csr_biclique*>* get_csr();
    std::vector<csc_biclique*>* get_csc();
    std::vector<std::pair<uint32_t, std::vector<uint32_t>>>* get_marks();
    std::vector<uint32_t>* get_indexes(uint32_t);
    void delete_csr();
    void delete_csc();
    uint32_t maxCol();
    uint32_t maxRow();
    uint32_t maxDim();
    uint64_t getNumEdges();
    size_t countBicliques() { return csr->size(); }
    void saveTxt() const;
    void saveTxt(std::string pathFile) const;
    void saveBin() const;
    void saveBin(std::string pathFile) const;
    csr_matrix* toMatrix();
    static csr_matrix* csrFromInters(std::vector<Inters_Bicl>* inters);
    std::string getPath() { return path; }

  private:
    std::string path = "noname_biclique.txt";
    bool binaryFormat = false;
    std::vector<csr_biclique*>* csr = nullptr;
    std::vector<csc_biclique*>* csc = nullptr;
    std::vector<std::pair<uint32_t, std::vector<uint32_t>>>* marks = nullptr;
    uint64_t num_edges = 0;
    uint32_t max_col = 0;
    uint32_t max_row = 0; 
    uint32_t binary_search (uint32_t, uint32_t, uint32_t);
};

#endif