#include <matrix.hpp>
#include <multiplicator.hpp>

#include <iostream>

int main(int argc, char const *argv[])
{

    std::cout << "main" << std::endl;

    /*
    auto csr = new csr_matrix();

    csr->values.assign({2,3,4,5,6,
                        1,3,4,5,
                        2,4,5,6,
                        1,3,6,
                        2,4,6,
                        1,3,4,5
                        });

    csr->col_ind.assign({2,3,4,5,6,
                        1,3,4,5,
                        2,4,5,6,
                        1,3,6,
                        2,4,6,
                        1,3,4,5
                        });

    csr->row_ptr.assign({0,5,9,13,16,19});

    csr->row_id.assign({1,2,3,4,5,6});

    std::cout << "CSR 1:" << std::endl;
    csr->print();

    std::cout << std::endl << std::endl;
    */

    /*
    Matrix m("test.txt", true, false);
    auto csr2 = m.get_csr();
    std::cout << "CSR 2:" << std::endl;../datasets/100_dblp/100_dblp_w.txt
    csr2->print();

    std::cout << std::endl;
    std::cout << "CSC:" << std::endl;
    auto csc = m.make_csc();
    csc->print();

    auto res = mat_pow(&m);
    res->get_csr()->print();
    */

    if (argc == 2) {
        TIMERSTART(BUILD);
        Matrix m(argv[1], true, true);
        TIMERSTOP(BUILD);
        //m.get_csr()->print();
        //m.get_csc()->print();

        TIMERSTART(TIME_POW);
        auto res = mat_pow(&m);
        TIMERSTOP(TIME_POW);

        res->saveTxt("res.txt");
        //res->get_csr()->print();
        delete res; 
    }

    return 0;
}
