#include "multiplicator_boolean.hpp"
#include "biclique_boolean.hpp"

#include <algorithm>
#include <unordered_set>
#include <utility>
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

void powBicl(Matrix* matrix, Biclique* biclique)
{
  #if DEBUG
  std::cout << "Matrix CSR:" << std::endl;
  matrix->get_csr()->print();
  std::cout << "Matrix CSC:" << std::endl;
  matrix->get_csc()->print();
  biclique->print_csr();
  biclique->print_csc();
  #endif

  bool useDelta16 = matrix->getUseDelta16();

  std::string pathMatrix = matrix->getPath();

  std::cout << "Starting pow with bicliques" << std::endl;
  TIMERSTART(TOTAL);
  TIMERSTART(total_operations);
  TIMERSTART(AxA);
  auto* AxA = mult(matrix->get_csc(), matrix->get_csr());
  TIMERSTOP(AxA);
  
  #if DEBUG
  AxA->print();
  AxA->printAsList();
  #endif

  TIMERSTART(Axb);
  auto* Axb = mult(matrix->get_csc(), biclique);
  TIMERSTOP(Axb);
  matrix->delete_csc();

  #if DEBUG
  Axb->print();
  Axb->printAsList();
  #endif

  TIMERSTART(bxA);
  auto* bxA = mult(biclique, matrix->get_csr());
  TIMERSTOP(bxA);
  matrix->delete_csr();
  delete matrix;

  #if DEBUG
  bxA->print();
  bxA->printAsList();
  #endif

  TIMERSTART(bxb);
  auto* bxb = mult(biclique, biclique);
  TIMERSTOP(bxb);
  TIMERSTOP(total_operations);
  delete biclique;

  #if DEBUG
  bxb->print();
  bxb->printAsList();
  #endif
  

  TIMERSTART(join);
  auto* join = csr_add(AxA, Axb);
  std::cout << "edges AxA + Axb: " << join->nEdges() << std::endl;
  delete AxA;
  delete Axb;
  auto* join2 = csr_add(bxA, bxb);
  std::cout << "edges bxA + bxb: " << join2->nEdges() << std::endl;
  delete bxA;
  delete bxb;
  auto* join3 = csr_add(join, join2);
  delete join;
  delete join2;
  TIMERSTOP(join);
  TIMERSTOP(TOTAL);

  Matrix res;
  res.set_csr(join3);
  res.setUseDelta16(useDelta16);

  #if DEBUG
  join3->print();
  join3->printAsList();
  #else
  // auto newPath = utils::modify_path(pathMatrix, "_powBic.txt");
  // res.saveTxt(newPath);
  auto newPath = utils::modify_path(pathMatrix, "_powBic.bin");
  res.saveBin(newPath);

  // Calcular bits por arista
  std::ifstream file(newPath, std::ios::binary | std::ios::ate);
  if (file) {
    std::streamsize fileSize = file.tellg();
    file.close();
    size_t nEdges = join3->nEdges();
    if (nEdges > 0) {
      double bitsPerEdge = (double(fileSize) * 8.0) / double(nEdges);
      std::cout << "Bits per edge: " << bitsPerEdge << std::endl;
    }
  }

  #endif

}

