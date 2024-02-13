#include "multiplicator.hpp"


Matrix* mat_mult(Matrix* A, Matrix* B) 
{
    assert(A != nullptr and B != nullptr);
    auto res_csr = mult(A->get_csc(), B->get_csr());
    auto res = new Matrix();
    res->set_csr(res_csr);
    return res;
}

Matrix* mat_mult(Matrix* A, Biclique* Ab, Matrix* B, Biclique* Bb) 
{
    TIMERSTART(AXA);
    auto resAxB = mult(A->get_csc(), B->get_csr()); //AxB
    //resAxB->print();
    TIMERSTOP(AXA);
    
    TIMERSTART(AXB);
    auto resAxBb = mult(A->get_csc(), Bb); //A X Bb
    //resAxBb->print();
    TIMERSTOP(AXB);
    
    TIMERSTART(APPEND);
    auto add = csr_add(resAxB, resAxBb);
    TIMERSTOP(APPEND);

    delete resAxB;
    delete resAxBb;

    TIMERSTART(BXA);
    auto resAbxB = mult(Ab, B->get_csr());
    //resAbxB->print();
    TIMERSTOP(BXA);
    
    TIMERSTART(BXB);
    auto resAbxBb = mult(Ab, Bb); //Ab X Bb
    //resAbxBb->print();
    TIMERSTOP(BXB);

    TIMERSTART(APPEND2);
    auto add2 = csr_add(resAbxB, resAbxBb);
    TIMERSTOP(APPEND2);
    delete resAbxB;
    delete resAbxBb;
    
    TIMERSTART(APPEND3);
    auto add3 = csr_add(add, add2);
    TIMERSTOP(APPEND3);
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
}


csr_matrix* mult(csc_matrix* A, csr_matrix* B)  //AxA
{
    assert(A != nullptr and B != nullptr);

    auto csr_res = new csr_matrix;

    std::priority_queue<Intersection, std::vector<Intersection>, Intersection::Col_Comp> Hr;
    std::priority_queue<Intersection, std::vector<Intersection>, Intersection::Row_Comp> Hc;

    //auto inters = new std::vector<Intersection>();

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
            Hr.push(inter);
            i++;
            j++;
        } else if (A->col_id[i] > B->row_id[j]) {
            j++;
        } else {
            i++;
        }
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
                sum += A->values[inter.start_col] * B->values[inter.start_row];

                if (Hc.empty() or inter.value_row != Hc.top().value_row) { // si queda vacio o si el siguiente valor es distinto, push en csr
                    csr_res->values.push_back(sum);
                    csr_res->col_ind.push_back(B->col_ind[inter.start_row]);

                    if (csr_res->row_id.empty() or (csr_res->row_id.back() != A->row_ind[inter.start_col])) {
                        csr_res->row_id.push_back(A->row_ind[inter.start_col]);
                        csr_res->row_ptr.push_back(csr_res->col_ind.size()-1);
                    }

                    //std::cout <<"(" << csc->row_ind[inter.start_col] << ", " << csr->col_ind[inter.start_row] << ") = " << sum << std::endl;
                    sum = 0;
                } 
                
                if (inter.start_row < inter.end_row - 1) {
                    inter.start_row++;
                    inter.value_row = B->col_ind[inter.start_row];
                    //std::cout << "inter modified:" << inter.index_col << " " << inter.index_row << " | " << inter.start_col << " " << inter.end_col << " | " << inter.start_row << " " << inter.end_row << " | " << inter.value_col << " " << inter.value_row << std::endl;
                    Hc.push(inter);
                } 
                
            }
        }

        if (elem.start_col < elem.end_col - 1) {
            elem.start_col++;
            elem.value_col = A->row_ind[elem.start_col];
            Hr.push(elem);
        } 

        //std::cout << "new iter" << std::endl;
        
    }
    csr_res->row_ptr.push_back(csr_res->values.size());
    //std::cout << "edges computed: " << csr_res->values.size() << std::endl;
    return csr_res;    
}

