#include "csr_multiplication.hpp"

namespace csr 
{
    void print_csr(SparseMatrix* matrix) {
        std::cout << std::endl <<"Rowptr: ";
        for (auto i : matrix->row_ptr) {
            std::cout << i << " ";
        }
    
        std::cout << std::endl << "Col: " ;
        for (auto i : matrix->col) {
            std::cout << i << " "; 
        }

        std::cout << std::endl <<"Val: ";
        for (auto i : matrix->val) {
            std::cout << i << " ";
        }

        std::cout << std::endl;
        
    }
        


    SparseMatrix* graphWeightedToSparseMatrix(GraphWeighted* graph) 
    {
        SparseMatrix* res = new SparseMatrix(); 
        res->row_ptr.push_back(0);
        res->row_ptr.push_back(0);

        
        for (auto node = graph->begin(); node != graph->end(); node++) {
            for(auto adj = (*node)->wAdjacentsBegin(); adj != (*node)->wAdjacentsEnd(); adj++) {
                res->col.push_back(uInt((*adj).first));
                res->val.push_back(uInt((*adj).second));
            }
            res->row_ptr.push_back(res->row_ptr.back()+(*node)->edgesSize());
        }
        
        return res; 
    }

    SparseMatrix* multiplySMxSMt(SparseMatrix* A, SparseMatrix* B)
    {
        cout << "sizeof: " << sizeof(uInt) << endl;
        SparseMatrix* res = new SparseMatrix();
        res->row_ptr.push_back(0);

        long long int rowsA = A->row_ptr.size() - 1;
        long long int rowsB = B->row_ptr.size() - 1;

        for (size_t i = 0; i < rowsA; i++) {
            for (size_t j = 0 ; j < rowsB; j++) {
                long long int sum = 0;
                uInt indexA = A->row_ptr[i];
                uInt indexB = B->row_ptr[j]; 
                while (indexA < A->row_ptr[i+1] and indexB < B->row_ptr[j+1]) {
                    if (A->col[indexA] < B->col[indexB]) {
                        indexA++;
                    } else if (A->col[indexA] > B->col[indexB]) {
                        indexB++;
                    } else {
                        sum+= A->val[indexA] * B->val[indexB];
                        indexA++;
                        indexB++;
                    }
                }
                if(sum>0){
                    res->val.push_back(sum);
                    res->col.push_back(j);
                }   
                
            }
            res->row_ptr.push_back(res->col.size());
        }

        return res;
    }

    GraphWeighted* csrToGraphWeighted(SparseMatrix* matrix)
    {
        GraphWeighted* res = new GraphWeighted(); 

        size_t numRows = matrix->row_ptr.size() - 1;

        for (size_t i = 0; i < numRows; i++) {
            //std::cout << i << ": ";
            Node* n = new Node(i, true);
            res->insert(n);
            for(size_t j = matrix->row_ptr[i]; j < matrix->row_ptr[i + 1]; j++) {
                uInt col = matrix->col[j];
                uInt val = matrix->val[j];
                //std::cout << "(" << col << "," << val << ") ";
                n->addAdjacent(col, val);
            }
            //std::cout << std::endl;
        }
        return res;
    }
};