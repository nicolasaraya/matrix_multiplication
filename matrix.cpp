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

csc_matrix *Matrix::make_csc()
{
    TIMERSTART(BUILD_CSC);
    csc = new csc_matrix();
    csc->col_ptr.push_back(0);
    bool flag = false;
    if (csr == nullptr) {
        flag = true;
        csr = make_csr();
    }

    uint32_t max = csr->col_ind.front();
    uint32_t min = csr->col_ind.front();

    for (auto i : csr->col_ind) {
        if (i > max) {
            max = i;
        }
        if (i < min) {
            min = i;
        }
    }

    //int it = 0;
    while (min <= max) {
        uint32_t new_min = max;
        for (size_t i = 0; i < csr->row_ptr.size(); i++) {
            size_t start = csr->row_ptr[i];
            size_t finish;

            if (i == csr->row_ptr.size()-1) finish = csr->col_ind.size();
            else finish = csr->row_ptr[i+1]; 

            //std::cout << "s: " << start << " , f:" << finish << std::endl;

            for (size_t j = start; j < finish; j++) {
                //std::cout << "val:" << csr->col_ind[j] << std::endl;

                if (csr->col_ind[j] == min) {
                    csc->row_ind.push_back(csr->row_id[i]);
                    csc->values.push_back(csr->values[j]);
                    //break;
                }

                if (csr->col_ind[j] > min) {
                    if (csr->col_ind[j] < new_min) {
                        new_min = csr->col_ind[j];
                        std::cout << "new_min: " << new_min << std::endl;
                    }
                    break;
                }
                
            }    
        }
        if (min != max) {
            csc->col_ptr.push_back(csc->row_ind.size());
        }
        csc->col_id.push_back(min);
        std::cout << "new it" << std::endl;
        if(min == new_min) {
            new_min++;
        }
        min = new_min;
        //if (it++ > 10) break;
    }

    /*
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

    if (flag) delete csr; 
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
    std::ofstream file;
	std::string pathFile = utils::modify_path(path, 4 ,utils::now_time()+".txt");

	file.open(pathFile, std::ofstream::out | std::ofstream::trunc); 
	assert(file.is_open());

    for (size_t i = 0; i < csr->row_id.size(); i++) {
        size_t start = csr->row_ptr[i];
        size_t stop; 
        if (i == csr->row_id.size()-1) stop = csr->col_ind.size();
        else stop = csr->row_ptr[i+1];

        for (size_t j = start; j < stop; j++) {
            file << csr->row_id[i] << " " << csr->col_ind[j] << " " << csr->values[j] << std::endl;
        }
    }
}
