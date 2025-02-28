#include <matrix.hpp>
#include <biclique.hpp>
#include <multiplicator.hpp>
#include <multiplicator_boolean.hpp>
#include <matrix_boolean.hpp>

#include <iostream>

int main(int argc, char const *argv[])
{

	std::cout << "main" << std::endl;

	TIMERSTART(TOTAL);

  if (argc == 2) {
		TIMERSTART(BUILD);
		boolean::Matrix m(argv[1]);
		TIMERSTOP(BUILD);
		TIMERSTART(TIME_POW);
		auto res = boolean::mat_pow(&m);
		TIMERSTOP(TIME_POW);
		res->saveTxt("res.txt");
		delete res; 
	} else if (argc == 3) {
		TIMERSTART(BUILD_MATRIX);
		boolean::Matrix m(argv[1]);
		TIMERSTOP(BUILD_MATRIX);
		TIMERSTART(BUILD_BICLIQUES)
		boolean::Biclique b(argv[2]);
		TIMERSTOP(BUILD_BICLIQUES);
		TIMERSTART(TIME_POW_BICLIQUES);
		auto res = boolean::mat_pow(&m, &b);
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