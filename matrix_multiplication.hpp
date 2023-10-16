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
#include <mutex>
#include <cassert>

//#define parallel

#if defined(parallel)
    #define THREADS 4
#endif

#define debug 0

namespace mw
{
    typedef std::vector<std::pair<uInt, uInt>> C_Values;

    struct Biclique 
    {
        std::vector<uInt>* S;
        C_Values* C; 
        
        Biclique()
        {
            S = new std::vector<uInt>();
            C = new C_Values(); 
        }

        void setC(C_Values* newC)
        {
            delete C;
            C = newC; 
        }
        
        void setS(std::vector<uInt>* newS) 
        {
            delete S;
            S = newS; 
        } 

        ~Biclique()
        {
            //if (C != nullptr) delete C;
            if (S != nullptr) delete S;
        }
    };

    struct CompactBiclique
    {
        typedef std::pair<uInt, std::vector<uInt>> S_values;
        std::vector<C_Values*> c_collection; 
        std::vector<Biclique*> biclique_collection; 
        std::vector<S_values> linked_s; //S_value to c_collection index 

        ~CompactBiclique()
        {
            while(not c_collection.empty()) {
                if (c_collection.back() != nullptr) delete c_collection.back();
                c_collection.pop_back();
            }
            while(not biclique_collection.empty()) {
                if (biclique_collection.back() != nullptr) delete biclique_collection.back();
                biclique_collection.pop_back();
            }

            c_collection.clear();
            biclique_collection.clear();
            linked_s.clear();
        }  
    };

    std::vector<Biclique*>* load_biclique(std::string);
    std::vector<Biclique*>* load_biclique_bin(std::string);

    CompactBiclique* load_CompactBiclique(std::string);
    CompactBiclique* load_CompactBiclique_bin(std::string);

    GraphWeighted* transpose_CompactBiclique(CompactBiclique*);

    void printBicliqueAsMatrix(Biclique*);
    void printCompactStructure(CompactBiclique*);


    GraphWeighted* matrix_multiplication(GraphWeighted*, GraphWeighted*, std::vector<Biclique*>*);
    GraphWeighted* matrix_multiplication(GraphWeighted*, GraphWeighted*, CompactBiclique*); 

    GraphWeighted* matrix_pow(GraphWeighted*, CompactBiclique*);
    
    GraphWeighted* matrix_multiplication(GraphWeighted*, GraphWeighted*); //standard transpose multiplication
    
    std::vector<std::pair<uInt,uInt>>* vector_matrix_multiplication(std::vector<std::pair<uInt, uInt>>*, GraphWeighted*); //for std_bicl optimized
    
    std::vector<std::vector<std::pair<uInt,uInt>>*>* compute_compact_struct(CompactBiclique*, GraphWeighted*);  // b x A'
    
    GraphWeighted* compute_compact_struct(GraphWeighted*, CompactBiclique*, GraphWeighted* res);  // b x A'

    void appendInGraph(std::vector<std::vector<std::pair<uInt,uInt>>*>*, CompactBiclique*, GraphWeighted*);
    void mergeGraph(GraphWeighted*, GraphWeighted*);

    GraphWeighted* biclique_pow(CompactBiclique*, GraphWeighted*);

    
    std::vector<std::pair<uInt, uInt>>* checkIntersection(Biclique*, Biclique*);


    typedef std::vector<std::vector<std::pair<uInt,uInt>>*> resArray;
    typedef std::vector<std::pair<uInt,uInt>>::iterator Iter;

    // A x A = (A)^2 
    // A = A' + b
    // A x A = (A')^2 + A'x b + b x A' + (b)^2
}

#endif