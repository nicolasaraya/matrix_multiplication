#ifndef MULTIPLICATOR_HPP
#define MULTIPLICATOR_HPP

#include <matrix.hpp>
#include <biclique.hpp>

#include <cstdint>
#include <iostream>
#include <vector>
#include <queue>


struct Intersection_MM
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
        bool operator()(const Intersection_MM& a, const Intersection_MM& b) const {
            return a.value_col > b.value_col;
        }
    };

    // Definir el operador de comparación para la segunda cola
    struct Row_Comp 
    {
        bool operator()(const Intersection_MM& a, const Intersection_MM& b) const {
            return a.value_row > b.value_row;
        }
    };
};

struct Intersection_bb
{
    uint32_t col_id;
    uint32_t start_col;
    uint32_t end_col;
    uint32_t start_row;
    uint32_t end_row;
    uint32_t value_col;
    uint32_t value_row; 
    uint32_t weight;


    // Definir el operador de comparación para la cola de prioridad
    struct Col_Comp 
    {
        bool operator()(const Intersection_bb& a, const Intersection_bb& b) const {
            return a.value_col > b.value_col;
        }
    };

    // Definir el operador de comparación para la segunda cola
    struct Row_Comp 
    {
        bool operator()(const Intersection_bb& a, const Intersection_bb& b) const {
            return a.value_row > b.value_row;
        }
    };
};



Matrix* mat_mult(Matrix* A, Matrix* B);
Matrix* mat_mult(Matrix* A, Biclique* Ab, Matrix* B, Biclique* Bb);
Matrix* mat_pow(Matrix* A);
Matrix* mat_pow(Matrix* A, Biclique* b);

csr_matrix* mult(csc_matrix* A, csr_matrix* B);     //AxA
csr_matrix* mult(csc_matrix* A, csr_biclique* b);   //Axb
csr_matrix* mult(csr_biclique* csr_a, csc_biclique* csc_a, csr_matrix* B);   //bxA
csr_matrix* mult(csc_biclique* a, csr_biclique* b); //bxb

csr_matrix* csr_add(csr_matrix* A, csr_matrix* B);

csr_matrix* compute_intersections(std::vector<Intersection_MM>* inters, csc_matrix* A_csc, csr_matrix* B_csr); //AxA
csr_matrix* compute_intersections(std::vector<Intersection_MM>* inters, csc_matrix* A_csc, csr_biclique* b_csr); //Axb
csr_matrix* compute_intersections(std::vector<Intersection_bb>* inters, csr_biclique* b_csr, csc_biclique* b_csc, csr_matrix* A_csr); //bxA
csr_matrix* compute_intersections(std::vector<Intersection_bb>* inters, csc_biclique* b_csc, csr_biclique* b_csr); //bxb

std::vector<Intersection_MM>* get_intersections(csc_matrix* A, csr_matrix* B);     //AxA
std::vector<Intersection_MM>* get_intersections(csc_matrix* A, csr_biclique* b);   //Axb
std::vector<Intersection_bb>* get_intersections(csr_biclique* csr_b, csc_biclique* csc_b, csr_matrix* A);   //bxA
std::vector<Intersection_bb>* get_intersections(csc_biclique* csc_b, csr_biclique* csr_b); //bxb

#endif