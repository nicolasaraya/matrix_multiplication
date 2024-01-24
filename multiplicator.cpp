#include "multiplicator.hpp"


Matrix* mat_mult(Matrix* A, Matrix* B) 
{
    auto res_csr = csr_mult(A->get_csc(), B->get_csr());
    auto res = new Matrix();
    res->set_csr(res_csr);
    return res;
}

Matrix* mat_mult(Matrix* A, Biclique* Ab, Matrix* B, Biclique* Bb) 
{
    auto resAxB = csr_mult(A->get_csc(), B->get_csr()); //AxB
    auto resAxBb = csr_mult(A->get_csc(), Bb->get_csr()); //A X Bb
    
    auto add = csr_add(resAxB, resAxBb);
    delete resAxB;
    delete resAxBb;

    auto resAbxA = csr_mult(Ab->get_csc(), B->get_csr()); //Ab x B
    auto resAbxBb = csr_mult(Ab->get_csc(), Bb->get_csr()); //Ab X Bb
    auto add2 = csr_add(resAbxA, resAbxBb);
    delete resAbxA;
    delete resAbxBb;

    auto add3 = csr_add(add, add2);

    delete add;
    delete add2;

    auto res = new Matrix();
    res->set_csr(add3);

    return res;
}

Matrix* mat_pow(Matrix* A) 
{
    return mat_mult(A, A);

}

Matrix* mat_pow(Matrix* A, Biclique* b) 
{
    return mat_mult(A, b, A, b);

    /*
    auto resAxA = csr_mult(A->get_csc(), A->get_csr());
    auto resAxB = csr_mult(A->get_csc(), b->get_csr()); //A X B 
    
    auto add = csr_add(resAxA, resAxB);
    delete resAxA;
    delete resAxB;

    auto resBxA = csr_mult(b->get_csc(), A->get_csr()); //B X A 
    auto resBxB = csr_mult(b->get_csc(), b->get_csr()); //B X B
    auto add2 = csr_add(resBxA, resBxB);
    delete resBxA;
    delete resBxB;

    auto add3 = csr_add(add, add2);

    delete add;
    delete add2;

    auto res = new Matrix();
    res->set_csr(add3);
    
    return res;
    */
}


csr_matrix* csr_mult(csc_matrix* csc, csr_matrix* csr)
{
    assert(csc != nullptr and csr != nullptr);
    auto inters = get_intersections(csc, csr);
    return compute_intersections(inters, csc, csr);
}

csr_matrix* csr_mult(csc_matrix* csc, csr_biclique* csr) 
{
    assert(csc != nullptr and csr != nullptr);
    auto inters = get_intersections(csc, csr);
    return compute_intersections(inters, csc, csr);
}

