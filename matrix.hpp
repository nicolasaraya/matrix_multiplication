#ifndef MATRIX_CSR
#define MATRIX_CSR

#include <iostream>
#include <cstdint>
#include <vector>
#include <cassert>
#include <fstream>
#include <set>
#include <unordered_map>

#include <include/Utils/Utils.hpp>

namespace weighted
{

  struct csr_matrix 
  {
    std::vector<int32_t> values; 
    std::vector<uint32_t> col_ind;
    std::vector<uint32_t> row_ptr;
    std::vector<uint32_t> row_id;

    uint32_t max_row = 0;

    void print()
    {
      std::cout << "values: ";
      for (auto i : values) {
        std::cout << i << " "; 
      } 
      std::cout << std::endl << "col_ind: ";
      for (auto i : col_ind) {
        std::cout << i << " ";
      } 
      std::cout << std::endl << "row_ptr: "; 
      for (auto i : row_ptr) {
        std::cout << i << " ";
      } 
      std::cout << std::endl << "row_id: ";
      for (auto i : row_id) {
        std::cout << i << " ";
      } 
      std::cout << std::endl;

    }
  };

  struct csc_matrix 
  {
    std::vector<int32_t> values; 
    std::vector<uint32_t> row_ind;
    std::vector<uint32_t> col_ptr;
    std::vector<uint32_t> col_id;

    uint32_t max_col = 0;

    void print()
    {
      std::cout << "values: ";
      for (auto i : values) {
        std::cout << i << " "; 
      } 
      std::cout << std::endl << "row_ind: ";
      for (auto i : row_ind) {
        std::cout << i << " ";
      } 
      std::cout << std::endl << "col_ptr: "; 
      for (auto i : col_ptr) {
        std::cout << i << " ";
      } 
      std::cout << std::endl << "col_id: ";
      for (auto i : col_id) {
        std::cout << i << " ";
      } 
      std::cout << std::endl;

    }
  };



  class Matrix {
    public:
      Matrix() = default;
      Matrix(std::string path, bool b_csr, bool b_csc);
      Matrix(std::string path);
      ~Matrix();
      void setFile(std::string path); 
      csr_matrix* make_csr_bin();
      csr_matrix* make_csr();
      csc_matrix* make_csc();
      csr_matrix* get_csr();
      csc_matrix* get_csc();
      void set_csr(csr_matrix* mat);
      void set_csc(csc_matrix* mat);
      void delete_csr();
      void delete_csc();
      void saveTxt();
      void saveTxt(std::string path);
    private:
      std::string path = "";
      csr_matrix* csr = nullptr;
      csc_matrix* csc = nullptr; 
  };

};
#endif