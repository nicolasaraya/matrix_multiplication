#ifndef BICLIQUE_CSR
#define BICLIQUE_CSR

#include <iostream>
#include <cstdint>
#include <vector>
#include <cassert>
#include <fstream>
#include <set>
#include <unordered_map>

#include <matrix.hpp>
#include <utils.hpp>


struct csr_biclique 
{
    std::vector<int32_t> values; 
    std::vector<uint32_t> col_ind;
    std::vector<uint32_t> row_ptr;
    //std::vector<uint32_t> row_id;
    std::vector<std::vector<uint32_t>> row_id;
    //std::vector<std::pair<uint32_t, std::vector<uint32_t>>> row_id_2; 

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
            std::cout << "{";
            for (auto j : i) {
                std::cout << j << " ";
            }
            std::cout << "}";
        }
        /*
        std::cout << std::endl << "row_id_2: ";
        for (auto i : row_id_2) {
            std::cout << i.first << ": "; 
            for (auto j : i.second ){
                std::cout << j << " ";
            }
            std::cout << std::endl;
        } 
        std::cout << std::endl;
        */

    }
};

//typedef csc_matrix csc_biclique;


struct csc_biclique
{
    std::vector<int32_t> values; 
    std::vector<uint32_t> row_ind; //index de row_id de csr. 
    std::vector<uint32_t> col_ptr;
    std::vector<uint32_t> col_id;


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


class Biclique {
    public:
        Biclique();
        Biclique(std::string path);
        ~Biclique();
        void setFile(std::string path); 
        void make_csr_bin();
        void make_csr();
        void make_csc();
        csr_biclique* get_csr();
        csc_biclique* get_csc();
        void set_csr(csr_biclique* mat);
        void set_csc(csc_biclique* mat);
        void delete_csr();
        void delete_csc();
        uint64_t getNumEdges(); 
    private:
        std::string path = "";
        csr_biclique* csr = nullptr;
        csc_biclique* csc = nullptr; 
        uint64_t num_edges = 0;
        //uint32_t max_edges = 0; 

};


#endif