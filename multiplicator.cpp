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

    auto csr_res = new csr_matrix();
    //csr_res->row_ptr.push_back(0);

    std::vector<uint32_t> index(b->maxDim() + 1, UINT32_MAX);
    for (size_t i = 0; i < A->row_id.size(); i++) {
        if (A->row_id[i] > b->maxDim()) break;
        index[A->row_id[i]] = i;
    }

    //std::vector<Res_bic> res;
    std::priority_queue<Inters_Bicl, std::vector<Inters_Bicl>, Inters_Bicl::comp_row> Hr;
    std::priority_queue<Inters_Bicl, std::vector<Inters_Bicl>, Inters_Bicl::comp_col> Hc;

    
    for (size_t i = 0; i < b_csc->size(); i++) { 
        //std::priority_queue<std::pair<uint32_t, uint32_t>, std::vector<std::pair<uint32_t, uint32_t>>, decltype(min_heap_comp)> heap(min_heap_comp);
        auto S_i = &(b_csc->at(i)->col_id);
        auto C_i = &(b_csc->at(i)->row_ind);
        auto Cv_i = &(b_csc->at(i)->values);

        for (size_t j = 0; j < S_i->size(); j++) { 
            size_t ind = S_i->at(j);
            if (index[ind] != UINT32_MAX) {
                size_t start_row = A->row_ptr[index[ind]];
                size_t end_row = A->row_ptr[index[ind] + 1];
                //std::cout << "find: " << ind  << ", "<< index[ind] <<  ", startrow: " << start_row << ", endrow: " << end_row <<std::endl;
                Inters_Bicl p;
                p.row_id = C_i;
                for (size_t k = start_row; k < end_row; k++) {
                    //heap.push(std::pair(A->col_ind[k], A->values[k] * Cv_i->at(j)));
                    //p.C->push_back(std::pair(A->col_ind[k], A->values[k] * Cv_i->at(j)));
                    p.col_ind.push_back(A->col_ind[k]);
                    p.values.push_back(A->values[k] * Cv_i->at(j)); 

                }
                p.key_s = p.row_id->front();
                p.key_c = p.col_ind.front();
                Hr.push(p);
                //res.push_back(p);
                
            }        
        }
        
    }
    index.clear();

    //std::cout << "inters: " << Hr.size() << std::endl;
    
    //std::priority_queue<Res_bic, std::vector<Res_bic>, decltype(&Res_bic::comp_row)> Hr(res.begin(), res.end(), &Res_bic::comp_row);
    //res.clear();
    //TIMERSTART(WHILE);
    //uint32_t count = 0;
    while (not Hr.empty()) {
        auto i = Hr.top();
        Hr.pop();
        Hc.push(i);
        //res.push_back(i);
        if (Hr.empty() or (Hr.top().key_s != i.key_s)) {
            //std::cout << "size: " <<  Hc.size() << std::endl;
            //res.clear();
            uint32_t sum = 0;
            //count++;
            while (not Hc.empty()) {
                auto j = Hc.top();
                Hc.pop();
                sum += j.values.at(j.index_c);

                if (Hc.empty() or j.key_c != Hc.top().key_c) {
                    csr_res->values.push_back(sum);
                    csr_res->col_ind.push_back(j.col_ind.at(j.index_c));

                    if (csr_res->row_id.empty() or (csr_res->row_id.back() != j.key_s)) {
                        csr_res->row_id.push_back(j.key_s);
                        csr_res->row_ptr.push_back(csr_res->col_ind.size()-1);
                    }
                    sum = 0;
                }
                if (j.index_c < j.col_ind.size() - 1) {
                    j.index_c++;
                    j.key_c = j.col_ind.at(j.index_c);
                    Hc.push(j);
                }
            }

        }
        if (i.index_s < i.row_id->size() - 1) {
            i.index_s++;
            i.key_s = i.row_id->at(i.index_s);
            Hr.push(i);
        }
        
    }
    csr_res->row_ptr.push_back(csr_res->col_ind.size()); 
    //TIMERSTOP(WHILE);
    //std::cout << "count: " <<  count << std::endl;
    //std::cout << "edges computed: " << csr_res->values.size() << std::endl;

    return csr_res;
}

