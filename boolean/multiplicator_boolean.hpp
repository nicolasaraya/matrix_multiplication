#ifndef MULT_BOOLEAN
#define MULT_BOOLEAN

#include <matrix_boolean.hpp>
#include <biclique_boolean.hpp>

#include <cstdint>
#include <iostream>
#include <vector>
#include <queue>

namespace boolean
{

    struct Intersection
    {
        uint32_t start_col;
        uint32_t end_col;
        uint32_t start_row;
        uint32_t end_row;
        uint32_t value_col;
        uint32_t value_row; 

    
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
    Matrix* mat_pow(Matrix* A);

    csr_matrix* mult(csc_matrix* A, csr_matrix* B);     //AxA

    csr_matrix* csr_add(csr_matrix* A, csr_matrix* B);

    csr_matrix* compute_intersections(std::vector<Intersection>* inters, csc_matrix* A_csc, csr_matrix* B_csr); //AxA

    std::vector<Intersection>* get_intersections(csc_matrix* A, csr_matrix* B);     //AxA

};
#endif