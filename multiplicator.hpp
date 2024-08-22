#ifndef MULTIPLICATOR_HPP
#define MULTIPLICATOR_HPP

#include <matrix.hpp>
#include <biclique.hpp>

#include <cstdint>
#include <iostream>
#include <vector>
#include <queue>

namespace weighted
{
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



  struct Inters_Bicl {
    struct comparator
    {
      bool operator()(const std::pair<uint32_t,uint32_t>& a, const std::pair<uint32_t,uint32_t>& b) const {
        if (a.first != b.first) {
          // Ordenar por el primer índice de menor a mayor
          return a.first > b.first;
        } else {
          // En caso de empate, sumar los segundos índices
          return a.second > b.second;
        }
      }
    };

    std::vector<uint32_t>* S; // S 
    std::vector<std::pair<uint32_t,uint32_t>> C;

    
  };


  struct node_temp 
  {
    uint32_t id;
    uint32_t ady;
    uint32_t weight; 

    node_temp(uint32_t id, uint32_t ady, uint32_t weight) : id(id), ady(ady), weight(weight) {;}

    struct comp
    {
      bool operator()(const node_temp& a, const node_temp& b) const
      {
        if (a.id != b.id) {
          return a.id < b.id;
        } else {
          return a.ady < b.ady;
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
}


#endif