csr_matrix* mult(Biclique* a, Biclique* b) //bxb
{
    
    assert(a != nullptr and b != nullptr);
    auto a_csc = a->get_csc();
    auto b_csr = b->get_csr();



    std::vector<std::vector<uint32_t>> h(a->maxDim()+1, std::vector<uint32_t>());

    for (size_t i = 0; i < b_csr->size(); i++) {
        for (size_t j = 0; j < b_csr->at(i)->row_id.size(); j++) {
            if (b_csr->at(i)->row_id.at(j) > a->maxDim()) break;
            h[b_csr->at(i)->row_id.at(j)].push_back(i);
        }
    }
    

    //std::vector<Res_bic> res;

    std::priority_queue<Inters_Bicl, std::vector<Inters_Bicl>, Inters_Bicl::comp_row> Hr;
    std::priority_queue<Inters_Bicl, std::vector<Inters_Bicl>, Inters_Bicl::comp_col> Hc;

    for (size_t i = 0; i < a_csc->size(); i++) {
        auto csc = a_csc->at(i);
        for (size_t j = 0; j < csc->col_id.size(); j++) {
            if (not h[csc->col_id[j]].empty()) {
                for (auto index_csr : h[csc->col_id[j]]) {
                    Inters_Bicl p;
                    p.row_id = &(csc->row_ind);
                    p.key_s = p.row_id->front();
                    //p.C = new std::vector<std::pair<uint32_t,uint32_t>>();

                    for(size_t row = 0; row < b_csr->at(index_csr)->col_ind.size(); row++){
                        //p.C->push_back(std::pair(b_csr->at(index_csr)->col_ind.at(row),weight * b_csr->at(index_csr)->values.at(row)));
                        p.col_ind.push_back(b_csr->at(index_csr)->col_ind.at(row));
                        p.values.push_back(csc->values[j] *  b_csr->at(index_csr)->values.at(row));
                    }
                    p.key_c = p.col_ind.front();
                    Hr.push(p);
                    //res.push_back(p);
                }
            }

        }
    }
    h.clear();
    auto csr_res = new csr_matrix();

    //std::cout << "inters: " << Hr.size() << std::endl;
  
    while (not Hr.empty()) {
        auto i = Hr.top();
        Hr.pop();
        Hc.push(i);

        if (Hr.empty() or (Hr.top().key_s != i.key_s)) {
            uint32_t sum = 0;
            while (not Hc.empty()) {
                auto j = Hc.top();
                Hc.pop();
                sum += j.values.at(j.index_c);

                if (Hc.empty() or j.key_c != Hc.top().key_c) {
                    csr_res->values.push_back(sum);
                    csr_res->col_ind.push_back(j.col_ind.at(j.index_c));

                    if (csr_res->row_id.empty() or (csr_res->row_id.back() != j.key_s)) {
                        csr_res->row_id.push_back(j.key_s);
                        csr_res->row_ptr.push_back(csr_res->col_ind.size()-1);
                    }
                    sum = 0;
                }
                if (j.index_c < j.col_ind.size() - 1) {
                    j.index_c++;
                    j.key_c = j.col_ind.at(j.index_c);
                    Hc.push(j);
                }
            }

        }
        if (i.index_s < i.row_id->size() - 1) {
            i.index_s++;
            i.key_s = i.row_id->at(i.index_s);
            Hr.push(i);
        }
        
    }
    csr_res->row_ptr.push_back(csr_res->col_ind.size()); 

    /* 1 heap
    uint32_t sum = 0;
    while (not Hr.empty()) {
        auto i = Hr.top();
        Hr.pop();

        sum += i.C->at(i.index_c).second;

        if (Hr.empty() or i.key_s != Hr.top().key_s or i.key_c != Hr.top().key_c) {
            csr_res->values.push_back(sum);
            csr_res->col_ind.push_back(i.C->at(i.index_c).first);

            if (csr_res->row_id.empty() or (csr_res->row_id.back() != i.key_s)) {
                csr_res->row_id.push_back(i.key_s);
                csr_res->row_ptr.push_back(csr_res->col_ind.size()-1);
            }
            sum = 0;
        }

        if (i.index_c < i.C->size() - 1) {
            i.index_c++;
            i.key_c = i.C->at(i.index_c).first;
            Hr.push(i);
        } else if (i.index_s < i.S->size() - 1) {
            i.index_s++;
            i.key_s = i.S->at(i.index_s);
            i.index_c = 0;
            i.key_c = i.C->at(i.index_c).first;
            Hr.push(i);
        }
        
    }*/
    
    //std::cout << "edges computed: " << csr_res->values.size() << std::endl;
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




