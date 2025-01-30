#ifndef BUILDER_BOOLEAN
#define BUILDER_BOOLEAN

#include <biclique_boolean.hpp>
#include <matrix_boolean.hpp>
#include <include/Graph/Graph.hpp>

namespace Boolean
{
  class Builder
  {
    public:
      static csr_matrix* to_csr(Graph* m);
      static csc_matrix* to_csc(Graph* m);
      static Biclique* to_Biclique();
    private:
      Builder() = delete;
  };
} // namespace boolean


#endif