csr_matrix* csr_add(csr_matrix* A, csr_matrix* B)
{

    csr_matrix* res = new csr_matrix();
    res->row_ptr.push_back(0);

    uint32_t i = 0; 
    uint32_t j = 0; 

    while (i < A->row_id.size() and j < B->row_id.size()) {
        if (A->row_id[i] == B->row_id[j]) {
            size_t startA = A->row_ptr[i];
            size_t stopA = A->row_ptr[i+1];
            size_t startB = B->row_ptr[j];
            size_t stopB = B->row_ptr[j+1];

            while (startA < stopA and startB < stopB) {
                if (A->col_ind[startA] == B->col_ind[startB]) {
                    res->col_ind.push_back(A->col_ind[startA]);
                    res->values.push_back(A->values[startA]+B->values[startB]);
                    startA++;
                    startB++;
                } else if (A->col_ind[startA] < B->col_ind[startB]) {
                    res->col_ind.push_back(A->col_ind[startA]);
                    res->values.push_back(A->values[startA]);
                    startA++;
                } else {
                    res->col_ind.push_back(B->col_ind[startB]);
                    res->values.push_back(B->values[startB]);
                    startB++;
                }
            }

            while (startA < stopA) {
                res->col_ind.push_back(A->col_ind[startA]);
                res->values.push_back(A->values[startA]);
                startA++;
            }
            while (startB < stopB) {
                res->col_ind.push_back(B->col_ind[startB]);
                res->values.push_back(B->values[startB]);
                startB++;
            }

            res->row_id.push_back(B->row_id[j]);
            res->row_ptr.push_back(res->col_ind.size());
            
            i++;
            j++;
        } else if (A->row_id[i] < B->row_id[j]) {
            size_t start = A->row_ptr[i];
            size_t stop = A->row_ptr[i+1];

            while (start < stop) {
                res->values.push_back(A->values[start]);
                res->col_ind.push_back(A->col_ind[start]);
                start++;
            }

            res->row_id.push_back(A->row_id[i]);
            res->row_ptr.push_back(res->col_ind.size());
            i++;

        } else {
            size_t start = B->row_ptr[j];
            size_t stop = B->row_ptr[j+1];

            while (start < stop) {
                res->values.push_back(B->values[start]);
                res->col_ind.push_back(B->col_ind[start]);
                start++;
            }

            res->row_id.push_back(B->row_id[j]);
            res->row_ptr.push_back(res->col_ind.size());
            j++;
        }

    }

    while (i < A->row_id.size()) {
        size_t start = A->row_ptr[i];
        size_t stop = A->row_ptr[i+1];

        while (start < stop) {
            res->values.push_back(A->values[start]);
            res->col_ind.push_back(A->col_ind[start]);
            start++;
        }
        res->row_id.push_back(A->row_id[i]);
        res->row_ptr.push_back(res->col_ind.size());
        i++;

    }
    while (j < B->row_id.size()) {
        size_t start = B->row_ptr[j];
        size_t stop = B->row_ptr[j+1];

        while (start < stop) {
            res->values.push_back(B->values[start]);
            res->col_ind.push_back(B->col_ind[start]);
            start++;
        }

        res->row_id.push_back(B->row_id[j]);
        res->row_ptr.push_back(res->col_ind.size());
        j++;

    }

       
    return res;

}

template<typename csr_type>
csr_matrix* compute_intersections(std::vector<Intersection>* inters, csc_matrix* csc, csr_type* csr) 
{
    assert(csc != nullptr and csr != nullptr);

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
    return csr_res;    

}



std::vector<Intersection>* get_intersections(csc_matrix* csc, csr_matrix* csr)
{
    auto inters = new std::vector<Intersection>();

    assert(csc != nullptr and csr != nullptr);

    size_t i = 0;
    size_t j = 0;

    while (i < csc->col_id.size() and j < csr->row_id.size()) {
        if (csc->col_id[i] == csr->row_id[j]) {
            uint32_t start_col = csc->col_ptr[i];
            uint32_t end_col = csc->col_ptr[i+1];

            uint32_t start_row = csr->row_ptr[j];
            uint32_t end_row = csr->row_ptr[j+1];

            inters->push_back(Intersection(start_col, end_col, start_row, end_row, csc->row_ind[start_col], csr->col_ind[start_row]));
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



std::vector<Intersection>* get_intersections(csc_matrix* csc, csr_biclique* csr) 
{
    auto inters = new std::vector<Intersection>();

    assert(csc != nullptr and csr != nullptr);

    size_t i = 0;
    size_t j = 0;

    while (i < csc->col_id.size() and j < csr->row_id_2.size()) {
        if (csc->col_id[i] == csr->row_id_2[j].first) {
            uint32_t start_col = csc->col_ptr[i];
            uint32_t end_col = csc->col_ptr[i+1];

            for (size_t k = 0; k < csr->row_id_2[j].second.size(); k++) {
                uint32_t indexRow = csr->row_id_2[j].second.at(k);
                uint32_t start_row = csr->row_ptr[indexRow];
                uint32_t end_row = csr->row_ptr[indexRow+1];
                inters->push_back(Intersection(start_col, end_col, start_row, end_row, csc->row_ind[start_col], csr->col_ind[start_row]));
            }

            i++;
            j++;
        } else if (csc->col_id[i] > csr->row_id_2[j].first) {
            j++;
        } else {
            i++;
        }
    }
    return inters;
}