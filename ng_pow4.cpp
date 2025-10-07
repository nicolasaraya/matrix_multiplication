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

int main(int argc, char const *argv[])
{
  {
    std::cout << "Arguments:";
    for (size_t i = 1; i < argc; ++i) {
      std::cout << " " << argv[i] << std::endl;
    }

    std::cout << "Using CM, CB and compute Pow4 as CSR Matrix" << std::endl;

    Matrix* matrix = argc > 1 ? new Matrix(argv[1]) : nullptr;
    Biclique* biclique = argc > 2 ? new Biclique(argv[2]) : nullptr;

    switch (argc) {
      case 2:
        pow(matrix);
        break;
      case 3: 
      {
        Biclique* b = nullptr;
        Matrix* C = nullptr;
        powBicl(matrix, biclique, C, b);
        b->make_csc();
        C->make_csc();
        powBicl(C,b);
        break;
      }
      default:
        break;
    }
  }
 
  #if MEMORY
  utils::MemoryTracker::Report();
  #endif

  return 0;
}