csr_matrix* mult(csc_matrix* A, Biclique* b)
{
    assert(A != nullptr and b != nullptr);

    auto b_csr = b->get_csr();
    auto csr_res = new csr_matrix();

    std::vector<uint32_t> index(A->col_id.back()+1, UINT32_MAX);
    for (size_t i = 0; i < A->col_id.size(); i++) {
        index[A->col_id[i]] = i;
        //std::cout << A->col_id[i] << ": " << i << std::endl;
    }

    std::priority_queue<Intersection, std::vector<Intersection>, Intersection::Col_Comp> Hr;
    std::priority_queue<Intersection, std::vector<Intersection>, Intersection::Row_Comp> Hc;

    for (size_t i = 0; i < b_csr->size(); i++) {
        auto csr = b_csr->at(i);
        for (size_t j = 0; j < csr->row_id.size(); j++) {
            if (index[csr->row_id[j]] != UINT32_MAX) {
                Intersection inter;
                inter.start_col = A->col_ptr[index[csr->row_id[j]]];
                inter.end_col = A->col_ptr[index[csr->row_id[j]] + 1];
                inter.start_row = 0;
                inter.end_row = csr->col_ind.size();
                inter.value_col = A->row_ind[inter.start_col];
                inter.value_row = csr->col_ind[inter.start_row];
                inter.index_bicl = i;
                Hr.push(inter);
            }
        }
    }

    //std::cout << "inters: " << Hr.size() << std::endl;
    //uint32_t count = 0;

    while (not Hr.empty()) {
        auto elem = Hr.top();
        Hr.pop();
        Hc.push(elem);
        //std::cout  << "pop Hr " << elem.index_col << " " << elem.index_row << " | " << elem.start_col << " " << elem.end_col << " | " << elem.start_row << " " << elem.end_row << " | " << elem.value_col << " " << elem.value_row << std::endl;
                
        if (Hr.empty() or (Hr.top().value_col != elem.value_col)) { //primer elemento de la columna es distinto
            uint32_t sum = 0;
            //count++;
            while (not Hc.empty()) {
                auto inter = Hc.top();
                Hc.pop(); 
                //std::cout << "pop Hc " << "inter: " << inter.index_col << " " << inter.index_row << " | " << inter.start_col << " " << inter.end_col << " | " << inter.start_row << " " << inter.end_row << " | " << inter.value_col << " " << inter.value_row << std::endl;
                sum += A->values[inter.start_col] * b_csr->at(inter.index_bicl)->values[inter.start_row];

                if (Hc.empty() or inter.value_row != Hc.top().value_row) { // si queda vacio o si el siguiente valor es distinto, push en csr
                    csr_res->values.push_back(sum);
                    csr_res->col_ind.push_back(b_csr->at(inter.index_bicl)->col_ind[inter.start_row]);

                    if (csr_res->row_id.empty() or (csr_res->row_id.back() != A->row_ind[inter.start_col])) {
                        csr_res->row_id.push_back(A->row_ind[inter.start_col]);
                        csr_res->row_ptr.push_back(csr_res->col_ind.size()-1);
                    }

                    //std::cout <<"(" << csc->row_ind[inter.start_col] << ", " << csr->col_ind[inter.start_row] << ") = " << sum << std::endl;
                    sum = 0;
                } 
                
                if (inter.start_row < inter.end_row - 1) {
                    inter.start_row++;
                    inter.value_row = b_csr->at(inter.index_bicl)->col_ind[inter.start_row];
                    //std::cout << "inter modified:" << inter.index_col << " " << inter.index_row << " | " << inter.start_col << " " << inter.end_col << " | " << inter.start_row << " " << inter.end_row << " | " << inter.value_col << " " << inter.value_row << std::endl;
                    Hc.push(inter);
                } 
                
            }
        }

        if (elem.start_col < elem.end_col - 1) {
            elem.start_col++;
            elem.value_col = A->row_ind[elem.start_col];
            Hr.push(elem);
        } 

        //std::cout << "new iter" << std::endl;
        
    }
    //std::cout << "count: " <<  count << std::endl;
    csr_res->row_ptr.push_back(csr_res->values.size());
    //std::cout << "edges computed: " << csr_res->values.size() << std::endl;
    return csr_res;   

}