void powBicl(Matrix* matrix, Biclique* biclique, Matrix*& outMatrix, Biclique*& outBiclique)
{
  #if DEBUG
  matrix->get_csr()->print();
  matrix->get_csc()->print();
  biclique->print_csr();
  biclique->print_csc();
  #endif

  bool useDelta16 = matrix->getUseDelta16();

  std::string pathMatrix = matrix->getPath();
  std::string pathBicliques = biclique->getPath();

  std::cout << "Starting pow with bicliques" << std::endl;
  TIMERSTART(TOTAL);
  TIMERSTART(total_operations);
  TIMERSTART(AxA);
  auto* AxA = mult(matrix->get_csc(), matrix->get_csr());
  TIMERSTOP(AxA);
  
  #if DEBUG
  AxA->print();
  AxA->printAsList();
  #endif

  TIMERSTART(Axb);
  auto* Axb = mult(matrix->get_csc(), biclique);
  TIMERSTOP(Axb);
  matrix->delete_csc();

  #if DEBUG
  Axb->print();
  Axb->printAsList();
  #endif

  TIMERSTART(bxA);
  auto* bxAinter = compute_intersections(biclique, matrix->get_csr());
  TIMERSTOP(bxA);

  matrix->delete_csr();
  delete matrix;

  TIMERSTART(bxb);
  auto* bxbinter = compute_intersections(biclique, biclique);
  TIMERSTOP(bxb);
  TIMERSTOP(total_operations);
  
  TIMERSTART(join);
  auto* bicbxA = bicliqueFromIntersBicl(biclique, bxAinter);
  //delete bxAinter;
  auto* bicbxb = bicliqueFromIntersBicl(biclique, bxbinter);
  //delete bxbinter;
  //delete biclique;
  bicbxA->addBiclique(bicbxb);
  delete bicbxb;
  outBiclique = bicbxA;
  auto* join = csr_add(AxA, Axb);
  delete AxA;
  delete Axb;
  TIMERSTOP(join);

  TIMERSTOP(TOTAL);
  auto* join2 = csr_add(csrFromIntersBicl(biclique, bxAinter), csrFromIntersBicl(biclique, bxbinter));

  outMatrix = new Matrix();
  outMatrix->set_csr(join);
  outMatrix->setUseDelta16(useDelta16);

  #if DEBUG
  join->print();
  join->printAsList();
  #else
  // auto newPath = utils::modify_path(pathMatrix, "_powBic_cm.txt");
  // outMatrix->saveTxt(newPath);
  // auto newPathBic = utils::modify_path(pathBicliques, "_powBic_cb.txt");
  // outBiclique->saveTxt(newPathBic);
  auto newPath = utils::modify_path(pathMatrix, "_powBic_cm.bin");
  outMatrix->saveBin(newPath);
  auto newPathBic = utils::modify_path(pathBicliques, "_powBic_cb.bin");
  outBiclique->saveBin(newPathBic);

  Matrix temp;
  temp.set_csr(csr_add(join, join2));
  temp.setUseDelta16(useDelta16);
  auto newPathCsrFull = utils::modify_path(pathMatrix, "_powBic.bin");
  temp.saveBin(newPathCsrFull);

  std::ifstream fileCM(newPath, std::ios::binary | std::ios::ate);
  std::ifstream fileCB(newPathBic, std::ios::binary | std::ios::ate);
  std::ifstream fileFull(newPathCsrFull, std::ios::binary | std::ios::ate);

  if (fileCM and fileCB and fileFull) {
    std::streamsize sizeCM = fileCM.tellg();
    std::streamsize sizeCB = fileCB.tellg();
    std::streamsize sizeFull = fileFull.tellg();
    fileCM.close();
    fileCB.close();
    fileFull.close();

    size_t nEdgesFull = temp.get_csr()->nEdges();

    if (nEdgesFull > 0) {
      double bpeCMCB = (double(sizeCM + sizeCB) * 8.0) / double(nEdgesFull);
      std::cout << "Bits per edge (cm+cb): " << bpeCMCB << std::endl;

      double bpeFull = (double(sizeFull) * 8.0) / double(nEdgesFull);
      std::cout << "Bits per edge (powBic.bin): " << bpeFull << std::endl;
    }
  }

  #endif
  delete bxAinter;
  delete bxbinter;
  delete biclique;
  delete join2;
  
  return;
}

