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

    // Constructor
    Intersection(uint32_t start_col, uint32_t end_col, uint32_t start_row, uint32_t end_row, uint32_t value_col, uint32_t value_row) :   
                start_col(start_col), end_col(end_col), start_row(start_row), end_row(end_row), value_col(value_col), value_row(value_row)  {}

   
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



Matrix* mat_mult(Matrix* A, Matrix* B);
Matrix* mat_mult(Matrix* A, Biclique* Ab, Matrix* B, Biclique* Bb);
Matrix* mat_pow(Matrix* A);
Matrix* mat_pow(Matrix* A, Biclique* b);


csr_matrix* csr_mult(csc_matrix* A, csr_matrix* B); 
csr_matrix* csr_mult(csc_matrix* A, csr_biclique* b);
csr_matrix* csr_add(csr_matrix* A, csr_matrix* B);

template<typename csr_type> 
csr_matrix* compute_intersections(std::vector<Intersection>* inters, csc_matrix* A, csr_type* B);

std::vector<Intersection>* get_intersections(csc_matrix* A, csr_matrix* B);
std::vector<Intersection>* get_intersections(csc_matrix* A, csr_biclique* b);

#endif