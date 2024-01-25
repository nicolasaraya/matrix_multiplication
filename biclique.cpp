#include "biclique.hpp"

Biclique::Biclique(){}

Biclique::Biclique(std::string path)
{
    setFile(path);
    make_csr();
    make_csc();
}

Biclique::~Biclique()
{
    if (csc) delete csc;
    if (csr) delete csr;
}

void Biclique::setFile(std::string path)
{
    this->path = path; 
}

void Biclique::make_csr()
{
    TIMERSTART(BUILD_CSR_BICLIQUE);

    csr = new csr_biclique();
    csr->row_ptr.push_back(0);

    std::ifstream file;
    file.open(path);
    assert(file.is_open());

    std::string s;
    bool t = false;

	while (getline(file, s)) {
        //std::cout << s << std::endl;
        if ((not s.empty()) and s.front() == 'S') {
            t = true;
            continue;
        } 
        if (s.size() <= 2) continue;
        auto values = utils::splitString(s, " ");

        if (not t) {
            for (size_t i = 1; i < values.size(); i++) {
                    auto temp = values[i];
                    temp.erase(temp.begin());
                    temp.pop_back();
                    auto sp = utils::splitString(temp, ",");
                    csr->col_ind.push_back(atoll(sp[0].c_str()));
                    csr->values.push_back(atoll(sp[1].c_str()));
                }
                csr->row_ptr.push_back(csr->col_ind.size());
        } else {
            csr->row_id.resize(csr->row_ptr.size()-1, std::vector<uint32_t>());
            //csr->row_id_2.push_back(make_pair(atoll(values[0].c_str()), std::vector<uint32_t>()));
            for (size_t i = 1; i < values.size(); i++) {
                uint32_t a = atoi(values[i].c_str());
                uint32_t b = atoi(values[0].c_str());
                csr->row_id.at(a).push_back(b);
                //csr->row_id_2.back().second.push_back(atoll(values[i].c_str()));
                num_edges += ((csr->row_ptr.at(atoll(values[i].c_str())+1)) - (csr->row_ptr.at(atoll(values[i].c_str()))));
            }
        }

	}

	
	file.close();
    csr->col_ind.shrink_to_fit();
    csr->row_id.shrink_to_fit();
    csr->values.shrink_to_fit();
    csr->row_ptr.shrink_to_fit();

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

    csc = new csc_biclique();
    csc->col_ptr.push_back(0);

    std::unordered_map<uint32_t, uint32_t> cols;

    for (size_t i = 0; i < csr->row_id.size(); i++) {
        size_t start = csr->row_ptr.at(i);
        size_t stop = csr->row_ptr.at(i+1);
        for (size_t j = start; j < stop; j++) {
            cols[csr->col_ind.at(j)]++;
        }

    }

    size_t count = 0;
    for (size_t i = 0; i < cols.size(); i++) {
        if (cols[i] > 0) {
            count += cols[i];
            csc->col_id.push_back(i);
            uint32_t temp = csc->col_ptr.back();
            csc->col_ptr.push_back(csc->col_ptr.back() + cols[i]);
            cols[i] = temp;
            //std::cout << i << ": " << cols[i] << std::endl;
        }
    }
  

    csc->row_ind.resize(count, 0);
    csc->values.resize(count, 0);
    

    for (size_t i = 0; i < csr->row_id.size(); i++) {
        size_t start = csr->row_ptr.at(i);
        size_t stop = csr->row_ptr.at(i+1);

        for (size_t j = start; j < stop; j++) {
            //std::cout << csr->col_ind[j] << " " << start << " " << stop << " " << j  <<" "<< csr->col_ind[j]  << " " <<cols[csr->col_ind[j]] <<std::endl;
            csc->row_ind[cols[csr->col_ind[j]]] = i; //id de row_id
            csc->values[cols[csr->col_ind[j]]] = csr->values[j];
            cols[csr->col_ind[j]]++;
        }
    }
    //csc->print();

    TIMERSTOP(BUILD_CSC_BICLIQUE);

}


csr_biclique *Biclique::get_csr()
{
    return csr;
}

csc_biclique *Biclique::get_csc()
{
    return csc;
}

void Biclique::set_csr(csr_biclique *mat)
{
    assert(csr == nullptr); 
    csr = mat; 
}

void Biclique::set_csc(csc_biclique *mat)
{
    assert(csc == nullptr);
    csc = mat;
}

void Biclique::delete_csr()
{
    delete csr;
    csr = nullptr;
}

void Biclique::delete_csc()
{
    delete csc;
    csc = nullptr;
}

uint64_t Biclique::getNumEdges()
{
    return num_edges;
}


