#include "boost_multiplication.hpp"

namespace boost_mult {
    void fillZero (ublas::matrix<uInt>* matrix) 
    {
        for (size_t i = 0; i < matrix->size1(); i++) {
            for(size_t j = 0; j <matrix->size2(); j++) {
                matrix->insert_element(i, j, 0);
            }
            
        }
    }

    void normalizeSize(ublas::matrix<uInt>* matrixA, ublas::matrix<uInt>* matrixB) 
    {
        assert(matrixA->size1() == matrixA->size2());
        assert(matrixB->size1() == matrixB->size2());

        if (matrixA->size1() > matrixB->size1()) {
            size_t t = matrixB->size1();
            matrixB->resize(matrixA->size1(), matrixA->size1(), true); 

            for (size_t i = 0; i < matrixB->size1(); i++){
                if (i >= t) {
                    for (size_t j = 0; j < matrixB->size1(); j++) {
                        matrixB->insert_element(i, j, 0);
                    }
                    continue;
                }

                for (size_t j = t ; j < matrixB->size1(); j++){
                    matrixB->insert_element(i, j, 0);
                }
            }
        } else if (matrixB->size1() > matrixA->size1()) {
            size_t t = matrixA->size1();
            matrixA->resize(matrixB->size1(), matrixB->size1(), true); 

            for (size_t i = 0; i < matrixA->size1(); i++){
                if (i >= t) {
                    for (size_t j = 0; j < matrixA->size1(); j++) {
                        matrixA->insert_element(i, j, 0);
                    }
                    continue;
                }

                for (size_t j = t ; j < matrixA->size1(); j++){
                    matrixA->insert_element(i, j, 0);
                }
            }
        }
    }


    ublas::matrix<uInt> createMatrix(std::string path) 
    {
        auto temp = new GraphWeighted(path);

        size_t maxRow = temp->back()->getId() + 1;
        size_t maxCol = temp->maxValueEdge() + 1;

        std::cout << "maxRow: " << maxRow << std::endl;
        std::cout << "maxCol: " << maxCol << std::endl;

        if(maxRow > maxCol) { 
            maxCol = maxRow;
        } else {
            maxRow = maxCol;
        }

        //temp->print();

        //auto matrix = new ublas::matrix<uInt>(maxRow + 2, maxCol + 2);
        ublas::matrix<uInt> matrix(maxRow, maxCol);
        std::cout << matrix.size1() << " " << matrix.size2() << std::endl;

        auto itNode = temp->begin(); 

        for (size_t i = 0; i < maxRow; i++) {
            for(size_t j = 0; j < maxCol; j++) {
                matrix.insert_element(i, j, 0);
            }
            if (i == (*itNode)->getId()) {
                for(auto j = (*itNode)->wAdjacentsBegin(); j != (*itNode)->wAdjacentsEnd(); j++) {
                    matrix.insert_element(i, (*j)->first, (*j)->second);
                }
                itNode++;
            }
        }
        delete temp;

        return matrix; 
    }

     ublas::compressed_matrix<uInt> createMatrixSparse(std::string path) 
    {
        auto temp = new GraphWeighted(path);

        size_t maxRow = temp->back()->getId() + 1;
        size_t maxCol = temp->maxValueEdge() + 1;

        std::cout << "maxRow: " << maxRow << std::endl;
        std::cout << "maxCol: " << maxCol << std::endl;

        if(maxRow > maxCol) { 
            maxCol = maxRow;
        } else {
            maxRow = maxCol;
        }

        //temp->print();

        //auto matrix = new ublas::matrix<uInt>(maxRow + 2, maxCol + 2);
        ublas::compressed_matrix<uInt> matrix(maxRow, maxCol);
        std::cout << matrix.size1() << " " << matrix.size2() << std::endl;

        auto itNode = temp->begin(); 

        for (size_t i = 0; i < maxRow; i++) {
            for(size_t j = 0; j < maxCol; j++) {
                matrix.insert_element(i, j, 0);
            }
            if (i == (*itNode)->getId()) {
                for(auto j = (*itNode)->wAdjacentsBegin(); j != (*itNode)->wAdjacentsEnd(); j++) {
                    matrix.insert_element(i, (*j)->first, (*j)->second);
                }
                itNode++;
            }
        }
        delete temp;

        return matrix; 
    }

    GraphWeighted *buildGraphFromMatrix(ublas::matrix<uInt> *matrix)
    {
        auto graph = new GraphWeighted();

        for (size_t i = 0; i < matrix->size1(); i++) {
            auto tempNode = new Node(i);
            for (size_t j = 0; j < matrix->size2(); j++) {
                if (matrix->at_element(i,j) > 0) {
                    tempNode->addAdjacent(j, matrix->at_element(i,j));
                }
            } 
            if (tempNode->edgesSize() > 0) {
                graph->insert(tempNode);
            } else {
                delete tempNode;
            }
        }
        return graph;
    }

    void printMatrix(ublas::matrix<uInt>& matrix) 
    {
        for (size_t i = 0; i < matrix.size1(); i++) {
            for (size_t j = 0; j < matrix.size2(); j++) {
                std::cout << matrix(i, j) << " "; 
            }
            std::cout << std::endl;
        }
    }
}