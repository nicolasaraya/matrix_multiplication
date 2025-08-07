#include "multiplicator_boolean.hpp"
#include "biclique_boolean.hpp"

#include <algorithm>
#include <set>

#ifndef DEBUG
#define DEBUG 0
#endif

std::ostream& operator<<(std::ostream& os, const Intersection& elem)
{
  os  << "\n++++++++++++++++\n"
      << "start_col: " << elem.start_col << "\n"
      << "end_col: " << elem.end_col << "\n"
      << "start_row: " << elem.start_row << "\n"
      << "end_row: " << elem.end_row << "\n"
      << "value_col: " << elem.value_col << "\n"
      << "value_row: " << elem.value_row << "\n"
      << "index_biclique: " << elem.index_bicl << "\n"
      << "----------------\n";
  return os;
}

std::ostream& operator<<(std::ostream& os, const Intersection* elem)
{
  return os << *elem;
}

#if 0
csr_matrix* mult(csc_matrix* A_csc, csr_matrix* B_csr)
{
  #if DEBUG
  assert(A_csc and B_csr);
  #endif

  PQ_Col Hr;
  PQ_Row Hc;

  size_t i = 0;
  size_t j = 0;

  size_t Acol_id_Size = A_csc->col_id.size();
  size_t Brow_id_Size = B_csr->row_id.size();

  size_t estEdges = 0;
  size_t estRows = 0;

  while (i <  Acol_id_Size and j < Brow_id_Size) {
    #if DEBUG
    std::cout << "i: " << i << std::endl;
    std::cout << "j: " << j << std::endl;
    std::cout << "Current A->col_id[i]: " << A_csc->col_id[i] << std::endl;
    std::cout << "Current B->row_id[j]: " << B_csr->row_id[j] << std::endl;
    #endif 

    if (A_csc->col_id[i] == B_csr->row_id[j]) {
      #if DEBUG
      std::cout << "EQUALS" << std::endl;
      #endif

      Intersection* inter = new Intersection();
      inter->start_col = A_csc->col_ptr[i];
      inter->end_col = A_csc->col_ptr[i+1];
      inter->start_row = B_csr->row_ptr[j];
      inter->end_row =  B_csr->row_ptr[j+1];
      inter->value_col = A_csc->row_ind[inter->start_col];
      inter->value_row = B_csr->col_ind[inter->start_row];
      Hr.push(inter);

      estEdges += (inter->end_col - inter->start_col) * (inter->end_row - inter->start_row);
      estRows += 1;
      
      #if DEBUG
      std::cout << "Pushing: " << inter << std::endl;  
      #endif

      ++i;
      ++j;
    } else if (A_csc->col_id[i] > B_csr->row_id[j]) {
      #if DEBUG
      std::cout << "Increasing j" << std::endl;
      #endif
      ++j;
    } else {
      #if DEBUG
      std::cout << "Increasing i" << std::endl;
      #endif
      ++i;
    }
  }

  auto* res = new csr_matrix;
  res->col_ind.reserve(estEdges);
  res->row_ptr.reserve(estRows);
  res->row_id.reserve(estRows);

  while (not Hr.empty()) {
    auto* elem = Hr.top();
    Hr.pop();
    Hc.push(new Intersection(*elem));

    #if DEBUG
    std::cout << "Pop Hr: " << elem << std::endl;
    #endif

    #if DEBUG
    std::cout << (Hr.empty() ? "Hr empty" : "") << std::endl;
    std::cout << (Hr.top()->value_col != elem->value_col ? "Hr.top() != current value_col" : "Hr.top() == current value_col") << std::endl;
    #endif

    if (Hr.empty() or (Hr.top()->value_col != elem->value_col)) {
      while (not Hc.empty()) {
        auto* inter = Hc.top();
        Hc.pop();

        #if DEBUG
        std::cout << "Pop Hc: " << inter << std::endl;
        #endif
        
        if (Hc.empty() or inter->value_row != Hc.top()->value_row) { //push
          res->col_ind.push_back(B_csr->col_ind[inter->start_row]);

          if (res->row_id.empty() or (res->row_id.back() != A_csc->row_ind[inter->start_col])) {
            res->row_id.push_back(A_csc->row_ind[inter->start_col]);
            res->row_ptr.push_back(res->col_ind.size()-1);
          }

          #if DEBUG
          std::cout << "(" << A_csc->row_ind[inter->start_col] << ", " << B_csr->col_ind[inter->start_row] << ")" << std::endl;
          #endif
        }

        if (inter->start_row < inter->end_row - 1) {
          ++(inter->start_row);
          inter->value_row = B_csr->col_ind[inter->start_row];
          Hc.push(inter);

          #if DEBUG 
          std::cout << "Inter modified, push in Hc: " << inter <<  std::endl;
          #endif
        }
      }
    } 

    if (elem->start_col < elem->end_col - 1) {
      ++(elem->start_col);
      elem->value_col = A_csc->row_ind[elem->start_col];
      Hr.push(elem);

      #if DEBUG 
      std::cout << "Elem modified, push in Hr: " << elem <<  std::endl;
      #endif
    } 

    #if DEBUG
    std::cout << std::endl << std::endl << "New iteration" << std::endl;
    #endif
  }

  res->row_ptr.push_back(res->col_ind.size());

  return res;
}
#endif 

