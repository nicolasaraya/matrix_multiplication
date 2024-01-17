#include "multiplicator.hpp"

Matrix* mat_mult(Matrix *A, Matrix* B)
{
    auto csc = A->get_csc();
    auto csr = B->get_csr();
    assert(csc != nullptr and csr != nullptr);

    auto csr_res = new csr_matrix;

    auto inters = get_intersections(A, B);

    std::priority_queue<Intersection, std::vector<Intersection>, Intersection::Col_Comp> Hr;
    std::priority_queue<Intersection, std::vector<Intersection>, Intersection::Row_Comp> Hc;


    for (auto i : inters) {
        //std::cout << i.index_col << " " << i.index_row << " | " << i.start_col << " " << i.end_col << " | " << i.start_row << " " << i.end_row << " | " << i.value_col << " " << i.value_row << std::endl;
        Hr.push(i);
    }


    while (not Hr.empty()) {
        auto elem = Hr.top();
        Hr.pop();
        Hc.push(elem);
        //std::cout  << "pop Hr " << elem.index_col << " " << elem.index_row << " | " << elem.start_col << " " << elem.end_col << " | " << elem.start_row << " " << elem.end_row << " | " << elem.value_col << " " << elem.value_row << std::endl;
                
        if (Hr.empty() or (Hr.top().value_col != elem.value_col)) { //primer elemento de la columna es distinto
            uint32_t sum = 0;
            while (not Hc.empty()) {
                auto inter = Hc.top();
                Hc.pop(); 
                //std::cout << "pop Hc " << "inter: " << inter.index_col << " " << inter.index_row << " | " << inter.start_col << " " << inter.end_col << " | " << inter.start_row << " " << inter.end_row << " | " << inter.value_col << " " << inter.value_row << std::endl;
                sum += csc->values[inter.start_col] * csr->values[inter.start_row];

                if (Hc.empty() or inter.value_row != Hc.top().value_row) { // si queda vacio o si el siguiente valor es distinto, push en csr
                    csr_res->values.push_back(sum);
                    csr_res->col_ind.push_back(csr->col_ind[inter.start_row]);

                    if (csr_res->row_id.empty() or (csr_res->row_id.back() != csc->row_ind[inter.start_col])) {
                        csr_res->row_id.push_back(csc->row_ind[inter.start_col]);
                        csr_res->row_ptr.push_back(csr_res->col_ind.size()-1);
                    }

                    //std::cout <<"(" << csc->row_ind[inter.start_col] << ", " << csr->col_ind[inter.start_row] << ") = " << sum << std::endl;
                    sum = 0;
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
    csr_res->row_ptr.push_back(csr_res->values.size());
    Matrix* res = new Matrix();
    res->set_csr(csr_res);
    return res;
}

Matrix* mat_pow(Matrix* A) 
{
    return mat_mult(A, A);
}

std::vector<Intersection> get_intersections(Matrix* A, Matrix* B)
{
    std::vector<Intersection> inters;

    auto csc = A->get_csc();
    auto csr = B->get_csr();

    size_t i = 0;
    size_t j = 0;

    while (i < csc->col_id.size() and j < csr->row_id.size()) {
        if (csc->col_id[i] == csr->row_id[j]) {
            uint32_t start_col = csc->col_ptr[i];
            uint32_t end_col = csc->col_ptr[i+1];

            uint32_t start_row = csr->row_ptr[j];
            uint32_t end_row = csr->row_ptr[j+1];
            //else 

            inters.push_back(Intersection(i,j, start_col, end_col, start_row, end_row, csc->row_ind[start_col], csr->col_ind[start_row]));
            i++;
            j++;
        } else if (csc->col_id[i] > csr->row_id[j]) {
            j++;
        } else {
            i++;
        }
    }
    return inters;
}