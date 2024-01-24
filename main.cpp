#include <matrix.hpp>
#include <biclique.hpp>
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

    /*

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



    */

    Matrix a(argv[1]);
    std::cout << "csr A: " << std::endl;
    a.get_csr()->print();
    std::cout << "csc A: " << std::endl;
    a.get_csc()->print();

    std::cout << std::endl << std::endl;


    Biclique b(argv[2]);
    std::cout << "csr b: " << std::endl;
    b.get_csr()->print();
    std::cout << "csc b: " << std::endl;
    b.get_csc()->print();
    std::cout << std::endl << std::endl;



    auto resAxA = csr_mult(a.get_csc(), a.get_csr());
    std::cout << "AxA: " << std::endl; 
    resAxA->print();
    std::cout << std::endl;

    auto resAxB = csr_mult(a.get_csc(), b.get_csr()); //A X B ? 
    std::cout << "AxB: " << std::endl; 
    resAxB->print();
    std::cout << std::endl;

    auto resBxA = csr_mult(b.get_csc(), a.get_csr()); //B X A 
    std::cout << "BxA: " << std::endl; 
    resBxA->print();
    std::cout << std::endl;

    auto resBxB = csr_mult(b.get_csc(), b.get_csr()); //B X B
    std::cout << "BxB: " << std::endl; 
    resBxB->print();
    std::cout << std::endl;



    auto add = csr_add(resAxA, resAxB);
    auto add2 = csr_add(resBxA, resBxB);
    auto add3 = csr_add(add, add2);

    std::cout << "AxA + AxB: " << std::endl; 
    add->print();
    std::cout << std::endl;

    std::cout << "BxA + BxB: " << std::endl; 
    add2->print();
    std::cout << std::endl;

    std::cout << "RESPOW: " << std::endl; 
    add3->print();
    std::cout << std::endl;

    delete resAxA;
    delete resAxB;
    delete resBxA;
    delete resBxB;
    delete add;
    delete add2;
    delete add3;



    return 0;
}
