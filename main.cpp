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

void powBicl(char const *argv[])
{
  Matrix* matrix = new Matrix(argv[1]);
  Biclique* biclique = new Biclique(argv[2]);

  #if DEBUG
  matrix->get_csr()->print();
  matrix->get_csc()->print();
  biclique->print_csr();
  biclique->print_csc();
  #endif

  std::cout << "Starting pow with bicliques" << std::endl;
  TIMERSTART(TOTAL);
  TIMERSTART(total_operations);
  TIMERSTART(AxA);
  auto* AxA = compute_intersections(matrix->get_csc(), matrix->get_csr());
  TIMERSTOP(AxA);
  
  #if DEBUG
  AxA->print();
  AxA->printAsList();
  #endif

  TIMERSTART(Axb);
  auto* Axb = compute_intersections(matrix->get_csc(), biclique);
  TIMERSTOP(Axb);
  matrix->delete_csc();

  #if DEBUG
  Axb->print();
  Axb->printAsList();
  #endif

  TIMERSTART(bxA);
  auto* bxA = compute_intersections(biclique, matrix->get_csr());
  TIMERSTOP(bxA);
  matrix->delete_csr();
  delete matrix;

  #if DEBUG
  bxA->print();
  bxA->printAsList();
  #endif

  TIMERSTART(bxb);
  auto* bxb = compute_intersections(biclique, biclique);
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

  #if DEBUG
  join3->print();
  join3->printAsList();
  #else
  auto newPath = utils::modify_path(argv[1], "_powBic.txt");
  Matrix res;
  res.set_csr(join3);
  res.saveTxt(newPath);
  #endif

}

void pow(char const *argv[])
{
  Matrix* matrix = new Matrix(argv[1]);

  #if DEBUG
  matrix->get_csr()->print();
  matrix->get_csc()->print();
  #endif

  std::cout << "Starting pow" << std::endl;

  TIMERSTART(AxA);
  auto* AxA = compute_intersections(matrix->get_csc(), matrix->get_csr());
  TIMERSTOP(AxA);

  #if DEBUG
  AxA->print();
  AxA->printAsList();
  #endif

  #if DEBUG
  AxA->print();
  AxA->printAsList();
  #else
  auto newPath = utils::modify_path(argv[1], "_pow.txt");
  Matrix res;
  res.set_csr(AxA);
  res.saveTxt(newPath);
  #endif
}

void mult(char const *argv[])
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

    switch (argc) {
    case 2:
      pow(argv);
      break;
    case 3:
      powBicl(argv);
      break;
    case 5:
      mult(argv);
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
