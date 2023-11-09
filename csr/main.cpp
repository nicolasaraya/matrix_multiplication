#include "csr_multiplication.hpp"

int main(int argc, char const *argv[])
{
    using namespace csr;
    if (argc == 2) {
        std::cout << "file: " << argv[1] << std::endl;

        GraphWeighted* a = new GraphWeighted(argv[1]);
        GraphWeighted* b = new GraphWeighted(argv[1]);
        b->transpose();

        TIMERSTART(BUILD_A);
        auto a_csr = graphWeightedToSparseMatrix(a);
        delete a;
        TIMERSTOP(BUILD_A);
        TIMERSTART(BUILD_AT);
        auto at_csr = graphWeightedToSparseMatrix(b);
        delete b;
        TIMERSTOP(BUILD_AT);

        //print_csr(a_csr);

        TIMERSTART(multiplication_csr);
        auto c_csr = multiplySMxSMt(a_csr, at_csr);
        TIMERSTOP(multiplication_csr);

        //print_csr(c_csr);
        auto res = csrToGraphWeighted(c_csr);





        delete at_csr;
        delete a_csr;
        delete c_csr;
        delete res;


    }
    return 0;
}
