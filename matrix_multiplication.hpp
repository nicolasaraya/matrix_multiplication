#ifndef MATRIX_MULTIPLICATION
#define MATRIX_MULTIPLICATION

#define NUM_THREADS 1


#include "../biclique_extraction/Graph/Graph.hpp"
#include "../biclique_extraction/Graph/GraphWeighted.hpp"
#include "../biclique_extraction/Graph/Node.hpp"
#include "../biclique_extraction/Utils/Utils.hpp"

#include <vector>
#include <algorithm>
#include <string.h>
#include <omp.h>
#include <mutex>
#include <cassert>

namespace Lisa 
{
    struct Biclique 
    {
        std::vector<uInt> S;
        std::vector<uInt> C;
        std::vector<std::pair<uInt,uInt>> C_w; 
    };

    struct CompactBicliqueWeighted 
    {
        typedef std::vector<std::pair<uInt, uInt>> C_values;
        std::set<uInt> weights_values;
        std::vector<C_values> c_bicliques; 
        std::vector<std::pair<uInt, std::vector<uInt>>> linked_s; //S_value to C_values index   
    };

    std::vector<Biclique*>* load_biclique_w(std::string);
    std::vector<Biclique*>* load_biclique_w_bin(std::string);
    CompactBicliqueWeighted* load_CompactBiclique_w(std::string);
    CompactBicliqueWeighted* load_CompactBiclique_w_bin(std::string);

    void printBicliqueAsMatrix(Biclique*);
    void printCompactStructure(CompactBicliqueWeighted*);

    GraphWeighted* matrix_multiplication_w(GraphWeighted*, GraphWeighted*, bool);
    GraphWeighted* matrix_multiplication_w(GraphWeighted*, GraphWeighted*, std::vector<Biclique*>*, bool);
    GraphWeighted* matrix_multiplication_w(GraphWeighted*, GraphWeighted*, CompactBicliqueWeighted*, bool);
    GraphWeighted* matrix_multiplication_w(GraphWeighted*, GraphWeighted*); //standard multiplication
    //GraphWeighted* matrix_multiplication_w(GraphWeighted*, GraphWeighted*, std::vector<Biclique*>*);
    //GraphWeighted* matrix_multiplication_w(GraphWeighted*, GraphWeighted*, CompactBicliqueWeighted*);
    GraphWeighted* matrix_multiplication_w_transposed(GraphWeighted*, GraphWeighted*); //standard transpose multiplication
    //GraphWeighted* matrix_multiplication_w_transposed(GraphWeighted*, GraphWeighted*, std::vector<Biclique*>*);
    //GraphWeighted* matrix_multiplication_w_transposed(GraphWeighted*, GraphWeighted*, CompactBicliqueWeighted*);
    std::vector<std::pair<uInt,uInt>>* vector_matrix_multiplication_w(std::vector<std::pair<uInt, uInt>>, GraphWeighted*); 
    std::vector<std::pair<uInt,uInt>>* vector_matrix_t_multiplication_w(std::vector<std::pair<uInt, uInt>>, GraphWeighted*);
    std::vector<std::vector<std::pair<uInt,uInt>>*>* compute_compact_struct_weight(CompactBicliqueWeighted*, GraphWeighted*, bool);  

    Graph* matrix_multiplication(Graph*, Graph*);
    Graph* matrix_multiplication(Graph*, Graph*, Biclique*);
    std::vector<uInt>* vector_matrix_multiplication(std::vector<uInt>, Graph*);

}

#endif