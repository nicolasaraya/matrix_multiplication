#include "matrix.hpp"

Matrix::Matrix(){}

Matrix::Matrix(std::string path, bool b_csr, bool b_csc)
{
    setFile(path);
    if (b_csr) {
        if(path.find(".txt" ) != std::string::npos) {
            make_csr();
	    } else if(path.find(".bin") != std::string::npos) {
		    make_csr_bin();
	    }
    }
    if (b_csc) make_csc();
}

void Matrix::setFile(std::string path)
{
    this->path = path; 
}

csr_matrix* Matrix::make_csr()
{
    TIMERSTART(BUILD_CSR);
    csr = new csr_matrix();
    std::ifstream file;
    file.open(path);
    assert(file.is_open());
    std::string s;
	while (getline(file, s)) {
        //std::cout << s << std::endl;
        auto values = utils::splitString(s, " ");

        csr->values.push_back(atoi(values[2].c_str()));
        csr->col_ind.push_back(atoi(values[1].c_str()));
        
        if (csr->row_id.empty() or csr->row_id.back() != unsigned(atoi(values[0].c_str()))) {
            csr->row_id.push_back(atoi(values[0].c_str()));
            csr->row_ptr.push_back(csr->col_ind.size()-1);
        }

	}
    csr->row_ptr.push_back(csr->col_ind.size());

	
	file.close();
    TIMERSTOP(BUILD_CSR);
    return csr;
}

csr_matrix* Matrix::make_csr_bin()
{
    csr = new csr_matrix();

    std::ifstream file;
	file.open(path, std::ios::in | std::ios::binary);
	file.seekg (0, file.beg);
	assert(file.is_open());

	int32_t* buffer = new int32_t(-1);

	while (not file.eof()) {
		file.read((char*)buffer, sizeof(int32_t));
		if(*buffer < 0) {
			csr->row_id.push_back(-(*buffer));
            csr->row_ptr.push_back(csr->col_ind.size());
		} else {
			//if(file.eof()) break;
			uint32_t adj = *buffer;
			file.read((char*)buffer, sizeof(uint32_t));
			uint32_t weight = *buffer;
            csr->col_ind.push_back(adj),
            csr->values.push_back(weight);
		}
		//temp->print();
		//if (matrix->size() > 400000 ) break;
	}
	delete buffer;
	file.close();;

    return csr;
}

csc_matrix* Matrix::make_csc()
{
    
    bool flag = false;
    if (csr == nullptr) {
        flag = true;
        csr = make_csr();
    }

    TIMERSTART(BUILD_CSC);

    csc = new csc_matrix();
    csc->col_ptr.push_back(0);
    csc->row_ind.resize(csr->values.size(), 0);
    csc->values.resize(csr->values.size(), 0);
    
    std::vector<uint32_t> cols(csr->col_ind.size(), 0); 

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
        size_t stop; 
        if (i == csr->row_id.size() - 1) {
            stop = csr->values.size();
        } else {
            stop = csr->row_ptr[i+1];
        }

        for (size_t j = start; j < stop; j++) {
            csc->row_ind[cols[csr->col_ind[j]]] = csr->row_id[i];
            csc->values[cols[csr->col_ind[j]]] = csr->values[j];
            cols[csr->col_ind[j]]++;
        }
    }

    cols.clear();

    if (flag) {
        delete csr; 
    }
    TIMERSTOP(BUILD_CSC);
    return csc;
}

csr_matrix *Matrix::get_csr()
{
    return csr;
}

csc_matrix *Matrix::get_csc()
{
    return csc;
}

void Matrix::set_csr(csr_matrix *mat)
{
    assert(csr == nullptr); 
    csr = mat; 
}

void Matrix::set_csc(csc_matrix *mat)
{
    assert(csc == nullptr);
    csc = mat;
}

void Matrix::delete_csr()
{
    delete csr;
    csr = nullptr;
}

void Matrix::delete_csc()
{
    delete csc;
    csc = nullptr;
}

void Matrix::saveTxt()
{
    std::string pathFile = utils::modify_path(path, 4 ,utils::now_time() + ".txt");
    saveTxt(pathFile);
}

void Matrix::saveTxt(std::string pathFile) {
    std::ofstream file;

	file.open(pathFile, std::ofstream::out | std::ofstream::trunc); 
	assert(file.is_open());


    for (size_t i = 0; i < csr->row_id.size(); i++) {
        size_t start = csr->row_ptr[i];
        size_t stop = csr->row_ptr[i+1];

        for (size_t j = start; j < stop; j++) {
            file << csr->row_id[i] << " " << csr->col_ind[j] << " " << csr->values[j] << std::endl;
        }
    }
    
}



    /*
    //v2
    csc = new csc_matrix();
    csc->col_ptr.push_back(0);


    std::set<uint32_t> cols;

    for (auto i : csr->col_ind) {
        cols.insert(i);
    }

    std::vector<uint32_t> starts = csr->row_ptr;

    //int it = 0;
    for (auto col_val : cols) {
        for (size_t i = 0; i < csr->row_ptr.size(); i++) {

            size_t start = starts[i];
            size_t finish;

            if (i == csr->row_ptr.size()-1) finish = csr->col_ind.size();
            else finish = csr->row_ptr[i+1]; 

            //std::cout << "s: " << start << " , f:" << finish << std::endl;

            for (size_t j = start; j < finish; j++) {
                //std::cout << "val:" << csr->col_ind[j] << std::endl;

                if (csr->col_ind[j] == col_val) {
                    csc->row_ind.push_back(csr->row_id[i]);
                    csc->values.push_back(csr->values[j]);
                    
                    //break;
                }

                if (csr->col_ind[j] > col_val) {
                    break;
                }
                starts[i] = j;
                
                
            }    
        }
        if (col_val != *(cols.end())) {
            csc->col_ptr.push_back(csc->row_ind.size());
        }
        csc->col_id.push_back(col_val);
        //std::cout << "new it" << std::endl;
        //if (it++ > 10) break;
    }
    */

    /*
    //v1
    while (min <= max) {
        for (size_t i = 0; i < csr->row_ptr.size(); i++) {

            size_t start = csr->row_ptr[i];
            size_t finish;

            if (i == csr->row_ptr.size()-1) finish = csr->col_ind.size();
            else finish = csr->row_ptr[i+1]; 

            //std::cout << "s: " << start << " , f:" << finish << std::endl;

            for (size_t j = start; j < finish; j++) {
                if (csr->col_ind[j] == min) {
                    csc->row_ind.push_back(csr->row_id[i]);
                    csc->values.push_back(csr->values[j]);
                }
            }    
        }
        if (min != max) {
            csc->col_ptr.push_back(csc->row_ind.size());
        }
        csc->col_id.push_back(min);

        min++;
    }
    */