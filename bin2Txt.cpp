#include "biclique_boolean.hpp"
#include "matrix_boolean.hpp"
#include "multiplicator_boolean.hpp"
#include "Utils.hpp"

#include <iostream>
#include <cstdlib>
#include <new>

int main(int argc, char const *argv[])
{
  {
    std::cout << "Arguments:";
    for (size_t i = 1; i < argc; ++i) {
      std::cout << " " << argv[i] << std::endl;
    }

    std::cout << "Transformer Bin to Txt Matrix" << std::endl;

    Matrix* matrix = argc > 1 ? new Matrix(argv[1]) : nullptr;

    if (not matrix) {
      std::cerr << "Error: No matrix file provided." << std::endl;
      return 1;
    }

    auto newPath = utils::modify_path(matrix->getPath(), 4, ".txt");
    matrix->saveTxt(newPath);

    delete matrix;
  }
 
  return 0;
}