void pow(Matrix* matrix)
{
  #if DEBUG
  matrix->get_csr()->print();
  matrix->get_csc()->print();
  #endif

  std::cout << "Starting pow" << std::endl;

  bool useDelta16 = matrix->getUseDelta16();
  std::string originalPath = matrix->getPath();
  TIMERSTART(AxA);
  auto* AxA = mult(matrix->get_csc(), matrix->get_csr());
  TIMERSTOP(AxA);
  delete matrix;

  Matrix res;
  res.set_csr(AxA);
  res.setUseDelta16(useDelta16);

  #if DEBUG
  AxA->print();
  AxA->printAsList();
  #else

  auto newPath = utils::modify_path(originalPath, "_pow.bin");
  res.saveBin(newPath);

  std::ifstream file(newPath, std::ios::binary | std::ios::ate);
  if (file) {
    std::streamsize fileSize = file.tellg();
    file.close();
    size_t nEdges = AxA->nEdges();
    if (nEdges > 0) {
      double bitsPerEdge = (double(fileSize) * 8.0) / double(nEdges);
      std::cout << "Bits per edge (pow.bin): " << bitsPerEdge << std::endl;
    }
  }

  #endif
}

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
    p.S.assign(C_i->begin(), C_i->end());
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

    if (count > 1) {
      std::sort(C_temp.begin(), C_temp.end());
      for (auto k : C_temp) {
        if (p.C.empty() or p.C.back() != k) {
          p.C.push_back(k);
        }
      }
    } else {
      //assert(not C_temp.empty());
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
    p.S = (a_csc->at(i)->col_id);
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

    std::unordered_set<uint32_t> visited;

    for (auto& j : i.second) {
      for (auto& k : intersections->at(j).C) {
        if (visited.emplace(k).second) {
          C_temp.push_back(k);
        }
      }
    }

    if (i.second.size() > 0) std::sort(C_temp.begin(), C_temp.end());

    for (auto& j : C_temp) {
      if (res->col_ind.empty() or res->col_ind.size() == res->row_ptr.back() or res->col_ind.back() < j) { // or =
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

Biclique* bicliqueFromIntersBicl(Biclique* b, std::vector<Inters_Bicl>* intersections)
{
  auto* newBicl = new Biclique();

  std::unordered_map<uint32_t, std::vector<uint32_t>> visited;
  std::map<uint32_t, std::vector<uint32_t>> tempMark;

  auto* b_marks = b->get_marks();

  for (auto& i : *b_marks) {
    for (auto& j : i.second) {
      if (intersections->at(j).C.empty()) {
        continue;
      }
      visited[j].push_back(i.first);
    }
  }

  for (size_t i = 0; i < intersections->size(); i++) {
    if (not visited[i].empty()) {
      auto newCsr = new csr_biclique();
      newCsr->col_ind.assign(intersections->at(i).C.begin(), intersections->at(i).C.end());
      newCsr->row_id.assign(visited[i].begin(), visited[i].end());
      newBicl->add_csr(newCsr);
       for (auto& s : newCsr->row_id) {
        tempMark[s].push_back(newBicl->countBicliques()-1);
      }
    }
  }

  newBicl->update_marks(tempMark);

  return newBicl;
  
}

struct PairHash
{
  template <class T1, class T2>
  std::size_t operator()(const std::pair<T1, T2>& p) const 
  {
    std::size_t h1 = std::hash<T1>{}(p.first);
    std::size_t h2 = std::hash<T2>{}(p.second);
    return h1 ^ (h2 << 1); // combinación simple de hashes
  }
};

struct PairEq
{
  bool operator()(const std::pair<uint32_t,uint32_t>& a, const std::pair<uint32_t,uint32_t>& b) const
  {
    return a.first == b.first && a.second == b.second;
  }
};


Inters_Bicl removeSFromInter(Inters_Bicl& original, uint32_t index)
{
  Inters_Bicl generated;
  for (auto& s : original.S) {
    if (s != index) {
      generated.S.push_back(s);
    }
  }
  generated.C.insert(generated.C.end(), original.C.begin(), original.C.end());
  return generated;
}

void removeCFromInter(Inters_Bicl*& original, uint32_t index)
{
  std::vector<uint32_t> filteredC;
  for (auto& c : original->C) {
    if (c != index) {
      filteredC.push_back(c);
    }
  }
  original->C.swap(filteredC);
}

Biclique* biclique_add(Biclique* a, Biclique* b, std::vector<Inters_Bicl>* interA, std::vector<Inters_Bicl>* interB)
{
  auto *merge = new Biclique();

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

  auto aIter = a->get_marks()->begin();
  auto bIter = b->get_marks()->begin();
  auto aEnd = a->get_marks()->end();
  auto bEnd = b->get_marks()->end();

  // std::unordered_map<uint32_t, std::vector<uint32_t>> tempMarkA;
  // std::unordered_map<uint32_t, std::vector<uint32_t>> tempMarkB;

  while (aIter != aEnd and bIter != bEnd) {
    auto aId = (*aIter).first;
    auto bId = (*bIter).first;
    std::unordered_set<uint32_t> visited;

    if (aId == bId) {
      //a->get_indexes()
      //tempMarkA[aId].insert(tempMarkA[aId].end(), (*aIter).second.begin(), (*aIter).second.end());

      for (auto& i : (*aIter).second) {
        for (auto& j : interA->at(i).C) {

          std::vector<uint32_t> rep;
          if (visited.emplace(j).second) {
            rep.push_back(j);
          }

          if (not rep.empty()) {
            interA->push_back(removeSFromInter(interA->at(i), aId));

            for (auto &r : rep) {
              auto* ptr = &(interA->at(i));
              removeCFromInter(ptr, r);
            }

            for (auto s = interA->at(i).S.begin(); s != interA->at(i).S.end(); s++)  {
              if (*s != aId) {
                if (a->get_indexes(*s)) {
                  a->get_indexes(*s)->push_back(interA->size()-1);
                } else {
                  std::cout << "Warning" << std::endl;
                }
              }
            }
          }
        }
      }


      for (auto& i : (*bIter).second) {
        for (auto& j : interB->at(i).C) {

          std::vector<uint32_t> rep;
          if (visited.emplace(j).second) {
            rep.push_back(j);
          }

          if (not rep.empty()) {
            interB->push_back(removeSFromInter(interB->at(i), aId));

            for (auto &r : rep) {
              auto* ptr = &(interB->at(i));
              removeCFromInter(ptr, r);
            }

            for (auto s = interB->at(i).S.begin(); s != interB->at(i).S.end(); s++)  {
              if (*s != bId) {
                if (b->get_indexes(*s)) {
                  b->get_indexes(*s)->push_back(interB->size()-1);
                } else {
                  std::cout << "Warning" << std::endl;
                }
              }
            }
          }
        }
      }
      aIter++;
      bIter++;
    } else if (aId < bId) {

      for (auto& i : (*aIter).second) {
        for (auto& j : interA->at(i).C) {

          std::vector<uint32_t> rep;
          if (visited.emplace(j).second) {
            rep.push_back(j);
          }

          if (not rep.empty()) {
            interA->push_back(removeSFromInter(interA->at(j), aId));

            for (auto &r : rep) {
              auto* ptr = &(interA->at(j));
              removeCFromInter(ptr, r);
            }

            for (auto s = interA->at(j).S.begin(); s != interA->at(j).S.end(); s++)  {
              if (*s != aId) {
                if (a->get_indexes(*s)) {
                  a->get_indexes(*s)->push_back(interA->size()-1);
                } else {
                  std::cout << "Warning" << std::endl;
                }
              }
            }
          }
        }
      }
      aId++;
    } else {
      for (auto& i : (*bIter).second) {
        for (auto& j : interB->at(i).C) {

          std::vector<uint32_t> rep;
          if (visited.emplace(j).second) {
            rep.push_back(j);
          }

          if (not rep.empty()) {
            interB->push_back(removeSFromInter(interB->at(j),bId));

            for (auto &r : rep) {
              auto* ptr = &(interB->at(j));
              removeCFromInter(ptr, r);
            }

            for (auto s = interB->at(j).S.begin(); s != interB->at(j).S.end(); s++)  {
              if (*s != bId) {
                if (b->get_indexes(*s)) {
                  b->get_indexes(*s)->push_back(interB->size()-1);
                } else {
                  std::cout << "Warning" << std::endl;
                }
              }
            }
          }
        }
      }
      bId++;
    }
  }

  std::map<uint32_t, std::vector<uint32_t>> tempMarks;

  for (auto& bicA : *interA) {
    if (bicA.C.empty() or bicA.S.empty()) {
      continue;
    }

    auto csr = new csr_biclique();
    csr->col_ind.insert(csr->col_ind.end(), bicA.C.begin(), bicA.C.end());

    for (auto& s : bicA.S) {
      csr->row_id.push_back(s);
      tempMarks[s].push_back(merge->countBicliques());
    }
    merge->add_csr(csr);
  }

  for (auto& bicB : *interB) {
    if (bicB.C.empty() or bicB.S.empty()) {
      continue;
    }

    auto csr = new csr_biclique();
    csr->col_ind.insert(csr->col_ind.end(), bicB.C.begin(), bicB.C.end());

    for (auto& s : bicB.S) {
      csr->row_id.push_back(s);
      tempMarks[s].push_back(merge->countBicliques());
    }
    merge->add_csr(csr);
  }
  merge->update_marks(tempMarks); 
  return merge;
}


