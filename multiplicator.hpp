#ifndef MULTIPLICATOR_HPP
#define MULTIPLICATOR_HPP

#include <matrix.hpp>

#include <cstdint>
#include <iostream>
#include <vector>
#include <queue>


struct Intersection
{
    uint32_t index_col;
    uint32_t index_row;
    uint32_t start_col;
    uint32_t end_col;
    uint32_t start_row;
    uint32_t end_row;
    uint32_t value_col;
    uint32_t value_row; 


    // Constructor
    Intersection(uint32_t val, uint32_t val2, uint32_t start_col, uint32_t end_col, uint32_t start_row, uint32_t end_row, uint32_t value_col, uint32_t value_row) 
                                                                                                      : index_col(val), 
                                                                                                        index_row(val2), 
                                                                                                        start_col(start_col),
                                                                                                        end_col(end_col),
                                                                                                        start_row(start_row),
                                                                                                        end_row(end_row),
                                                                                                        value_col(value_col), 
                                                                                                        value_row(value_row) {}

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

std::vector<Intersection> get_intersections(Matrix* A, Matrix* B);

#endif