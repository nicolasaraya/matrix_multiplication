#ifndef MATRIX_MULTIPLICATION
#define MATRIX_MULTIPLICATION

#include "../biclique_extraction/Graph/Graph.hpp"
#include "../biclique_extraction/Graph/GraphWeighted.hpp"
#include "../biclique_extraction/Graph/Node.hpp"
#include "../biclique_extraction/Utils/Utils.hpp"

#include <vector>
#include <algorithm>
#include <string.h>
#include <omp.h>


namespace Lisa {
    struct Biclique{
        std::vector<uInt> S;
        std::vector<uInt> C;
        std::vector<std::pair<uInt,uInt>> C_w; 
    };

    GraphWeighted* matrix_multiplication_w(GraphWeighted*, GraphWeighted*);
    GraphWeighted* matrix_multiplication_w(GraphWeighted*, GraphWeighted*, std::vector<Biclique*>*);
    std::vector<std::pair<uInt,uInt>>* vector_matrix_multiplication_w(std::vector<std::pair<uInt, uInt>>, GraphWeighted*); 

    Graph* matrix_multiplication(Graph*, Graph*);
    Graph* matrix_multiplication(Graph*, Graph*, Biclique*);
    std::vector<uInt>* vector_matrix_multiplication(std::vector<uInt>, Graph*);

    vector<Biclique*>* load_biclique_w(std::string);
    void printBicliqueAsMatrix(Biclique*);
}
#endif