csr_matrix* mult(Biclique* b, csr_matrix* A) //bxA
{
    
    assert(A != nullptr and b != nullptr);
    auto b_csc = b->get_csc();
    auto b_marks = b->get_marks();

    std::vector<uint32_t> index(b->maxDim() + 1, UINT32_MAX);
    for (size_t i = 0; i < A->row_id.size(); i++) {
        if (A->row_id[i] > b->maxDim()) break;
        index[A->row_id[i]] = i;
    }

    std::vector<Inters_Bicl> intersections; 
    
    for (size_t i = 0; i < b_csc->size(); i++) { 
        //std::priority_queue<std::pair<uint32_t, uint32_t>, std::vector<std::pair<uint32_t, uint32_t>>, decltype(min_heap_comp)> heap(min_heap_comp);
        auto S_i = &(b_csc->at(i)->col_id);
        auto C_i = &(b_csc->at(i)->row_ind);
        auto Cv_i = &(b_csc->at(i)->values);

        Inters_Bicl p;
        p.S = C_i;
        //std::priority_queue<std::pair<uint32_t, uint32_t>, std::vector<std::pair<uint32_t, uint32_t>>, Inters_Bicl::comparator> C_temp;
        std::vector<std::pair<uint32_t, uint32_t>> C_temp;

        size_t count = 0;

        for (size_t j = 0; j < S_i->size(); j++) { 
            size_t ind = S_i->at(j);
            if (index[ind] != UINT32_MAX) {
                size_t start_row = A->row_ptr[index[ind]];
                size_t end_row = A->row_ptr[index[ind] + 1];
                for (size_t k = start_row; k < end_row; k++) {
                    C_temp.push_back(std::pair(A->col_ind[k], A->values[k] * Cv_i->at(j)));
                }
                count++;
            }        
        }

        if (count > 1){
            std::sort(C_temp.begin(), C_temp.end());
            for (auto k : C_temp) {
                if (p.C.empty() or p.C.back().first != k.first) {
                    p.C.push_back(k);
                } else {
                    p.C.back().second += k.second;
                }
            }
        } else {
            p.C = C_temp;
        }
        intersections.push_back(p);

    }
    index.clear();

    auto csr_res = new csr_matrix();

    for (auto i : *b_marks) {
        csr_res->row_ptr.push_back(csr_res->col_ind.size());
        csr_res->row_id.push_back(i.first);

        std::vector<std::pair<uint32_t,uint32_t>> C_temp;

        for (auto j : i.second) {
            for (auto k : intersections.at(j).C) {
                C_temp.push_back(k);
            }
        }

        if (i.second.size() > 0) std::sort(C_temp.begin(), C_temp.end());

        for (auto j : C_temp) {
            if (csr_res->col_ind.empty() or csr_res->col_ind.size() == csr_res->row_ptr.back() or csr_res->col_ind.back() != j.first) {
                csr_res->col_ind.push_back(j.first);
                csr_res->values.push_back(j.second);
            } else {
                csr_res->values.back() += j.second;
            }
        }

    }



    csr_res->row_ptr.push_back(csr_res->col_ind.size()); 
    //csr_res->print();
    return csr_res;
}

csr_matrix* mult(Biclique* a, Biclique* b) //bxb
{
    
    assert(a != nullptr and b != nullptr);
    auto a_csc = a->get_csc();
    auto b_csr = b->get_csr();
    auto a_marks = a->get_marks();

    std::vector<Inters_Bicl> intersections; 

    for (size_t i = 0; i < a_csc->size(); i++) {
        Inters_Bicl p;
        std::vector<std::pair<uint32_t,uint32_t>> C_temp;
        p.S= &(a_csc->at(i)->col_id);
        size_t count = 0;
        for (size_t j = 0; j < a_csc->at(i)->col_id.size(); j++) {
            auto index_to_inter = b->get_indexes(a_csc->at(i)->col_id.at(j));
            if (index_to_inter == nullptr)  continue;
            for (auto index_b : (*index_to_inter)){
                for (size_t k = 0; k < b_csr->at(index_b)->col_ind.size(); k++) {
                    C_temp.push_back(std::pair(b_csr->at(index_b)->col_ind.at(k), b_csr->at(index_b)->values.at(k) * a_csc->at(i)->values.at(j)));
                }
            }
            count++;
        }
        if (count > 1) {
            std::sort(C_temp.begin(), C_temp.end());
            for (auto k : C_temp) {
                //std::cout << C_temp.top().first << "," << C_temp.top().second << std::endl;
                if (p.C.empty() or p.C.back().first != k.first) {
                    p.C.push_back(k);
                } else {
                    p.C.back().second += k.second;
                }
            }
        } else {
            p.C = C_temp;
        }

        intersections.push_back(p);
    }


    auto csr_res = new csr_matrix();

    for (auto i : *a_marks) {
        csr_res->row_ptr.push_back(csr_res->col_ind.size());
        csr_res->row_id.push_back(i.first);

        std::vector<std::pair<uint32_t,uint32_t>> C_temp;

        for (auto j : i.second) {
            for (auto k : intersections.at(j).C) {
                C_temp.push_back(k);
            }
        }
        if (i.second.size() > 0)  std::sort(C_temp.begin(), C_temp.end());

        for (auto j : C_temp) {
            if (csr_res->col_ind.empty() or csr_res->col_ind.size() == csr_res->row_ptr.back() or csr_res->col_ind.back() != j.first) {
                csr_res->col_ind.push_back(j.first);
                csr_res->values.push_back(j.second);
            } else {
                csr_res->values.back() += j.second;
            }
        }
    }



    csr_res->row_ptr.push_back(csr_res->col_ind.size()); 
    //csr_res->print();
    return csr_res;
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




