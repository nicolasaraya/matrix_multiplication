#include "multiplicator.hpp"


Matrix* mat_mult(Matrix* A, Matrix* B) 
{
    auto res_csr = mult(A->get_csc(), B->get_csr());
    auto res = new Matrix();
    res->set_csr(res_csr);
    return res;
}

Matrix* mat_mult(Matrix* A, Biclique* Ab, Matrix* B, Biclique* Bb) 
{
    TIMERSTART(AXA);
    auto resAxB = mult(A->get_csc(), B->get_csr()); //AxB
    TIMERSTOP(AXA);
    
    TIMERSTART(AXB);
    auto resAxBb = mult(A->get_csc(), Bb->get_csr()); //A X Bb
    TIMERSTOP(AXB);
    
    TIMERSTART(APPEND);
    auto add = csr_add(resAxB, resAxBb);
    TIMERSTOP(APPEND);
    delete resAxB;
    delete resAxBb;

    TIMERSTART(BXA);
    auto resAbxA = mult(Ab->get_csr(),Ab->get_csc(), B->get_csr()); //Ab x B
    TIMERSTOP(BXA);
    
    TIMERSTART(BXB);
    auto resAbxBb = mult(Ab->get_csc(), Bb->get_csr()); //Ab X Bb
    TIMERSTOP(BXB);

    TIMERSTART(APPEND2);
    auto add2 = csr_add(resAbxA, resAbxBb);
    TIMERSTOP(APPEND2);
    delete resAbxA;
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


csr_matrix* mult(csc_matrix* csc, csr_matrix* csr)
{
    assert(csc != nullptr and csr != nullptr);
    auto inters = get_intersections(csc, csr);
    return compute_intersections(inters, csc, csr);
    return nullptr;
}

csr_matrix* mult(csc_matrix* csc, csr_biclique* csr) 
{
    assert(csc != nullptr and csr != nullptr);
    auto inters = get_intersections(csc, csr);
    return compute_intersections(inters, csc, csr);
}


csr_matrix* mult(csr_biclique* csr_b, csc_biclique* csc_b, csr_matrix* A) //bxA
{
    assert(csr_b != nullptr and csc_b != nullptr and A != nullptr);
    auto inters = get_intersections(csr_b, csc_b, A);
    return compute_intersections(inters, csr_b, csc_b, A);
}

csr_matrix* mult(csc_biclique* csc, csr_biclique* csr) //bxb
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


/* Compute Intersections */

csr_matrix* compute_intersections(std::vector<Intersection_MM>* inters, csc_matrix* csc, csr_matrix* csr) 
{
    assert(csc != nullptr and csr != nullptr);

    auto csr_res = new csr_matrix;

    std::priority_queue<Intersection_MM, std::vector<Intersection_MM>, Intersection_MM::Col_Comp> Hr;
    std::priority_queue<Intersection_MM, std::vector<Intersection_MM>, Intersection_MM::Row_Comp> Hc;


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

csr_matrix* compute_intersections(std::vector<Intersection_MM>* inters, csc_matrix* csc, csr_biclique* csr)  //Axb
{
    assert(csc != nullptr and csr != nullptr);

    auto csr_res = new csr_matrix;

    std::priority_queue<Intersection_MM, std::vector<Intersection_MM>, Intersection_MM::Col_Comp> Hr;
    std::priority_queue<Intersection_MM, std::vector<Intersection_MM>, Intersection_MM::Row_Comp> Hc;


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

csr_matrix* compute_intersections(std::vector<Intersection_bb>* inters, csr_biclique* csr_b, csc_biclique* csc_b, csr_matrix* A) //bxA
{
   assert(csc_b != nullptr and csr_b != nullptr and A != nullptr);

    auto csr_res = new csr_matrix;

    std::priority_queue<Intersection_bb, std::vector<Intersection_bb>, Intersection_bb::Col_Comp> Hr;
    std::priority_queue<Intersection_bb, std::vector<Intersection_bb>, Intersection_bb::Row_Comp> Hc;


    for (auto i : *inters) {
        //std::cout << i.col_id << " | " << i.start_col << " " << i.end_col << " | " << i.start_row << " " << i.end_row << " | " << i.value_col << " " << i.value_row << " | " << i.weight <<std::endl;
        Hr.push(i);
    }

    delete inters;

    
    while (not Hr.empty()) {
        auto elem = Hr.top();
        Hr.pop();
        Hc.push(elem);
        //std::cout << elem.col_id << " | " << elem.start_col << " " << elem.end_col << " | " << elem.start_row << " " << elem.end_row << " | " << elem.value_col << " " << elem.value_row << " | " << elem.weight <<std::endl;
                
        if (Hr.empty() or (Hr.top().value_col != elem.value_col)) { //primer elemento de la columna es distinto
            uint32_t sum = 0;
            while (not Hc.empty()) {
                auto inter = Hc.top();
                Hc.pop(); 
                //std::cout << "pop Hc " << "inter: " <<  inter.col_id << " | " << inter.start_col << " " << inter.end_col << " | " << inter.start_row << " " << inter.end_row << " | " << inter.value_col << " " << inter.value_row << " | " << inter.weight <<std::endl;
                sum += inter.weight * A->values[inter.start_row];

                if (Hc.empty() or inter.value_row != Hc.top().value_row) { // si queda vacio o si el siguiente valor es distinto, push en csr
                    csr_res->values.push_back(sum);
                    csr_res->col_ind.push_back(A->col_ind[inter.start_row]);

                    if (csr_res->row_id.empty() or (csr_res->row_id.back() != csr_b->row_id[inter.col_id][inter.start_col])) {
                        csr_res->row_id.push_back(csr_b->row_id[inter.col_id][inter.start_col]);
                        csr_res->row_ptr.push_back(csr_res->col_ind.size()-1);
                    }

                    //std::cout <<"(" << csc->row_ind[inter.start_col] << ", " << csr->col_ind[inter.start_row] << ") = " << sum << std::endl;
                    sum = 0;
                } 
                
                if (inter.start_row < inter.end_row - 1) {
                    inter.start_row++;
                    inter.value_row = A->col_ind[inter.start_row];
                    //std::cout << "inter modified:" << inter.index_col << " " << inter.index_row << " | " << inter.start_col << " " << inter.end_col << " | " << inter.start_row << " " << inter.end_row << " | " << inter.value_col << " " << inter.value_row << std::endl;
                    Hc.push(inter);
                } 
                
            }
        }

        if (elem.start_col < elem.end_col - 1) {
            elem.start_col++;
            elem.value_col = csr_b->row_id[elem.col_id][elem.start_col];
            Hr.push(elem);
        }   

        //std::cout << "new iter" << std::endl;
        
    }
    
    
    csr_res->row_ptr.push_back(csr_res->values.size());
    return csr_res;    
}



csr_matrix* compute_intersections(std::vector<Intersection_bb>* inters, csc_biclique* csc_b, csr_biclique* csr_b) //bxb
{
   assert(csc_b != nullptr and csr_b != nullptr);

    auto csr_res = new csr_matrix;

    std::priority_queue<Intersection_bb, std::vector<Intersection_bb>, Intersection_bb::Col_Comp> Hr;
    std::priority_queue<Intersection_bb, std::vector<Intersection_bb>, Intersection_bb::Row_Comp> Hc;


    for (auto i : *inters) {
        //std::cout << i.col_id << " | " << i.start_col << " " << i.end_col << " | " << i.start_row << " " << i.end_row << " | " << i.value_col << " " << i.value_row << " | " << i.weight <<std::endl;
        Hr.push(i);
    }

    delete inters;

    
    while (not Hr.empty()) {
        auto elem = Hr.top();
        Hr.pop();
        Hc.push(elem);
        //std::cout << elem.col_id << " | " << elem.start_col << " " << elem.end_col << " | " << elem.start_row << " " << elem.end_row << " | " << elem.value_col << " " << elem.value_row << " | " << elem.weight <<std::endl;
                
        if (Hr.empty() or (Hr.top().value_col != elem.value_col)) { //primer elemento de la columna es distinto
            uint32_t sum = 0;
            while (not Hc.empty()) {
                auto inter = Hc.top();
                Hc.pop(); 
                //std::cout << "pop Hc " << "inter: " <<  inter.col_id << " | " << inter.start_col << " " << inter.end_col << " | " << inter.start_row << " " << inter.end_row << " | " << inter.value_col << " " << inter.value_row << " | " << inter.weight <<std::endl;
                sum += inter.weight * csr_b->values[inter.start_row];

                if (Hc.empty() or inter.value_row != Hc.top().value_row) { // si queda vacio o si el siguiente valor es distinto, push en csr
                    csr_res->values.push_back(sum);
                    csr_res->col_ind.push_back(csr_b->col_ind[inter.start_row]);

                    if (csr_res->row_id.empty() or (csr_res->row_id.back() != csr_b->row_id[inter.col_id][inter.start_col])) {
                        csr_res->row_id.push_back(csr_b->row_id[inter.col_id][inter.start_col]);
                        csr_res->row_ptr.push_back(csr_res->col_ind.size()-1);
                    }

                    //std::cout <<"(" << csc->row_ind[inter.start_col] << ", " << csr->col_ind[inter.start_row] << ") = " << sum << std::endl;
                    sum = 0;
                } 
                
                if (inter.start_row < inter.end_row - 1) {
                    inter.start_row++;
                    inter.value_row = csr_b->col_ind[inter.start_row];
                    //std::cout << "inter modified:" << inter.index_col << " " << inter.index_row << " | " << inter.start_col << " " << inter.end_col << " | " << inter.start_row << " " << inter.end_row << " | " << inter.value_col << " " << inter.value_row << std::endl;
                    Hc.push(inter);
                } 
                
            }
        }

        if (elem.start_col < elem.end_col - 1) {
            elem.start_col++;
            elem.value_col = csr_b->row_id[elem.col_id][elem.start_col];
            Hr.push(elem);
        }   

        //std::cout << "new iter" << std::endl;
        
    }
    
    csr_res->row_ptr.push_back(csr_res->values.size());
    return csr_res;    
}

/* Get Intersections */



std::vector<Intersection_MM>* get_intersections(csc_matrix* A, csr_matrix* B) //AxA
{
    auto inters = new std::vector<Intersection_MM>();

    assert(A != nullptr and B != nullptr);

    size_t i = 0;
    size_t j = 0;

    while (i < A->col_id.size() and j < B->row_id.size()) {
        if (A->col_id[i] == B->row_id[j]) {
            Intersection_MM inter;
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



std::vector<Intersection_MM>* get_intersections(csc_matrix* A, csr_biclique* csr_b) //Axb
{
    auto inters = new std::vector<Intersection_MM>();

    assert(A != nullptr and csr_b != nullptr);
    
    std::vector<uint32_t> indexes(csr_b->row_id.size(), 0);
    
    for (size_t i = 0; i < A->col_id.size(); i++) {
        for (size_t j = 0; j < csr_b->row_id.size(); j++) {
            while (indexes[j] < csr_b->row_id[j].size() and A->col_id[i] > csr_b->row_id[j].at(indexes[j])){
                indexes[j]++;
            } 
            if (indexes[j] != csr_b->row_id[j].size() and A->col_id[i] == csr_b->row_id[j].at(indexes[j])) { 
                Intersection_MM inter;
                inter.start_col = A->col_ptr[i];
                inter.end_col = A->col_ptr[i+1];
                inter.start_row = csr_b->row_ptr[j];
                inter.end_row =  csr_b->row_ptr[j+1];
                inter.value_col = A->row_ind[inter.start_col];
                inter.value_row = csr_b->col_ind[inter.start_row];
                inters->push_back(inter);
                indexes[j]++;
            } 
        }
    }  
    return inters;
}


std::vector<Intersection_bb>* get_intersections(csr_biclique* csr_b, csc_biclique* csc_b, csr_matrix* csr)   //bxA
{
    auto inters = new std::vector<Intersection_bb>();

    assert(csr_b != nullptr and csc_b != nullptr and csr != nullptr);

    size_t i = 0; 
    size_t j = 0;

    while (i < csc_b->col_id.size() and j < csr->row_id.size()) {
        if (csc_b->col_id[i] == csr->row_id[j]) {
            uint32_t index_row_start = csc_b->col_ptr[i];
            uint32_t index_row_end = csc_b->col_ptr[i+1];

            uint32_t start_row = csr->row_ptr[j];
            uint32_t end_row = csr->row_ptr[j+1];

            for (size_t k = index_row_start; k < index_row_end; k++) {
                Intersection_bb inter;
                inter.col_id = csc_b->row_ind[k];
                inter.start_col = 0;
                inter.end_col = csr_b->row_id[inter.col_id].size();
                inter.start_row = start_row;
                inter.end_row = end_row;
                inter.value_col = csr_b->row_id[inter.col_id][0];
                inter.value_row = csr->col_ind[inter.start_row];
                inter.weight = csc_b->values[k];
                inters->push_back(inter);
            }
            i++;
            j++;
        } else if (csc_b->col_id[i] > csr->row_id[j]) {
            j++;
        } else {
            i++;
        }

    }
    return inters;


}


std::vector<Intersection_bb>* get_intersections(csc_biclique* csc_b, csr_biclique* csr_b) //bxb
{
    auto inters = new std::vector<Intersection_bb>();

    assert(csc_b != nullptr and csr_b != nullptr);

    std::vector<uint32_t> indexes(csr_b->row_id.size(), 0);
    
    for (size_t i = 0; i < csc_b->col_id.size(); i++) {
        for (size_t j = 0; j < csr_b->row_id.size(); j++) {
            while (indexes[j] < csr_b->row_id.at(j).size() and csc_b->col_id[i] > csr_b->row_id.at(j).at(indexes[j])){
                indexes[j]++;
            }
            
            if (indexes[j] != csr_b->row_id[j].size() and csc_b->col_id[i] == csr_b->row_id[j].at(indexes[j])) { 
                uint32_t index_row_start = csc_b->col_ptr[i];
                uint32_t index_row_stop = csc_b->col_ptr[i+1];

                uint32_t start_row = csr_b->row_ptr[j];
                uint32_t end_row = csr_b->row_ptr[j+1];

                for (size_t k = index_row_start; k < index_row_stop; k++) {
                    Intersection_bb inter;
                    inter.col_id = csc_b->row_ind[k];
                    inter.start_col = 0;
                    inter.end_col = csr_b->row_id[inter.col_id].size();
                    inter.start_row = start_row;
                    inter.end_row = end_row;
                    inter.value_col = csr_b->row_id[inter.col_id][0];
                    inter.value_row = csr_b->col_ind[inter.start_row];
                    inter.weight = csc_b->values[k];
                    inters->push_back(inter);
                }

                indexes[j]++;
            } 
        }
    }

    return inters;
}
