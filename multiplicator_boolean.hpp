#ifndef MULT_BOOLEAN
#define MULT_BOOLEAN

#include "matrix_boolean.hpp"
#include "biclique_boolean.hpp"

#include <cstdint>
#include <iostream>
#include <vector>
#include <queue>
#include <cstring>
#include <functional>

struct Intersection
{
  size_t start_col;
  size_t end_col;
  size_t start_row;
  size_t end_row;
  uint32_t value_col;
  uint32_t value_row;
  uint32_t index_bicl;

  Intersection() = default;
  Intersection(const Intersection& copy) { std::memcpy(this, &copy, sizeof(Intersection)); }
  
  Intersection(size_t start_col, size_t end_col, size_t start_row, size_t end_row, uint32_t value_col, uint32_t value_row) :
      start_col(start_col),
      end_col(end_col),
      start_row(start_row),
      end_row(end_row),
      value_col(value_col),
      value_row(value_row),
      index_bicl(0)
  {;}

  Intersection(size_t start_col, size_t end_col, size_t start_row, size_t end_row, uint32_t value_col, uint32_t value_row, uint32_t index_bicl) :
      start_col(start_col),
      end_col(end_col),
      start_row(start_row),
      end_row(end_row),
      value_col(value_col),
      value_row(value_row),
      index_bicl(index_bicl)
  {;}

  struct Col_Comp 
  {
    bool operator()(const Intersection* a, const Intersection* b) const 
    {
      return a->value_col > b->value_col;
    }
  };

  struct Row_Comp 
  {
    bool operator()(Intersection* a, Intersection* b) const 
    {
      return a->value_row > b->value_row;
    }
  };

  struct Row_Comp_ref 
  {
    bool operator()(Intersection& a, Intersection& b) const 
    {
      return a.value_row > b.value_row;
    }
  };
};

struct Inters_Bicl
{
  std::vector<uint32_t> S; 
  std::vector<uint32_t> C;
};

typedef std::priority_queue<Intersection*, std::vector<Intersection*>, Intersection::Col_Comp> PQ_Col;
typedef std::priority_queue<Intersection*, std::vector<Intersection*>, Intersection::Row_Comp> PQ_Row;

void powBicl(Matrix* matrix, Biclique* biclique);
void powBicl(Matrix* matrix, Biclique* biclique, Matrix*& outMatrix, Biclique*& outBiclique);
void pow(Matrix* matrix);
void multiply(Matrix* A, Matrix* B);
void multiply(Matrix* A, Biclique* aB, Matrix* B, Biclique* bB);
void multiply(Matrix* A, Biclique* aB, Matrix* B, Biclique* bB, Matrix*& outMatrix, Biclique*& outBiclique);

csr_matrix* mult(csc_matrix* A_csc, csr_matrix* B_csr);     //AxA
csr_matrix* mult(csc_matrix* A_csc, Biclique* b); //Axb
csr_matrix* mult(Biclique* b, csr_matrix* A_csr); //bxA
csr_matrix* mult(Biclique* a, Biclique* b); //bxb

std::vector<Inters_Bicl>* compute_intersections(Biclique* b, csr_matrix* A_csr); //AxA
std::vector<Inters_Bicl>* compute_intersections(Biclique* a, Biclique* b);

csr_matrix* csrFromIntersBicl(Biclique* b, std::vector<Inters_Bicl>* intersections);
Biclique* bicliqueFromIntersBicl(Biclique* b, std::vector<Inters_Bicl>* intersections);
csr_matrix* csr_add(csr_matrix* A, csr_matrix* B);
Biclique* biclique_add(Biclique* a, Biclique* b, std::vector<Inters_Bicl>* interA, std::vector<Inters_Bicl>* interB);

#endif