#ifndef BICLIQUE_BOOLEAN
#define BICLIQUE_BOOLEAN

#include <cstdint>
#include <iostream>
#include <vector>
#include <string>

namespace Boolean 
{
  struct csr_biclique 
  {
    //std::vector<int32_t> values; 
    std::vector<uint32_t> col_ind;
    std::vector<uint32_t> row_id;

    void print()
    {
      // std::cout << "values: ";
      // for (auto i : values) {
      //   std::cout << i << " "; 
      // } 
      std::cout << std::endl << "col_ind: ";
      for (auto i : col_ind) {
        std::cout << i << " ";
      }
      std::cout << std::endl << "row_id: ";
      for (auto i : row_id) {
        std::cout << i << " ";
      }
      std::cout << std::endl;
    }
  };

  struct csc_biclique
  {
    //std::vector<int32_t> values; 
    std::vector<uint32_t> row_ind; 
    std::vector<uint32_t> col_id;

    void print()
    {
      // std::cout << "values: ";
      // for (auto i : values) {
      //   std::cout << i << " "; 
      // }
      std::cout << std::endl << "row_ind: ";
      for (auto i : row_ind) {
        std::cout << i << " ";
      }
      std::cout << std::endl << "col_id: ";
      for (auto i : col_id) {
        std::cout << i << " ";
      }
      std::cout << std::endl;
    }
  };

  class Biclique 
  {
    public:
      Biclique() = default;
      Biclique(std::string path);
      ~Biclique();
      void setFile(std::string path);
      void make_csr_bin();
      void make_csr();
      void make_csc();
      void print_csr();
      void print_csc();
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

    private:
      std::string path = "";
      std::vector<csr_biclique*>* csr = nullptr;
      std::vector<csc_biclique*>* csc = nullptr;
      std::vector<std::pair<uint32_t, std::vector<uint32_t>>>* marks;
      uint64_t num_edges = 0;
      uint32_t max_col = 0;
      uint32_t max_row = 0; 
      uint32_t binary_search (uint32_t, uint32_t, uint32_t);
  };
};

#endif