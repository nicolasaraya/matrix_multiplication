#include "matrix.hpp"

Matrix::Matrix(){}

Matrix::Matrix(std::string path, bool b_csr, bool b_csc)
{
    setFile(path);
    if (b_csr) make_csr();
    if (b_csc) make_csc();
}

void Matrix::setFile(std::string path)
{
    this->path = path; 
}

csr_matrix* Matrix::make_csr()
{
    csr = new csr_matrix();
    std::ifstream file;
    file.open(path);
    assert(file.is_open());

	while (not file.eof()) {
		std::string s;
        getline(file, s);
        auto values = splitString(s, " ");

        csr->values.push_back(atoi(values[2].c_str()));
        csr->col_ind.push_back(atoi(values[1].c_str()));
        
        if (csr->row_id.empty() or csr->row_id.back() != unsigned(atoi(values[0].c_str()))) {
            csr->row_id.push_back(atoi(values[0].c_str()));
            csr->row_ptr.push_back(csr->col_ind.size()-1);
        }

	}
	
	file.close();

    return csr;
}

csc_matrix *Matrix::make_csc()
{
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

    if (flag) delete csr; 
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

std::vector<std::string> splitString(std::string line, std::string delims)
{
    std::string::size_type bi, ei;
    std::vector<std::string> words;
    bi = line.find_first_not_of(delims);
    while (bi != std::string::npos)
    {
        ei = line.find_first_of(delims, bi);
        if (ei == std::string::npos)
            ei = line.length();
        std::string aux = line.substr(bi, ei - bi);
        words.push_back(aux.c_str());
        bi = line.find_first_not_of(delims, ei);
    }
    return words;
}
