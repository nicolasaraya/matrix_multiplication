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


struct Res_bic {
    std::vector<uint32_t>* S;
    std::vector<std::pair<uint32_t,uint32_t>>* C; 

    uint32_t key_s = 0;
    uint32_t key_c = 0;
    uint32_t index_s = 0;
    uint32_t index_c = 0;

    static bool compare(const Res_bic& a, const Res_bic& b) 
    {
        // Comparar por el primer elemento de S
        if ((not a.S->empty()) and (not b.S->empty())) {
            if (a.S->at(a.index_s) != b.S->at(b.index_s)) {
                return a.S->at(a.index_s) > b.S->at(b.index_s);  // Ordenar de menor a mayor en S
            }
        }

        // Comparar por el primer elemento de C si S es igual
        if ((not a.C->empty()) and (not b.C->empty())) {
            return a.C->at(a.index_c).first > b.C->at(b.index_c).first;  // Ordenar de menor a mayor en C
        }

        // En caso de empate, no cambia el orden
        return false;
    }

    static bool compare_ptr(const Res_bic* a, const Res_bic* b) 
    {
        // Comparar por el primer elemento de S
        if ((not a->S->empty()) and (not b->S->empty())) {
            if (a->S->at(a->index_s) != b->S->at(b->index_s)) {
                return a->S->at(a->index_s) > b->S->at(b->index_s);  // Ordenar de menor a mayor en S
            }
        }

        // Comparar por el primer elemento de C si S es igual
        if ((not a->C->empty()) and (not b->C->empty())) {
            return a->C->at(a->index_c).first > b->C->at(b->index_c).first;  // Ordenar de menor a mayor en C
        }

        // En caso de empate, no cambia el orden
        return false;
    }

    struct comp_row
    {
        bool operator()(const Res_bic& a, const Res_bic& b) const {
            return (a.key_s) > (b.key_s);
        }

    };
  
    struct comp_col
    {
        bool operator()(const Res_bic& a, const Res_bic& b) const {
            return (a.key_c) > (b.key_c);
        }

    };

    struct comp_comb
    {
        bool operator()(const Res_bic& a, const Res_bic& b) const {
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