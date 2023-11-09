#ifndef CSR_MULT
#define CSR_MULT

#include <vector>
#include <iostream>

#include "../../biclique_extraction/Graph/Graph.hpp"
#include "../../biclique_extraction/Graph/GraphWeighted.hpp"
#include "../../biclique_extraction/Graph/Node.hpp"
#include "../../biclique_extraction/Utils/Utils.hpp"


namespace csr 
{
    struct SparseMatrix
    {
        std::vector<uInt> val; 
        std::vector<uInt> col; 
        std::vector<uInt> row_ptr; 
    };

    void print_csr(SparseMatrix*);

    SparseMatrix* graphWeightedToSparseMatrix(GraphWeighted*); 
    SparseMatrix* multiplySMxSMt(SparseMatrix*, SparseMatrix*);

    GraphWeighted* csrToGraphWeighted(SparseMatrix*);

};
#endif