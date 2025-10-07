#ifndef MATRIX_BOOLEAN
#define MATRIX_BOOLEAN

#include <vector>
#include <iostream>
#include <cstdint>
#include <fstream>
#include <cassert>
#include <sstream>

#include "Utils.hpp"

struct csr_matrix
{
  std::vector<uint32_t> col_ind;
  std::vector<size_t> row_ptr;
  std::vector<uint32_t> row_id;
  uint32_t max_row = 0;

  csr_matrix() = default;
  
  csr_matrix(uint32_t nrows, size_t ncols) 
  {
    col_ind.reserve(ncols + 1); 
    row_ptr.reserve(ncols + 1);
    row_id.reserve(nrows + 1);
    return;
  }

  ~csr_matrix()
  {
    std::vector<uint32_t>().swap(col_ind);
    std::vector<size_t>().swap(row_ptr);
    std::vector<uint32_t>().swap(row_id);
  }
  
  // Getters de punteros (lectura)
  inline const uint32_t* colIndPtr()  const noexcept { return col_ind.data(); }
  inline const size_t* rowPtrPtr()  const noexcept { return row_ptr.data(); }
  inline const uint32_t* rowIdPtr()   const noexcept { return row_id.data(); }

  inline  uint32_t* colIndPtr()   noexcept { return col_ind.data(); }
  inline  size_t* rowPtrPtr()   noexcept { return row_ptr.data(); }
  inline  uint32_t* rowIdPtr()    noexcept { return row_id.data(); }

  // Tamaños cacheados para bucles
  inline size_t nEdges()    const noexcept { return col_ind.size(); }
  inline size_t nRows()    const noexcept { return row_id.size(); }

  void print()
  {
    std::cout << std::endl << "col_ind: ";
    for (auto& i : col_ind) {
      std::cout << i << " ";
    } 
    std::cout << std::endl << "row_ptr: "; 
    for (auto& i : row_ptr) {
      std::cout << i << " ";
    } 
    std::cout << std::endl << "row_id: ";
    for (auto& i : row_id) {
      std::cout << i << " ";
    } 
    std::cout << std::endl;
    return;
  }

  void printAsList()
  {
    for (size_t i = 0; i < row_id.size(); ++i) {
      size_t start = row_ptr.at(i);
      size_t stop = row_ptr.at(i+1);

      if (start == stop) continue;

      std::cout << row_id.at(i) << ":";

      while (start < stop) {
        std::cout << " " << col_ind.at(start);
        ++start;
      }
      std::cout << std::endl;
    }
    return;
  }
};

struct csc_matrix
{
  std::vector<uint32_t> row_ind;
  std::vector<size_t> col_ptr;
  std::vector<uint32_t> col_id;

  uint32_t max_col = 0;

  ~csc_matrix()
  {
    std::vector<uint32_t>().swap(row_ind);
    std::vector<size_t>().swap(col_ptr);
    std::vector<uint32_t>().swap(col_id);
  }

  inline const uint32_t* rowIndPtr()  const noexcept { return row_ind.data(); }
  inline const size_t* colPtrPtr()  const noexcept { return col_ptr.data(); }
  inline const uint32_t* colIdPtr()   const noexcept { return col_id.data(); }

  inline size_t nEdges()      const noexcept { return row_ind.size(); }
  inline size_t nCols()    const noexcept { return col_id.size(); }

  void print()
  {
    std::cout << std::endl << "row_ind: ";
    for (auto& i : row_ind) {
      std::cout << i << " ";
    } 
    std::cout << std::endl << "col_ptr: "; 
    for (auto& i : col_ptr) {
      std::cout << i << " ";
    } 
    std::cout << std::endl << "col_id: ";
    for (auto& i : col_id) {
      std::cout << i << " ";
    } 
    std::cout << std::endl;
    return;
  }
};

class Matrix {
  public:
    Matrix() = default;
    Matrix(std::string path);
    ~Matrix();
    void setFile(std::string path); 
    csr_matrix* make_csr();
    csc_matrix* make_csc();
    csr_matrix* make_csr_bin();
    csr_matrix* make_csr_bin16();
    csr_matrix* get_csr();
    csc_matrix* get_csc();
    void set_csr(csr_matrix* mat);
    void set_csc(csc_matrix* mat);
    void delete_csr();
    void delete_csc();
    void saveTxt();
    void saveTxt(std::string path);
    void saveBin();
    void saveBin(std::string path);
    void saveBin16();
    void saveBin16(std::string path);
    std::string getPath() { return path; }
    void setPath(std::string newPath) { path = newPath; }
    void setUseDelta16(bool b) { useDelta16 = b; }
    bool getUseDelta16() { return useDelta16; }

  private:
    std::string path = "";
    csr_matrix* csr = nullptr;
    csc_matrix* csc = nullptr; 
    bool useDelta16 = false;
};


#endif