#if 1
csr_matrix* mult(csc_matrix* A_csc, csr_matrix* B_csr)
{
  #if DEBUG
  assert(A_csc and B_csr);
  #endif

  PQ_Col Hr;
  PQ_Row Hc;

  size_t i = 0;
  size_t j = 0;

  size_t nCols = A_csc->nCols();
  size_t nRows = B_csr->nRows();

  const uint32_t* A_rows  = A_csc->rowIndPtr();
  const size_t* A_cptr  = A_csc->colPtrPtr();
  const uint32_t* A_cid   = A_csc->colIdPtr();

  const uint32_t* B_cols  = B_csr->colIndPtr();
  const size_t* B_rptr  = B_csr->rowPtrPtr();
  const uint32_t* B_rid   = B_csr->rowIdPtr();

  size_t estEdges = 0;
  size_t estRows = 0;

  while (i <  nCols and j < nRows) {
    #if DEBUG
    std::cout << "i: " << i << std::endl;
    std::cout << "j: " << j << std::endl;
    std::cout << "Current A->col_id[i]: " << A_cid[i] << std::endl;
    std::cout << "Current B->row_id[j]: " << B_rid[j] << std::endl;
    #endif 
    uint32_t cid = A_cid[i], rid = B_rid[j];
    if (cid == rid) {
      #if DEBUG
      std::cout << "EQUALS" << std::endl;
      #endif

      Intersection* inter = new Intersection();
      inter->start_col = A_cptr[i];
      inter->end_col = A_cptr[i+1];
      inter->start_row = B_rptr[j];
      inter->end_row =  B_rptr[j+1];
      inter->value_col = A_rows[inter->start_col];
      inter->value_row = B_cols[inter->start_row];

      //estEdges += (inter->end_col - inter->start_col) * (inter->end_row - inter->start_row);
      estRows += 1;
      Hr.push(inter);
      
      #if DEBUG
      std::cout << "Pushing: " << inter << std::endl;  
      #endif

      ++i;
      ++j;
    } else if (cid > rid) {
      #if DEBUG
      std::cout << "Increasing j" << std::endl;
      #endif
      ++j;
    } else {
      #if DEBUG
      std::cout << "Increasing i" << std::endl;
      #endif
      ++i;
    }
  }                       

  auto* res = new csr_matrix;
  #if DEBUG
  std::cout << "Estimated edges: " << estEdges << std::endl;
  std::cout << "Estimated rows: " << estRows << std::endl;
  #endif

  //res->col_ind.reserve(estEdges);
  res->row_ptr.reserve(estRows);
  res->row_id.reserve(estRows);

  while (not Hr.empty()) {
    auto* elem = Hr.top();
    Hr.pop();
    Hc.push(new Intersection(*elem));

    #if DEBUG
    std::cout << "Pop Hr: " << elem << std::endl;
    #endif

    #if DEBUG
    std::cout << (Hr.empty() ? "Hr empty" : "") << std::endl;
    std::cout << (Hr.top()->value_col != elem->value_col ? "Hr.top() != current value_col" : "Hr.top() == current value_col") << std::endl;
    #endif

    if (Hr.empty() or (Hr.top()->value_col != elem->value_col)) {
      while (not Hc.empty()) {
        auto* inter = Hc.top();
        Hc.pop();

        #if DEBUG
        std::cout << "Pop Hc: " << inter << std::endl;
        #endif
        
        if (Hc.empty() or inter->value_row != Hc.top()->value_row) { //push
          res->col_ind.push_back(B_cols[inter->start_row]);

          if (res->row_id.empty() or (res->row_id.back() != A_rows[inter->start_col])) {
            res->row_id.push_back(A_rows[inter->start_col]);
            res->row_ptr.push_back(res->col_ind.size()-1);
          }

          #if DEBUG
          std::cout << "(" << A_csc->row_ind[inter->start_col] << ", " << B_csr->col_ind[inter->start_row] << ")" << std::endl;
          #endif
        }

        if (inter->start_row < inter->end_row - 1) {
          ++(inter->start_row);
          inter->value_row = B_cols[inter->start_row];
          Hc.push(inter);

          #if DEBUG 
          std::cout << "Inter modified, push in Hc: " << inter <<  std::endl;
          #endif
        } else {
          delete inter;
        }
      }
    } 

    if (elem->start_col < elem->end_col - 1) {
      ++(elem->start_col);
      elem->value_col = A_rows[elem->start_col];
      Hr.push(elem);

      #if DEBUG 
      std::cout << "Elem modified, push in Hr: " << elem <<  std::endl;
      #endif
    } else {
      delete elem;
    }

    #if DEBUG
    std::cout << std::endl << std::endl << "New iteration" << std::endl;
    #endif
  }

  res->row_ptr.push_back(res->col_ind.size());
 //res->col_ind.shrink_to_fit();

  return res;
}
#endif

