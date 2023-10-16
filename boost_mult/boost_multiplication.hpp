#ifndef BOOST_MULTIPLICATION
#define BOOST_MULTIPLICATION

#include <iostream>
#include <string.h>
#include <fstream>

#include <boost/numeric/ublas/matrix.hpp>

#include "../biclique_extraction/Utils/Utils.hpp"
#include "matrix_multiplication.hpp"

namespace boost {
    namespace ublas = boost::numeric::ublas;
    void fillZero (ublas::matrix<uInt>*); 
    void normalizeSize(ublas::matrix<uInt>*, ublas::matrix<uInt>*);
    ublas::matrix<uInt> createMatrix(std::string);
    void printMatrix(ublas::matrix<uInt>&);
    GraphWeighted* buildGraphFromMatrix(ublas::matrix<uInt>*);
}

#endif