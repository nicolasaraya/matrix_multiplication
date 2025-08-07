#include "biclique_boolean.hpp"
#include "matrix_boolean.hpp"
#include "multiplicator_boolean.hpp"
#include "Utils.hpp"

#include <iostream>
#include <cstdlib>
#include <new>

#if MEMORY
void* operator new(std::size_t size)
{
  void* ptr = std::malloc(size + sizeof(std::size_t));
  if (not ptr) {
    throw std::bad_alloc();
  }
  *reinterpret_cast<std::size_t*>(ptr) = size;
  utils::MemoryTracker::AddAllocation(size);
  return static_cast<char*>(ptr) + sizeof(std::size_t);
}

void operator delete(void* memory) noexcept
{
  if (not memory) {
    return;
  }
  void* realPtr = static_cast<char*>(memory) - sizeof(std::size_t);
  std::size_t size = *reinterpret_cast<std::size_t*>(realPtr);
  utils::MemoryTracker::RemoveAllocation(size);
  std::free(realPtr);
}

// Opcional: también soportar new[] y delete[]
void* operator new[](std::size_t size)
{
  return ::operator new(size);
}

void operator delete[](void* memory) noexcept
{
  ::operator delete(memory);
}
#endif

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
  delete AxA;
  delete Axb;
  auto* join2 = csr_add(bxA, bxb);
  delete bxA;
  delete bxb;
  auto* join3 = csr_add(join, join2);
  delete join;
  delete join2;
  TIMERSTOP(join);
  TIMERSTOP(TOTAL);

  Matrix res;
  res.set_csr(join3);

  #if DEBUG
  join3->print();
  join3->printAsList();
  #else
  auto newPath = utils::modify_path(pathMatrix, "_powBic.txt");
  res.saveTxt(newPath);
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
  auto* bxAinters = compute_intersections(biclique, matrix->get_csr());
  TIMERSTOP(bxA);
  matrix->delete_csr();
  delete matrix;

  TIMERSTART(bxb);
  auto* bxbinter = compute_intersections(biclique, biclique);
  TIMERSTOP(bxb);
  TIMERSTOP(total_operations);
  
  TIMERSTART(join);
  outBiclique = biclique_add(biclique, biclique, bxAinters, bxbinter);
  auto* join = csr_add(AxA, Axb);
  delete AxA;
  delete Axb;
  delete biclique;

  TIMERSTOP(join);
  TIMERSTOP(TOTAL);

  outMatrix->set_csr(join);

  #if DEBUG
  join->print();
  join->printAsList();
  #else
  auto newPath = utils::modify_path(pathMatrix, "_powBic_cm.txt");
  outMatrix->saveTxt(newPath);
  auto newPathBic = utils::modify_path(pathBicliques, "_powBic_cb.txt");
  outBiclique->saveTxt(newPathBic);
  #endif
  return;
}

void pow(Matrix* matrix)
{
  #if DEBUG
  matrix->get_csr()->print();
  matrix->get_csc()->print();
  #endif

  std::cout << "Starting pow" << std::endl;

  std::string originalPath = matrix->getPath();
  TIMERSTART(AxA);
  auto* AxA = mult(matrix->get_csc(), matrix->get_csr());
  TIMERSTOP(AxA);
  delete matrix;

  Matrix res;
  res.set_csr(AxA);

  #if DEBUG
  AxA->print();
  AxA->printAsList();
  #else

  auto newPath = utils::modify_path(originalPath, "_pow.txt");
  res.saveTxt(newPath);
  #endif
}

void multiply(char const *argv[])
{
  std::cout << "Not implemented yet" << std::endl;
  return;
}

int main(int argc, char const *argv[])
{
  {
    std::cout << "Arguments:";
    for (size_t i = 1; i < argc; ++i) {
      std::cout << " " << argv[i];
    }
    std::cout << std::endl;

    Matrix* matrix = argc > 1 ? new Matrix(argv[1]) : nullptr;
    Biclique* biclique = argc > 2 ? new Biclique(argv[2]) : nullptr;

    switch (argc) {
      case 2:
        pow(matrix);
        break;
      case 3: 
      {
        //powBicl(matrix, biclique);
        auto *C = new Matrix();
        auto *b = new Biclique();
        powBicl(matrix, biclique, C, b);

        C->make_csc();
        b->make_csc();
        powBicl(C, b);

        //delete C;
        //delete b;
        break;
      }
      case 5:
        multiply(argv);
        break;
      default:
        break;
    }
  }
 
  #if MEMORY
  utils::MemoryTracker::Report();
  #endif

  return 0;
}