csr_matrix* mult(csc_matrix* A_csc, Biclique* b)
{
  #if DEBUG
  assert(A_csc and b);
  #endif

  auto* b_csr = b->get_csr();
  auto* res = new csr_matrix();

  std::vector<uint32_t> index(A_csc->col_id.back()+1, UINT32_MAX);
  for (size_t i = 0; i < A_csc->col_id.size(); i++) {
    index[A_csc->col_id[i]] = i;
    #if DEBUG
    std::cout << A_csc->col_id[i] << ": " << i << std::endl;
    #endif
  }

  PQ_Col Hr;
  PQ_Row Hc;

  for (size_t i = 0; i < b_csr->size(); ++i) {
    auto* csr = b_csr->at(i);
    #if DEBUG
    std::cout << "Intersecting biclique num: " << i << std::endl;  
    #endif

    for (size_t j = 0; j < csr->row_id.size(); ++j) {
      if (csr->row_id[j] < index.size() and index[csr->row_id[j]] != UINT32_MAX) {
        Intersection* inter = new Intersection();
        inter->start_col = A_csc->col_ptr[index[csr->row_id[j]]];
        inter->end_col = A_csc->col_ptr[index[csr->row_id[j]] + 1];
        inter->start_row = 0;
        inter->end_row = csr->col_ind.size();
        inter->value_col = A_csc->row_ind[inter->start_col];
        inter->value_row = csr->col_ind[inter->start_row];
        inter->index_bicl = i;
        Hr.push(inter);

        #if DEBUG
        std::cout << "Pushing: " << inter << std::endl;
        #endif
      }
    }
  }

  #if DEBUG
  std::cout << "Inters: " << Hr.size() << std::endl;
  #endif

  while (not Hr.empty()) {
    auto* elem = Hr.top();
    Hc.push(new Intersection(*elem));
    Hr.pop();

    #if DEBUG
    std::cout  << "Pop Hr: " << elem << std::endl;
    #endif

    #if DEBUG
    std::cout << (Hr.empty() ? "Hr empty" : "") << std::endl;
    std::cout << (Hr.top()->value_col != elem->value_col ? "Hr.top() != current value_col" : "Hr.top() == current value_col") << std::endl;
    #endif
        
    if (Hr.empty() or (Hr.top()->value_col != elem->value_col)) { //primer elemento de la columna es distinto
      while (not Hc.empty()) {
        auto* inter = Hc.top();
        Hc.pop();

        #if DEBUG
        std::cout  << "Pop Hc: " << inter << std::endl;
        #endif

        if (Hc.empty() or inter->value_row != Hc.top()->value_row) { // si queda vacio o si el siguiente valor es distinto, push en csr
          res->col_ind.push_back(b_csr->at(inter->index_bicl)->col_ind[inter->start_row]);

          if (res->row_id.empty() or (res->row_id.back() != A_csc->row_ind[inter->start_col])) {
            res->row_id.push_back(A_csc->row_ind[inter->start_col]);
            res->row_ptr.push_back(res->col_ind.size()-1);
          }

          #if DEBUG
          //std::cout <<"(" << A_csc->row_ind[inter->start_col] << ", " << A_csc->row_ind[inter->start_row] << ")" << std::endl;
          std::cout <<"(" << A_csc->row_ind[inter->start_col] << ", " << b_csr->at(inter->index_bicl)->col_ind[inter->start_row] << ")" << std::endl;
          #endif
        } 
        
        if (inter->start_row < inter->end_row - 1) {
          ++(inter->start_row);
          inter->value_row = b_csr->at(inter->index_bicl)->col_ind[inter->start_row];
          Hc.push(inter);

          #if DEBUG 
          std::cout << "Inter modified, push in Hc: " << inter <<  std::endl;
          #endif
        } else {
          delete inter;
        }
        
      }
    }

    if (elem->start_col < elem->end_col - 1) {
      ++(elem->start_col);
      elem->value_col = A_csc->row_ind[elem->start_col];
      Hr.push(elem);
    } else {
      delete elem;
    }
  }

  res->row_ptr.push_back(res->col_ind.size());

  #if DEBUG
  std::cout << "edges computed: " << res->col_ind.size() << std::endl;
  #endif

  return res;
}

