#ifndef BICLIQUE_BOOLEAN
#define BICLIQUE_BOOLEAN

#include <cstdint>
#include <iostream>
#include <vector>
#include <string>
#include <map>

struct csr_biclique 
{
  std::vector<uint32_t> col_ind;
  std::vector<uint32_t> row_id;

  ~csr_biclique()
  {
    std::vector<uint32_t>().swap(col_ind);
    std::vector<uint32_t>().swap(row_id);
  }

  inline const uint32_t* colIndPtr() const noexcept { return col_ind.data(); }
  inline const uint32_t* rowIdPtr()  const noexcept { return row_id.data(); }

  inline size_t nCols()    const noexcept { return col_ind.size(); }
  inline size_t nRows()    const noexcept { return row_id.size(); }

  void print()
  {
    std::cout << "Biclique csr:" << std::endl; 
    std::cout << std::endl << "col_ind: ";
    for (auto &i : col_ind) {
      std::cout << i << " ";
    }
    std::cout << std::endl << "row_id: ";
    for (auto &i : row_id) {
      std::cout << i << " ";
    }
    std::cout << std::endl;
    return;
  }

};

struct csc_biclique
{
  std::vector<uint32_t> row_ind; 
  std::vector<uint32_t> col_id;

  ~csc_biclique()
  {
    std::vector<uint32_t>().swap(row_ind);
    std::vector<uint32_t>().swap(col_id);
  }

  void print()
  {
    std::cout << "Biclique csc:" << std::endl; 
    std::cout << std::endl << "row_ind: ";
    for (auto &i : row_ind) {
      std::cout << i << " ";
    }
    std::cout << std::endl << "col_id: ";
    for (auto &i : col_id) {
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
    ~Biclique();
    void setFile(std::string path);
    void make_csr_bin();
    void make_csr();
    void make_csc();

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
    void saveTxt();
    void saveTxt(std::string pathFile);
    void saveBin();
    void saveBin(std::string pathFile);
    std::string getPath() { return path; }

  private:
    std::string path = "";
    std::vector<csr_biclique*>* csr = nullptr;
    std::vector<csc_biclique*>* csc = nullptr;
    std::vector<std::pair<uint32_t, std::vector<uint32_t>>>* marks = nullptr;
    uint64_t num_edges = 0;
    uint32_t max_col = 0;
    uint32_t max_row = 0; 
    uint32_t binary_search (uint32_t, uint32_t, uint32_t);
};

#endif