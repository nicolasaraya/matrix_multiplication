#include <matrix.hpp>
#include <biclique.hpp>
#include <multiplicator.hpp>
#include <multiplicator_boolean.hpp>
#include <matrix_boolean.hpp>
#include <include/Utils/DebugSystem.hpp>

#include <iostream>

int main(int argc, char const *argv[])
{
	std::cout << "main" << std::endl;
	TIMERSTART(TOTAL);

  TIMERSTART(BUILD);
  Boolean::Matrix m(argv[1]);
  TIMERSTOP(BUILD);
  TIMERSTART(BUILD2);
  Boolean::Matrix m2(argv[2]);
  TIMERSTOP(BUILD2);
  TIMERSTART(TIME_MULT);
  auto res = Boolean::mat_mult(&m, &m2);
  TIMERSTOP(TIME_MULT);
  res->saveTxt(argv[1] + std::string(".pow"));
  delete res; 


  return 0;

  if (argc == 2) {
		TIMERSTART(BUILD);
		Boolean::Matrix m(argv[1]);
		TIMERSTOP(BUILD);
		TIMERSTART(TIME_POW);
		auto res = Boolean::mat_pow(&m);
		TIMERSTOP(TIME_POW);
		res->saveTxt("res.txt");
		delete res; 
	} else if (argc == 3) {
		TIMERSTART(BUILD_MATRIX);
		Boolean::Matrix m(argv[1]);
		TIMERSTOP(BUILD_MATRIX);
    
    msg(3) << "CSR:" << msgEndl;
    m.get_csr()->print();
    msg(3) << "CSR as list:" << msgEndl;
    m.get_csr()->printAsList();
    msg(3) << "CSC:" << msgEndl;
    m.get_csc()->print();

		TIMERSTART(BUILD_BICLIQUES)
		Boolean::Biclique b(argv[2]);
		TIMERSTOP(BUILD_BICLIQUES);

    msg(3) << "bicliques CSR:" << msgEndl;
    b.print_csr();
    msg(3) << "bicliques CSR:" << msgEndl;
    b.print_csc();

    //return 0;

		TIMERSTART(TIME_POW_BICLIQUES);
		auto res = Boolean::mat_pow(&m, &b);
		TIMERSTOP(TIME_POW_BICLIQUES);
		//res->get_csr()->print();
		res->saveTxt("res.txt");
		delete res; 
	}

  #if 0
	if (argc == 2) {
		TIMERSTART(BUILD);
		weighted::Matrix m(argv[1], true, true);
		TIMERSTOP(BUILD);
		TIMERSTART(TIME_POW);
		auto res = weighted::mat_pow(&m);
		TIMERSTOP(TIME_POW);
		res->saveTxt("res.txt");
		delete res; 
	} else if (argc == 3) {
		TIMERSTART(BUILD_MATRIX);
		weighted::Matrix m(argv[1], true, true);
		TIMERSTOP(BUILD_MATRIX);
		TIMERSTART(BUILD_BICLIQUES)
		weighted::Biclique b(argv[2]);
		TIMERSTOP(BUILD_BICLIQUES);
		TIMERSTART(TIME_POW_BICLIQUES);
		auto res = weighted::mat_pow(&m, &b);
		TIMERSTOP(TIME_POW_BICLIQUES);
		//res->get_csr()->print();
		res->saveTxt("res.txt");
		delete res; 
	}
  #endif

	TIMERSTOP(TOTAL);

	return 0;
}


/*
int main() {
		
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
	
}
*/