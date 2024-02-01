#include "biclique.hpp"

Biclique::Biclique(){}

Biclique::Biclique(std::string path)
{
    setFile(path);
    csr = new std::vector<csr_biclique*>();
    csc = new std::vector<csc_biclique*>();
    make_csr();
    make_csc();
}

Biclique::~Biclique()
{
    if (csc) {
        delete_csc();
        delete csc;
    }
    if (csr) {
        delete_csr();
        delete csr;
    }
}

void Biclique::setFile(std::string path)
{
    this->path = path; 
}

void Biclique::make_csr()
{
    TIMERSTART(BUILD_CSR_BICLIQUE);

    std::ifstream file;
    file.open(path);
    assert(file.is_open());

    std::string s;

	while (getline(file, s)) {
        auto b = new csr_biclique();
        csr->push_back(b);

        auto values = utils::splitString(s, ";");
        auto S = utils::splitString(values[0], " ");
        auto C = utils::splitString(values[1], " ");

        for (size_t i = 0; i < S.size(); i++) {
            b->row_id.push_back(atoll(S[i].c_str()));
        }
        for (size_t i = 0; i < C.size(); i++) {
            auto temp = C[i];
            temp.erase(temp.begin());
            temp.pop_back();
            auto sp = utils::splitString(temp, ",");
            b->col_ind.push_back(atoll(sp[0].c_str()));
            b->values.push_back(atoll(sp[1].c_str()));
        }
        if (max_row < b->row_id.back()) max_row = b->row_id.back();
        num_edges += C.size() * S.size();;

	}

	
	file.close();

    std::cout << "edges in bicliques: " << num_edges << std::endl;

    TIMERSTOP(BUILD_CSR_BICLIQUE);
}

void Biclique::make_csr_bin()
{
    return;
}
/*
void Biclique::make_csc()
{
    assert(csr != nullptr);

    TIMERSTART(BUILD_CSC_BICLIQUE);

    csc = new csc_biclique();

    std::unordered_map<uint32_t, uint32_t> cols;

    for (size_t i = 0; i < csr->row_id.size(); i++) {
        size_t start = csr->row_ptr.at(i);
        size_t stop = csr->row_ptr.at(i+1);
        for (size_t j = start; j < stop; j++) {
            cols[csr->col_ind.at(j)] += csr->row_id.at(i).size();
        }

    }

    csc->col_ptr.push_back(0);
    csc->row_ind.resize(num_edges, 0);
    csc->values.resize(num_edges, 0);

    for (size_t i = 0; i < cols.size(); i++) {
        if (cols[i] > 0) {
            csc->col_id.push_back(i);
            uint32_t temp = csc->col_ptr.back();
            csc->col_ptr.push_back(csc->col_ptr.back() + cols[i]);
            cols[i] = temp;
        }
    }


    for (size_t i = 0; i < csr->row_id.size(); i++) {
        if(csr->row_id.at(i).empty()) continue;
        for (auto k : csr->row_id.at(i)){
            size_t start = csr->row_ptr.at(i);
            size_t stop = csr->row_ptr.at(i+1);

            for (size_t j = start; j < stop; j++) {
                csc->row_ind.at(cols[csr->col_ind.at(j)]) = k;
                csc->values.at(cols[csr->col_ind.at(j)]) = csr->values.at(j);
                cols[csr->col_ind.at(j)]++;
            }
        }
    }

    cols.clear();

    csc->col_id.shrink_to_fit();
    csc->col_ptr.shrink_to_fit();

    TIMERSTOP(BUILD_CSC_BICLIQUE);
}
*/

void Biclique::make_csc() 
{
    assert(csr != nullptr);

    TIMERSTART(BUILD_CSC_BICLIQUE);

    for (auto i : *csr) {
        auto b = new csc_biclique();
        csc->push_back(b);
        b->values = i->values;
        b->row_ind = i->row_id;
        b->col_id = i->col_ind;

        if (max_col < b->col_id.back()) max_col = b->col_id.back();
    }



    return;

    TIMERSTOP(BUILD_CSC_BICLIQUE);

}

void Biclique::print_csr()
{
    for (auto i : *csr) {
        if (i != nullptr) i->print();
        std::cout << std::endl;
    }
}

void Biclique::print_csc()
{
    for (auto i : *csc) {
        if (i != nullptr) i->print();
        std::cout << std::endl;
    }
}

std::vector<csr_biclique*>* Biclique::get_csr()
{
    return csr;
}

std::vector<csc_biclique*>* Biclique::get_csc()
{
    return csc;
}

void Biclique::delete_csr()
{
    for(auto it : *csr) {
        delete it;
    }
    delete csr;
    csr = nullptr;
}

void Biclique::delete_csc()
{
    for (auto it : *csc) {
        delete it;
    }
    delete csc;
    csc = nullptr;
}

uint64_t Biclique::getNumEdges()
{
    return num_edges;
}

uint32_t Biclique::maxCol()
{
    return max_col;
}

uint32_t Biclique::maxRow()
{
    return max_row;
}

uint32_t Biclique::maxDim(){
    if (max_col > max_row) {
        return max_col;
    }
    return max_row;
}


