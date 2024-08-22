#include <matrix_boolean.hpp>

namespace boolean
{
	Matrix::Matrix() {;}

	Matrix::Matrix(std::string path) : path(path) 
	{
		make_csr();
		make_csc();
	}

	Matrix::~Matrix() 
	{
		delete_csc();
		delete_csr();
	}

	void Matrix::setFile(std::string path) 
	{
		this->path = path;
	}

	csr_matrix* Matrix::make_csr() 
	{
		std::ifstream file;
		file.open(path);
		assert(file.is_open());
		std::string line;
		uint32_t id;
		getline(file, line); // num nodes
		uint32_t num_nodes = atoi(line.c_str());

		csr = new csr_matrix();
		csr->row_ptr.push_back(0);
		//csr->col_ind.resize(num_nodes, 0);

		while (!file.eof()) {
			getline(file, line);
			auto adjacents = utils::splitString(line, " ");

			if (adjacents.empty()) {
				continue;
			}
			
			id = atoi(adjacents.at(0).c_str());

			for (size_t i = 1; i < adjacents.size();i++) {
				csr->col_ind.push_back(atoll(adjacents[i].c_str()));
			}
			csr->row_id.push_back(id);
			csr->row_ptr.push_back(csr->col_ind.size());
		}
		csr->max_row = csr->row_id.back();
		file.close();
		return csr;
	}

	csc_matrix* Matrix::make_csc() 
	{
		csc = new csc_matrix();
		csc->col_ptr.push_back(0);
		csc->row_ind.resize(csr->col_ind.size(), 0);

		std::unordered_map<uint32_t, uint32_t> cols;

		for (size_t i = 0; i < csr->col_ind.size(); i++) {
			cols[csr->col_ind[i]]++;
		}

		for (size_t i = 0; i < cols.size(); i++) {
			if (cols[i] > 0) {
				csc->col_id.push_back(i);
				uint32_t temp = csc->col_ptr.back();
				csc->col_ptr.push_back(csc->col_ptr.back() + cols[i]);
				cols[i] = temp;
			}
		}

		for (size_t i = 0; i < csr->row_id.size(); i++) {
			size_t start = csr->row_ptr[i];
			size_t stop = csr->row_ptr[i+1];

			for (size_t j = start; j < stop; j++) {
				csc->row_ind[cols[csr->col_ind[j]]] = csr->row_id[i];
				cols[csr->col_ind[j]]++;
			}
		}

		cols.clear();

		csc->max_col = csc->col_id.back();
		return csc;
	}

	csr_matrix* Matrix::get_csr()
	{
		return csr;
	}

	csc_matrix* Matrix::get_csc()
	{
		return csc;
	}

	void Matrix::set_csr(csr_matrix* mat) 
	{
		this->csr = mat;
	}

	void Matrix::set_csc(csc_matrix* mat)
	{
		this->csc = mat; 
	}

	void Matrix::delete_csr()
	{
		if (csr != nullptr) delete csr;
	}

	void Matrix::delete_csc()
	{
		if (csc != nullptr) delete csc; 
	}

	void Matrix::saveTxt()
	{
		saveTxt(path);
	}

	void Matrix::saveTxt(std::string pathFile)
	{
		std::ofstream file;
		file.open(pathFile, std::ofstream::out | std::ofstream::trunc); // limpia el contenido del fichero
		file << csr->col_ind.size() << std::endl;
		for (size_t i = 0; i < csr->row_id.size(); i++) {
			file << csr->row_id[i] << ":";
			size_t start = csr->row_ptr[i];
			size_t stop = csr->row_ptr[i+1];

			while (start != stop) {
				file << " " << csr->col_ind[start] ;
				start++;
			}
			file << std::endl;

		}
		file.close();
	}



	
};