csr_matrix* mult(Biclique* b, csr_matrix* A_csr)
{
  auto *inters = compute_intersections(b, A_csr);
  auto *res = csrFromIntersBicl(b, inters);
  std::vector<Inters_Bicl>().swap(*inters);
  return res;
}

csr_matrix* mult(Biclique* a, Biclique* b)
{
  auto *inters = compute_intersections(a, b);
  auto *res = csrFromIntersBicl(a, inters);
  std::vector<Inters_Bicl>().swap(*inters);
  return res;
}

std::vector<Inters_Bicl>* compute_intersections(Biclique* b, csr_matrix* A_csr)
{
  #if DEBUG
  assert(A_csr and b);
  #endif

  auto* b_csc = b->get_csc();
  auto* b_marks = b->get_marks();

  #if DEBUG
  b->printMarks();
  #endif

  std::vector<uint32_t> index(b->maxDim() + 1, UINT32_MAX);
  for (size_t i = 0; i < A_csr->row_id.size(); i++) {
    if (A_csr->row_id[i] > b->maxDim()) break;
    index[A_csr->row_id[i]] = i;
  }

  auto *intersections = new std::vector<Inters_Bicl>();


  for (size_t i = 0; i < b_csc->size(); i++) { 
    auto S_i = &(b_csc->at(i)->col_id);
    auto C_i = &(b_csc->at(i)->row_ind);

    Inters_Bicl p;
    p.S = C_i;
    std::vector<uint32_t> C_temp;

    size_t count = 0;

    for (size_t j = 0; j < S_i->size(); j++) { 
      size_t ind = S_i->at(j);
      if (index[ind] != UINT32_MAX) {
        size_t start_row = A_csr->row_ptr[index[ind]];
        size_t end_row = A_csr->row_ptr[index[ind] + 1];
        for (size_t k = start_row; k < end_row; k++) {
          C_temp.push_back(A_csr->col_ind[k]);
        }
        count++;
      }        
    }

    if (count > 1){
      std::sort(C_temp.begin(), C_temp.end());
      for (auto k : C_temp) {
        if (p.C.empty() or p.C.back() != k) {
          p.C.push_back(k);
        }
      }
    } else {
      p.C = C_temp;
    }
    intersections->push_back(p);
  }
  index.clear();

  return intersections;
}


