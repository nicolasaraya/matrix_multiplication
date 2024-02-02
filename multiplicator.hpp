#ifndef MULTIPLICATOR_HPP
#define MULTIPLICATOR_HPP

#include <matrix.hpp>
#include <biclique.hpp>

#include <cstdint>
#include <iostream>
#include <vector>
#include <queue>


struct Intersection
{
    uint32_t start_col;
    uint32_t end_col;
    uint32_t start_row;
    uint32_t end_row;
    uint32_t value_col;
    uint32_t value_row; 
    uint32_t index_bicl;
   
    // Definir el operador de comparación para la cola de prioridad
    struct Col_Comp 
    {
        bool operator()(const Intersection& a, const Intersection& b) const {
            return a.value_col > b.value_col;
        }
    };

    // Definir el operador de comparación para la segunda cola
    struct Row_Comp 
    {
        bool operator()(const Intersection& a, const Intersection& b) const {
            return a.value_row > b.value_row;
        }
    };
};

const auto min_heap_comp = [](const auto& a, const auto& b) {
    return a > b;  // Cambiar a < para ordenar de menor a mayor
};


struct Inters_Bicl {
    std::vector<uint32_t>* row_id; // S 
    //std::vector<std::pair<uint32_t,uint32_t>>* C; 
    std::vector<uint32_t> col_ind; //C
    std::vector<uint32_t> values;

    uint32_t key_s = 0;
    uint32_t key_c = 0;
    uint32_t index_s = 0;
    uint32_t index_c = 0;

    static bool compare(const Inters_Bicl& a, const Inters_Bicl& b) 
    {
        // Comparar por el primer elemento de S
        if ((not a.row_id->empty()) and (not b.row_id->empty())) {
            if (a.row_id->at(a.index_s) != b.row_id->at(b.index_s)) {
                return a.row_id->at(a.index_s) > b.row_id->at(b.index_s);  // Ordenar de menor a mayor en S
            }
        }

        // Comparar por el primer elemento de C si S es igual
        if ((not a.col_ind.empty()) and (not b.col_ind.empty())) {
            return a.col_ind.at(a.index_c) > b.col_ind.at(b.index_c);  // Ordenar de menor a mayor en C
        }

        // En caso de empate, no cambia el orden
        return false;
    }

    static bool compare_ptr(const Inters_Bicl* a, const Inters_Bicl* b) 
    {
        // Comparar por el primer elemento de S
        if ((not a->row_id->empty()) and (not b->row_id->empty())) {
            if (a->row_id->at(a->index_s) != b->row_id->at(b->index_s)) {
                return a->row_id->at(a->index_s) > b->row_id->at(b->index_s);  // Ordenar de menor a mayor en S
            }
        }

        // Comparar por el primer elemento de C si S es igual
        if ((not a->col_ind.empty()) and (not b->col_ind.empty())) {
            return a->col_ind.at(a->index_c) > b->col_ind.at(b->index_c);  // Ordenar de menor a mayor en C
        }

        // En caso de empate, no cambia el orden
        return false;
    }

    struct comp_row
    {
        bool operator()(const Inters_Bicl& a, const Inters_Bicl& b) const {
            return (a.key_s) > (b.key_s);
        }

    };
  
    struct comp_col
    {
        bool operator()(const Inters_Bicl& a, const Inters_Bicl& b) const {
            return (a.key_c) > (b.key_c);
        }

    };

    struct comp_comb
    {
        bool operator()(const Inters_Bicl& a, const Inters_Bicl& b) const {
            if (a.key_s != b.key_s) {
                return a.key_s < b.key_s;
            } else {
                return a.key_c < b.key_c;
            }
        }
    };
};





Matrix* mat_mult(Matrix* A, Matrix* B);
Matrix* mat_mult(Matrix* A, Biclique* Ab, Matrix* B, Biclique* Bb);
Matrix* mat_pow(Matrix* A);
Matrix* mat_pow(Matrix* A, Biclique* b);

csr_matrix* mult(csc_matrix* A_csc, csr_matrix* B_csr); //AxA
csr_matrix* mult(csc_matrix* A, Biclique* b);   //Axb
csr_matrix* mult(Biclique* b, csr_matrix* A);   //bxA
csr_matrix* mult(Biclique* a, Biclique* b); //bxb

csr_matrix* csr_add(csr_matrix* A, csr_matrix* B);


//std::vector<Intersection>* get_intersections(csc_matrix* A, csr_matrix* B);     //AxA

#endif