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
		assert(csc != nullptr and csr !=nullptr);
		auto inters = get_intersections(csc, csr);
		return compute_intersections(inters, csc, csr);
	}

	csr_matrix* compute_intersections(std::vector<Intersection>* inters, csc_matrix* csc, csr_matrix* csr)
	{
		auto csr_res = new csr_matrix;

		std::priority_queue<Intersection, std::vector<Intersection>, Intersection::Col_Comp> Hr;
		std::priority_queue<Intersection, std::vector<Intersection>, Intersection::Row_Comp> Hc;


		for (auto i : *inters) {
			//std::cout << i.index_col << " " << i.index_row << " | " << i.start_col << " " << i.end_col << " | " << i.start_row << " " << i.end_row << " | " << i.value_col << " " << i.value_row << std::endl;
			Hr.push(i);
		}

		delete inters;



	while (not Hr.empty()) {
		auto elem = Hr.top();
		Hr.pop();
		Hc.push(elem);
		//std::cout  << "pop Hr " << elem.index_col << " " << elem.index_row << " | " << elem.start_col << " " << elem.end_col << " | " << elem.start_row << " " << elem.end_row << " | " << elem.value_col << " " << elem.value_row << std::endl;
				
		if (Hr.empty() or (Hr.top().value_col != elem.value_col)) { //primer elemento de la columna es distinto
			//uint32_t sum = 0;

			while (not Hc.empty()) {
				auto inter = Hc.top();
				Hc.pop(); 
				//std::cout << "pop Hc " << "inter: " << inter.index_col << " " << inter.index_row << " | " << inter.start_col << " " << inter.end_col << " | " << inter.start_row << " " << inter.end_row << " | " << inter.value_col << " " << inter.value_row << std::endl;
				//sum += csc->values[inter.start_col] * csr->values[inter.start_row];

				if (Hc.empty() or inter.value_row != Hc.top().value_row) { // si queda vacio o si el siguiente valor es distinto, push en csr
					//csr_res->values.push_back(sum);
					csr_res->col_ind.push_back(csr->col_ind[inter.start_row]);

					if (csr_res->row_id.empty() or (csr_res->row_id.back() != csc->row_ind[inter.start_col])) {
						csr_res->row_id.push_back(csc->row_ind[inter.start_col]);
						csr_res->row_ptr.push_back(csr_res->col_ind.size()-1);
					}

					//std::cout <<"(" << csc->row_ind[inter.start_col] << ", " << csr->col_ind[inter.start_row] << ") = " << sum << std::endl;
					//sum = 0;
				} 
				
				if (inter.start_row < inter.end_row - 1) {
					inter.start_row++;
					inter.value_row = csr->col_ind[inter.start_row];
					//std::cout << "inter modified:" << inter.index_col << " " << inter.index_row << " | " << inter.start_col << " " << inter.end_col << " | " << inter.start_row << " " << inter.end_row << " | " << inter.value_col << " " << inter.value_row << std::endl;
					Hc.push(inter);
				} 
				
			}
		}

		if (elem.start_col < elem.end_col - 1) {
			elem.start_col++;
			elem.value_col = csc->row_ind[elem.start_col];
			Hr.push(elem);
		} 

		//std::cout << "new iter" << std::endl;
			
		}
		csr_res->row_ptr.push_back(csr_res->col_ind.size());
		return csr_res;  
	}

	std::vector<Intersection>* get_intersections(csc_matrix* A, csr_matrix* B) //AxA
	{
		auto inters = new std::vector<Intersection>();

		assert(A != nullptr and B != nullptr);

		size_t i = 0;
		size_t j = 0;

		while (i < A->col_id.size() and j < B->row_id.size()) {
			if (A->col_id[i] == B->row_id[j]) {
				Intersection inter;
				inter.start_col = A->col_ptr[i];
				inter.end_col = A->col_ptr[i+1];
				inter.start_row = B->row_ptr[j];
				inter.end_row =  B->row_ptr[j+1];
				inter.value_col = A->row_ind[inter.start_col];
				inter.value_row = B->col_ind[inter.start_row];
				inters->push_back(inter);
				i++;
				j++;
			} else if (A->col_id[i] > B->row_id[j]) {
				j++;
			} else {
				i++;
			}
		}
		return inters;
	}

};