std::vector<Inters_Bicl>* compute_intersections(Biclique* a, Biclique* b)
{
  #if DEBUG
  assert(a and b);
  #endif

  auto* a_csc = a->get_csc();
  auto* b_csr = b->get_csr();
  auto* a_marks = a->get_marks();

  auto *intersections = new std::vector<Inters_Bicl>(); 

  for (size_t i = 0; i < a_csc->size(); ++i) {
    Inters_Bicl p;
    std::vector<uint32_t> C_temp;
    p.S = &(a_csc->at(i)->col_id);
    size_t count = 0;

    for (size_t j = 0; j < a_csc->at(i)->col_id.size(); ++j) {
      auto* index_to_inter = b->get_indexes(a_csc->at(i)->col_id.at(j));
      if (index_to_inter == nullptr)  continue;
      
      for (auto& index_b : (*index_to_inter)) {
        
        for (size_t k = 0; k < b_csr->at(index_b)->col_ind.size(); ++k) {
          C_temp.push_back(b_csr->at(index_b)->col_ind.at(k));
        }
      }
      ++count;
    }

    if (count > 1) {
      std::sort(C_temp.begin(), C_temp.end());
      for (auto& k : C_temp) {
        #if DEBUG
        std::cout << k << std::endl;
        #endif
        if (p.C.empty() or p.C.back() != k) {
          p.C.push_back(k);
        }
      }
    } else {
      p.C = C_temp;
    }
    intersections->push_back(p);
  }

  return intersections;
}

csr_matrix* csrFromIntersBicl(Biclique* b, std::vector<Inters_Bicl>* intersections)
{
  auto* res = new csr_matrix();
  auto* b_marks = b->get_marks();

  for (auto& i : *b_marks) {
    res->row_ptr.push_back(res->col_ind.size());
    res->row_id.push_back(i.first);

    std::vector<uint32_t> C_temp;

    for (auto& j : i.second) {
      for (auto& k : intersections->at(j).C) {
        C_temp.push_back(k);
      }
    }

    if (i.second.size() > 0) std::sort(C_temp.begin(), C_temp.end());

    for (auto& j : C_temp) {
      if (res->col_ind.empty() or res->col_ind.size() == res->row_ptr.back() or res->col_ind.back() != j) {
        res->col_ind.push_back(j);
      }
    }
  }

  res->row_ptr.push_back(res->col_ind.size()); 
  return res;
}


