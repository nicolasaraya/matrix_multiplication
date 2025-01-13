#ifndef MULT_BOOLEAN
#define MULT_BOOLEAN

#include <matrix_boolean.hpp>
#include <biclique_boolean.hpp>

#include <cstdint>
#include <iostream>
#include <vector>
#include <queue>
#include <cstring>

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
    uint32_t index_col;
    uint32_t index_row;
    uint32_t index_bicl;

    Intersection() = default;

    Intersection(const Intersection& copy)
    {
      std::memcpy(this, &copy, sizeof(Intersection));
    }
    
    Intersection(uint32_t start_col, uint32_t end_col, uint32_t start_row, uint32_t end_row, uint32_t value_col, uint32_t value_row) :
        start_col(start_col),
        end_col(end_col),
        start_row(start_row),
        end_row(end_row),
        value_col(value_col),
        value_row(value_row),
        index_col(start_col),
        index_row(start_row),
        index_bicl(0)
    {;}

    Intersection(uint32_t start_col, uint32_t end_col, uint32_t start_row, uint32_t end_row, uint32_t value_col, uint32_t value_row, uint32_t index_bicl) :
        start_col(start_col),
        end_col(end_col),
        start_row(start_row),
        end_row(end_row),
        value_col(value_col),
        value_row(value_row),
        index_col(start_col),
        index_row(start_row),
        index_bicl(index_bicl)
    {;}

		// Definir el operador de comparación para la cola de prioridad
		struct Col_Comp 
		{
      bool operator()(const Intersection& a, const Intersection& b) const 
      {
				return a.value_col > b.value_col;
			}

			bool operator()(const Intersection* a, const Intersection* b) const 
      {
				return a->value_col > b->value_col;
			}
      
		};

		struct Row_Comp 
		{
			bool operator()(const Intersection& a, const Intersection& b) const 
      {
				return a.value_row > b.value_row;
			}
      bool operator()(const Intersection* a, const Intersection* b) const 
      {
				return a->value_row > b->value_row;
			}
		};
	};

  typedef std::priority_queue<Intersection*, std::vector<Intersection*>, Intersection::Col_Comp> PQ_Col;
  typedef std::priority_queue<Intersection*, std::vector<Intersection*>, Intersection::Row_Comp> PQ_Row;

  struct Inters_Bicl {
    // struct Bicl_Comp
    // {
    //   bool operator()(const uint32_t& a, const uint32_t& b) const {
    //     if (a != b) {
    //       return a > b;
    //     } else {
    //       return a > b;
    //     }
    //   }
    // };
    std::vector<uint32_t>* S; // S 
    std::vector<uint32_t> C;
  };

  // struct PQ_Row
  // {
  //   public:
  //     void emplace_back(Intersection& i)
  //     {
  //       elems.push_back(new Intersection(i));
  //       isHeap = false;
  //     }

  //     void make_heap()
  //     {
  //       std::make_heap(elems.begin(), elems.end(), Intersection::Row_Comp());
  //       isHeap = true;
  //     }

  //     Intersection* top()
  //     {
  //       if (not isHeap) {
  //         make_heap();
  //       }
  //       return elems.front();
  //     }

  //     void pop()
  //     {
  //       if (not isHeap) {
  //         make_heap();
  //       }
  //       std::pop_heap(elems.begin(), elems.end(), Intersection::Row_Comp());
  //       elems.pop_back();
  //     }

  //     void push(Intersection* i)
  //     {
  //       if (not isHeap) {
  //         make_heap();
  //       }
  //       elems.push_back(i);
  //       std::push_heap(elems.begin(), elems.end(), Intersection::Row_Comp());
  //     }
  //     bool empty()
  //     {
  //       return elems.empty();
  //     }

  //   private:
  //     std::vector<Intersection*> elems;
  //     bool isHeap = false;
  // };

	Matrix* mat_mult(Matrix* A, Matrix* B);
  Matrix* mat_mult(Matrix* A, Biclique* Ab, Matrix* B, Biclique* Bb);

	Matrix* mat_pow(Matrix* A);
  Matrix* mat_pow(Matrix* A, Biclique* b);

	csr_matrix* mult(csc_matrix* A_csc, csr_matrix* B_csr);     //AxA
  csr_matrix* mult(csc_matrix* A_csc, Biclique* b);   //Axb
  csr_matrix* mult(Biclique* b, csr_matrix* A_csr);   //bxA
  csr_matrix* mult(Biclique* a, Biclique* b); //bxb

  csr_matrix* compute_intersections(csc_matrix* A_csc, csr_matrix* B_csr); //AxA
  csr_matrix* compute_intersections(csc_matrix* A_csc, Biclique* b);
  csr_matrix* compute_intersections(Biclique* b, csr_matrix* A_csr); //AxA
  csr_matrix* compute_intersections(Biclique* a, Biclique* b);

	csr_matrix* csr_add(csr_matrix* A, csr_matrix* B);
};

#endif