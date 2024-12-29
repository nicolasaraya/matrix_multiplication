#include <multiplicator_boolean.hpp>
#include <matrix_boolean.hpp>


int main(int argc, char const *argv[])
{
    TIMERSTART(BUILD);
    auto b = new boolean::Matrix(argv[1]);
    TIMERSTOP(BUILD);


    //b->get_csr()->print();
    //b->get_csc()->print();

    TIMERSTART(POW);
    auto res = boolean::mat_mult(b,b);
    TIMERSTOP(POW);
    //res->get_csr()->print();
    res->saveTxt("res.txt");
    delete res;
    delete b;

    return 0;
}