csr_matrix* csr_add(csr_matrix* A, csr_matrix* B)
{
  csr_matrix* res = new csr_matrix();
  res->row_ptr.push_back(0);

  size_t i = 0; 
  size_t j = 0; 

  while (i < A->row_id.size() and j < B->row_id.size()) {
    if (A->row_id[i] == B->row_id[j]) {
      size_t startA = A->row_ptr[i];
      size_t stopA = A->row_ptr[i+1];
      size_t startB = B->row_ptr[j];
      size_t stopB = B->row_ptr[j+1];

      while (startA < stopA and startB < stopB) {
        if (A->col_ind[startA] == B->col_ind[startB]) {
          res->col_ind.push_back(A->col_ind[startA]);
          ++startA;
          ++startB;
        } else if (A->col_ind[startA] < B->col_ind[startB]) {
          res->col_ind.push_back(A->col_ind[startA]);
          ++startA;
        } else {
          res->col_ind.push_back(B->col_ind[startB]);
          ++startB;
        }
      }

      while (startA < stopA) {
        res->col_ind.push_back(A->col_ind[startA]);
        ++startA;
      }
      while (startB < stopB) {
        res->col_ind.push_back(B->col_ind[startB]);
        ++startB;
      }

      res->row_id.push_back(B->row_id[j]);
      res->row_ptr.push_back(res->col_ind.size());
      
      ++i;
      ++j;
    } else if (A->row_id[i] < B->row_id[j]) {
      size_t start = A->row_ptr[i];
      size_t stop = A->row_ptr[i+1];

      while (start < stop) {
        res->col_ind.push_back(A->col_ind[start]);
        ++start;
      }

      res->row_id.push_back(A->row_id[i]);
      res->row_ptr.push_back(res->col_ind.size());
      ++i;

    } else {
      size_t start = B->row_ptr[j];
      size_t stop = B->row_ptr[j+1];

      while (start < stop) {
        res->col_ind.push_back(B->col_ind[start]);
        ++start;
      }

      res->row_id.push_back(B->row_id[j]);
      res->row_ptr.push_back(res->col_ind.size());
      ++j;
    }

  }

  while (i < A->row_id.size()) {
    size_t start = A->row_ptr[i];
    size_t stop = A->row_ptr[i+1];

    while (start < stop) {
      res->col_ind.push_back(A->col_ind[start]);
      ++start;
    }
    res->row_id.push_back(A->row_id[i]);
    res->row_ptr.push_back(res->col_ind.size());
    ++i;

  }
  while (j < B->row_id.size()) {
    size_t start = B->row_ptr[j];
    size_t stop = B->row_ptr[j+1];

    while (start < stop) {
      res->col_ind.push_back(B->col_ind[start]);
      ++start;
    }

    res->row_id.push_back(B->row_id[j]);
    res->row_ptr.push_back(res->col_ind.size());
    ++j;

  } 
  return res;
}

Biclique* biclique_add(std::vector<Inters_Bicl>* interA, std::vector<Inters_Bicl>* interB)
{
  auto *merge = new Biclique();
  std::map<uint32_t, std::vector<uint32_t>> tempMark;

  #if DEBUG
  std::cout << "InterA size: " << interA->size() << std::endl;
  std::cout << "InterB size: " << interB->size() << std::endl;

  auto *merged = new std::vector<Inters_Bicl>();

  merged->insert(merged->end(), interA->begin(), interA->end());
  merged->insert(merged->end(), interB->begin(), interB->end());

  size_t count = 0;
  for(auto i : *merged) {
    std::cout << "Inter: " << count++ << std::endl;
    std::cout << "S:";
    for (auto &j : *(i.S)) {
      std::cout << " " << j;
    }
    std::cout << std::endl << "C:";
    for (auto &j : i.C) {
      std::cout << " " << j;
    }
    std::cout << std::endl;
  }
  #endif

  interA->insert(interA->end(), interB->begin(), interB->end());

  for (auto &bic : *interA) {
    auto b = new csr_biclique();
    auto *S  = bic.S;
    auto *C = &(bic.C);

    for (auto &value : *S) {
      b->row_id.push_back(value);
      tempMark[b->row_id.back()].push_back(merge->countBicliques()-1);
    }

    b->col_ind = *C;

    merge->add_csr(b);
  }

  merge->update_marks(tempMark);
  return merge;
}


