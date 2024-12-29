#include <multiplicator_boolean.hpp>

namespace boolean 
{

	Matrix* mat_mult(Matrix* A, Matrix* B)
	{
		assert(A != nullptr and B != nullptr);
		auto res_csr = mult(A->get_csc(), B->get_csr());
		auto res = new Matrix();
		res->set_csr(res_csr);
		return res;
	}

	Matrix* mat_pow(Matrix* A)
	{
		return mat_mult(A, A);
	}

	csr_matrix* csr_add(csr_matrix* A, csr_matrix* B)
	{
		return nullptr;
	}

	csr_matrix* mult(csc_matrix* csc, csr_matrix* csr)
	{
		//assert(csc != nullptr and csr !=nullptr);
    TIMERSTART(mult_local);
		auto c = compute_intersections(csc, csr);
    TIMERSTOP(mult_local);
    return c;
	}

	csr_matrix* compute_intersections(csc_matrix* csc, csr_matrix* csr)
	{
    TIMERSTART(get_int);
    PQ_Col Hr; 
    
    size_t dimX = 0;
    size_t dimY = 0;

		size_t i = 0;
		size_t j = 0;

    size_t csc_size = csc->col_id.size();
    size_t csr_size = csr->row_id.size();
		while (i < csc_size and j < csr_size) {
			if (csc->col_id[i] == csr->row_id[j]) {
				Hr.push(new Intersection(csc->col_ptr[i], csc->col_ptr[i+1], csr->row_ptr[j], csr->row_ptr[j+1], csc->row_ind[csc->col_ptr[i]], csr->col_ind[csr->row_ptr[j]]));
        dimY = (csc->col_ptr[i+1] - csc->col_ptr[i]) * (csr->row_ptr[j+1] - csr->row_ptr[j]);
        ++dimX;
				++i;
				++j;
			} else if (csc->col_id[i] > csr->row_id[j]) {
				++j;
			} else {
				++i;
			}
		}

    TIMERSTOP(get_int);
		//PQ_Row Hc;

    auto csr_res  = new csr_matrix(dimX, dimY);
    PQ_Row Hc; 

	  while (not Hr.empty()) {
		  auto* elem = Hr.top();
		  Hr.pop();
		  Hc.push(new Intersection(*elem));
		  if (Hr.empty() or (Hr.top()->value_col != elem->value_col)) { //primer elemento de la columna es distinto
        while (not Hc.empty()) {
          auto* inter = Hc.top();
          Hc.pop(); 
          if (Hc.empty() or inter->value_row != Hc.top()->value_row) { // si queda vacio o si el siguiente valor es distinto, push en csr
            csr_res->col_ind.push_back(csr->col_ind[inter->index_row]);
            if (csr_res->row_id.empty() or (csr_res->row_id.back() != csc->row_ind[inter->index_col])) {
              csr_res->row_id.push_back(csc->row_ind[inter->index_col]);
              csr_res->row_ptr.push_back(csr_res->col_ind.size()-1);
            }
          } 
      
          if (inter->index_row < inter->end_row - 1) {
            ++(inter->index_row);
            inter->value_row = csr->col_ind[inter->index_row];
            Hc.push(inter);
          } else {
            delete inter;
          }
        }
      }
		  if (elem->index_col < elem->end_col - 1) {
			  ++(elem->index_col);
			  elem->value_col = csc->row_ind[elem->index_col];
			  Hr.push(elem);
		  } else {
        delete elem;
      }
		}
		csr_res->row_ptr.push_back(csr_res->col_ind.size());
    //csr_res->print();
		return csr_res;  
	}

	//std::vector<Intersection>* get_intersections(csc_matrix* A, csr_matrix* B) //AxA
  PQ_Col get_intersections(csc_matrix* A, csr_matrix* B)
	{
		//auto inters = new std::vector<Intersection>();
    //auto Hr = new std::priority_queue<Intersection, std::vector<Intersection>, Intersection::Col_Comp>();
    PQ_Col Hr; 

		//assert(A != nullptr and B != nullptr);

		size_t i = 0;
		size_t j = 0;

		while (i < A->col_id.size() and j < B->row_id.size()) {
			if (A->col_id[i] == B->row_id[j]) {

				// Intersection inter;
				// inter.start_col = A->col_ptr[i];
				// inter.end_col = A->col_ptr[i+1];
				// inter.start_row = B->row_ptr[j];
				// inter.end_row =  B->row_ptr[j+1];
				// inter.value_col = A->row_ind[inter.start_col];
				// inter.value_row = B->col_ind[inter.start_row];
				Hr.push(new Intersection(A->col_ptr[i], A->col_ptr[i+1], B->row_ptr[j], B->row_ptr[j+1], A->row_ind[A->col_ptr[i]], B->col_ind[B->row_ptr[j]]));
				++i;
				++j;
			} else if (A->col_id[i] > B->row_id[j]) {
				++j;
			} else {
				++i;
			}
		}
    //return inters;
		return Hr;
	}

};