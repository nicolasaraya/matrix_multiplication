#include "matrix_multiplication.hpp"

namespace Lisa 
{
    vector<Biclique*>* load_biclique_w(std::string path) 
    {
        vector<Biclique*>* bicliques = new vector<Biclique*>();
        ifstream file; 
        file.open(path);
        assert(file.is_open());
        string line;
        Biclique* temp = nullptr;
        while (getline(file, line)) {
            auto components = splitString(line, " "); 
            if(components.size() <= 1) continue;
            size_t found = components.front().find("SxC");
            if (found != string::npos) {
                //cout << "SxC" << endl;
                continue;
                //break;
            }
            found = components.front().find("S");
            if (found != string::npos) {
                //cout << "S" << endl;
                temp = new Biclique();
                bicliques->push_back(temp);
                for (size_t i = 1; i < components.size(); i++) {
                    temp->S.push_back(atoi(components[i].c_str()));
                }
                continue;
            }
            found = components.front().find("C");
            if (found != string::npos) {
                //cout << "C" << endl;
                for (size_t i = 1; i < components.size(); i++){
                    string par = components[i].c_str();
                    par.pop_back(); 
                    par.erase(par.begin());
                    auto split = splitString(par, ",");
                    temp->C_w.push_back(make_pair(
                        atoi(split[0].c_str()),
                        atoi(split[1].c_str())
                    ));
                } 
                continue;
            }
        }
        file.close();
        return bicliques;
    }

    vector<Biclique*>* load_biclique_w_bin(std::string path) 
    {
        vector<Biclique*>* bicliques = new vector<Biclique*>();
        
        return bicliques;
    }

    CompactBicliqueWeighted* load_CompactBiclique_w(std::string path) 
    {
        CompactBicliqueWeighted* compBicl = new CompactBicliqueWeighted();
        assert(validateExtension(path, "txt"));
        ifstream file; 
        file.open(path);
        std::string line;
        getline(file,line);
        auto w = splitString(line, " ");
        for(size_t i = 1; i < w.size(); i++) {
            compBicl->weights_values.insert(atoi(w.at(i).c_str()));
        }

        while(getline(file, line) and line.front() != 'S') {
            CompactBicliqueWeighted::C_values vec; 
            auto temp = splitString(line, " ");
            temp.erase(temp.begin()); //erase B[i];

            for (auto i : temp) {
                auto sp = splitString(i, ",");
                sp[0].erase(sp[0].begin());
                sp[1].pop_back();
                vec.push_back(make_pair(atoi(sp[0].c_str()), atoi(sp[1].c_str())));
            };
            compBicl->c_bicliques.push_back(vec);
        }

        while (getline(file, line)) {
            auto spl = splitString(line, " ");
            if (spl.empty()) break;
            spl[0].pop_back();
            vector<uInt> vec; 
            for (size_t j = 1; j < spl.size(); j++) {
                vec.push_back(atoi(spl[j].c_str()));
            }
            compBicl->linked_s.push_back(make_pair(atoi(spl[0].c_str()), vec));
        }
        return compBicl;
    }

    CompactBicliqueWeighted* load_CompactBiclique_w_bin(std::string path) 
    {
        CompactBicliqueWeighted* compBicl = new CompactBicliqueWeighted();
        return compBicl;
    }

    void printBicliqueAsMatrix(Biclique* B)
    {
        for (size_t i = 0; i < B->S.size(); i++) {
            if(i < B->S.at(i) - 1) {
                int temp = i; 
                while(temp < B->S.at(i)) {
                    cout << "Node " << temp << ": -" << endl;
                    temp++; 
                }
            }
            cout << "Node " << B->S.at(i) << ": " ;
            if (not B->C_w.empty()) {
                for(size_t j = 0; j < B->C_w.size(); j++) {
                    if (j < B->C_w.at(j).first ) {
                        int temp2 = j; 
                        while(temp2 < B->C_w.at(j).first - 1) {
                            cout << "0 ";
                            temp2++; 
                        }
                    }
                    cout << B->C_w.at(j).second  << " ";
                }
            }
            cout << endl;   
        }
        return; 
    }

    void printCompactStructure(CompactBicliqueWeighted* B)
    {
        std::cout << "Weights: ";
        for (auto i : B->weights_values) {
            std::cout << i  << " ";
        }
        std::cout << endl; 
        std::cout << "Bicliques" << endl;
        for (size_t i = 0; i < B->c_bicliques.size(); i++) {
            std::cout << "B[" << i << "]: ";
            for (auto j : B->c_bicliques.at(i)) {
                std::cout << "(" << j.first << "," << j.second << ") "; 
            }
            std::cout << endl;
        }
        std::cout << "S: " << endl;
        for (auto i : B->linked_s){
            std::cout << i.first << ": ";
            for (auto j : i.second) {
                std::cout << j << " ";
            }
            std::cout << endl;
        } 
        return; 
    }

    GraphWeighted* matrix_multiplication_w(GraphWeighted* A, GraphWeighted* B, bool sw)
    {
        assert(A != nullptr and B != nullptr);
        if (sw) {
            return matrix_multiplication_w_transposed(A, B);
        } else {
            return matrix_multiplication_w(A,B);
        }
        return nullptr;
    }

    GraphWeighted* matrix_multiplication_w(GraphWeighted* A, GraphWeighted* B, std::vector<Biclique*>* bicliques, bool sw)
    {
        assert(A != nullptr and B != nullptr and bicliques != nullptr);
        GraphWeighted* C = nullptr;
        if (sw) {
            C = matrix_multiplication_w_transposed(A, B); 

        } else {
            C = matrix_multiplication_w(A, B); //standard multiplication A\b x B    
        }
        TIMERSTART(remplazing_std_bicl);
        assert(not bicliques->empty());
        for (auto X : *bicliques) {
            assert(not X->S.empty());
            assert(X->C.empty());
            assert(not X->C_w.empty());
            std::vector<std::pair<uInt,uInt>>* vec = nullptr; 

            if (sw) {
                vec = vector_matrix_t_multiplication_w(X->C_w, B);
            } else {
                vec = vector_matrix_multiplication_w(X->C_w, B);
            }   

            for (auto i : X->S) {
                Node* node = C->find(i); 
                if (node == nullptr) {
                    //cout << "no encuentra" << i << endl;
                    node = new Node(i, true); 
                    C->insert(node);
                    C->sort();
                } else {
                    //cout << "encuentra " << i << endl;
                }
                for (auto j : *vec) {
                    if ( not node->increaseWeight(j.first, j.second) ) {
                        //cout << "añade (" << j.first << "," << j.second << ") en " << i << endl;
                        node->addAdjacent(j.first, j.second);
                        node->sort();
                    }
                }
                //node->print();
            }
            delete vec; 
        } 

        TIMERSTOP(remplazing_std_bicl);
        return C;
    }

    GraphWeighted* matrix_multiplication_w(GraphWeighted* A, GraphWeighted* B, CompactBicliqueWeighted* compBicl, bool sw) 
    {
        GraphWeighted* C = nullptr; 
        assert(A != nullptr and B != nullptr and compBicl != nullptr);
        if (sw) {
            C = matrix_multiplication_w_transposed(A, B);
        } else {
            C = matrix_multiplication_w(A, B);
        }

        std::vector<std::vector<std::pair<uInt,uInt>>*> res; 

        for (auto i : compBicl->c_bicliques) {
            if (sw) {
                res.push_back(vector_matrix_t_multiplication_w(i, B));  
            } else {
                res.push_back(vector_matrix_multiplication_w(i, B));
            }
        }


        TIMERSTART(remplazing_comp);
        for (auto i : compBicl->linked_s) {
            Node* node = C->find(i.first);
            if (node == nullptr) {
                node = new Node(i.first, true);
                C->insert(node);
                C->sort();
            }
            for (auto j : i.second) {
                for (auto k : *res.at(j)) {
                    if (not node->increaseWeight(k.first, k.second)) {
                        node->addAdjacent(k.first, k.second);
                        node->sort();
                    }
                }
            }
        }
        res.clear();
        TIMERSTOP(remplazing_comp);
        return C; 
    }


    GraphWeighted* matrix_multiplication_w(GraphWeighted* A, GraphWeighted* B) 
    {
        if (B->isTransposed()) B->transpose();
        //se pueden omitir algunas columnas
        vector<uInt> skip; 
        //omp_set_num_threads(8);
        GraphWeighted* C = new GraphWeighted();
        Node* temp = nullptr; 
        //uint64_t jMax = B->maxValueEdge(); 
        uint64_t jMax = A->maxValueEdge();
        //cout << jMax << endl;
        //#pragma omp parallel for
        for (size_t i = 0; i < A->size(); i++) {
            Node* Ai = A->at(i);
            //cout << "fila:" << Ai->getId() << endl;
            temp = new Node(Ai->getId(), true);
            //uint64_t jMax = Ai->getBackAdjacent();
            C->insert(temp);
            if(Ai->edgesSize() == 0) continue;
            uInt index = 0; 
            for(size_t j = 0; j <= jMax + 1; j++) {
                //cout << "columna: " << j << endl;
                //if(j != 12) continue;
                if(not skip.empty() and j == skip[index]){
                    index++;
                    if(index >= skip.size()) index--;
                    //continue;
                }
                uint64_t sum = 0; 
                uint64_t count_zero = 0;
                size_t k = 0;
                for(auto Aik = Ai->wAdjacentsBegin(); Aik != Ai->wAdjacentsEnd(); Aik++) {
                    //if(sw) break; 
                    for (; k < B->size(); k++) {
                        //if(sw) break;
                        Node* Bk = B->at(k);
                        if(i == 0){
                            auto Bkj_w = Bk->findAdjacentWeighted(j);
                            if (Bkj_w == 0) {
                                count_zero++;
                            }
                        }
                        //if((*Aik).first > Bk->getBackAdjacent()) continue; 
                        if (Bk->getId() == (*Aik).first) {
                            //cout << "Node: " << temp->getId() << endl;
                            //cout << "j: " << j << endl;
                            //cout << "Aik: " << (*Aik).first << " " << (*Aik).second << endl;
                            //cout << "Bk: " << Bk->getId() << endl;
                            //cout << "Search: " << j << " in " << Bk->getId() << endl;  
                            auto Bkj_w = Bk->findAdjacentWeighted(j);
                            //cout << "Res search: " << Bkj_w << endl;
                            if (Bkj_w > 0) {
                                //if(7 == temp->getId()) cout << Ai->getId() << "!" << (*Aik).first << "//" << Bk->getId()<< "$" << j << endl;
                                //cout << "Sum: " << sum << endl;
                                //cout << (*Aik).second << " " << Bkj_w << endl;
                                sum += (*Aik).second * Bkj_w; 
                                //sum += (*Aik).second * j;
                                //cout << "Sum increased: " << sum << endl;
                            } 
                            //cout << "######" << endl;
                            break;
                        }
                    }
                }
                //cout << "push: " << temp->getId() << endl;
                //cout << j << " " << sum << endl;
                if(sum > 0) {
                    temp->addAdjacent(j, sum);
                }
                
                if(i == 0 && count_zero == B->size()){
                    skip.push_back(j);
                    //cout << "push" << j << endl;
                } else if (i == 0) {
                    //cout << "count zero: " << count_zero << endl;
                }
                
            }
            //temp->print();
        }
        return C;
    }

    
    GraphWeighted* matrix_multiplication_w_transposed(GraphWeighted* A, GraphWeighted* B)
    {
        // row x row  multiplication 
        omp_set_num_threads(NUM_THREADS);
        std::cout << "Using " << NUM_THREADS << " threads" << std::endl;
        GraphWeighted* C = new GraphWeighted();
        
        if (not B->isTransposed()) {
            B->transpose(); 
        }

        if (A->isTransposed()) {
            A->transpose(); 
        }
        std::mutex mtx;
        #pragma omp parallel for
        for (size_t i = 0; i < A->size(); i++) {
            Node* Ai = A->at(i);
            if (Ai->edgesSize() == 0) continue;
            Node* temp = new Node(Ai->getId(), true);
            //uint64_t jMax = Ai->getBackAdjacent();
            mtx.lock();
            //cout << Ai->getId() << endl;
            C->insert(temp);
            mtx.unlock();
            for (auto Bj = B->begin(); Bj != B->end(); Bj++){
                if ((*Bj)->edgesSize() == 0) continue;
                uInt sum = 0; 
                auto adjB = (*Bj)->wAdjacentsBegin(); 
                for (auto adjA = Ai->wAdjacentsBegin(); adjA != Ai->wAdjacentsEnd(); adjA++) {
                    while ((*adjB).first < (*adjA).first and adjB != (*Bj)->wAdjacentsEnd()) {
                        adjB++; 
                    } 
                    if ((*adjA).first == (*adjB).first) {
                        sum += (*adjA).second * (*adjB).second; 
                    }  
                }

                if (sum > 0) {
                    temp->addAdjacent((*Bj)->getId(), sum);
                }
            }
            //temp->print();
        }
        C->sort();
        return C;
    }

    vector<pair<uInt, uInt>>* vector_matrix_multiplication_w(vector<pair<uInt, uInt>> v, GraphWeighted *B)
    {
        if (B->isTransposed()) B->transpose();
        vector<pair<uInt,uInt>>* res = new vector<pair<uInt,uInt>>();
        //Graph* C = new Graph();
        uint64_t jMax = B->maxValueEdge(); 
        for(size_t j = 0; j <= jMax + 1; j++) {
            uInt sum = 0;
            for(auto vi = v.begin(); vi != v.end(); vi++) {
                for (size_t k = 0; k < B->size(); k++) {
                    auto Bk = B->at(k);
                    if((*vi).first == Bk->getId()){
                        auto Bk_w = Bk->findAdjacentWeighted(j);
                        if (Bk_w > 0) {
                            sum += (*vi).second * Bk_w; 
                        } 
                    }
                }
            }
            if(sum > 0) {
                res->push_back(make_pair(j, sum));
            }
        }
        return res;
    }

    vector<pair<uInt, uInt>>* vector_matrix_t_multiplication_w(vector<pair<uInt, uInt>> v, GraphWeighted *B)
    {
        if (not B->isTransposed()) B->transpose();
        vector<pair<uInt,uInt>>* res = new vector<pair<uInt,uInt>>();
        
        for (auto Bj = B->begin(); Bj != B->end(); Bj++) {
            if ((*Bj)->edgesSize() == 0) continue;
            uInt sum = 0;
            auto adjB = (*Bj)->wAdjacentsBegin();
            for (auto adjA = v.begin(); adjA != v.end(); adjA++) {
                while ((*adjB).first < (*adjA).first and adjB != (*Bj)->wAdjacentsEnd()) {
                    adjB++; 
                } 
                if ((*adjA).first == (*adjB).first) {
                    sum += (*adjA).second * (*adjB).second; 
                }  
            }
            if (sum > 0) {
                res->push_back(make_pair((*Bj)->getId(), sum));
            }
        }
        return res;
    }

    /*
     * No 
     * Weighted
    */

    Graph* matrix_multiplication(Graph* A, Graph* B)
    {
        Graph* C = new Graph();
        Node* temp = nullptr; 
        uint64_t jMax = A->maxValueEdge(); 
        for (size_t i = 0; i < A->size(); i++) {
            Node* Ai = A->at(i);
            temp = new Node(Ai->getId());
            C->insert(temp);
            for(size_t j = 0; j <= jMax + 1; j++) {
                bool sw = false;
                for(auto Aik = Ai->adjacentsBegin(); Aik != Ai->adjacentsEnd(); Aik++) {
                    if(sw) break; 
                    for (size_t k = 0; k < B->size(); k++) {
                        if(sw) break;
                        Node* Bk = B->at(k);
                        if (Bk->getId() == *Aik) {
                            //cout << "Node: " << temp->getId() << endl;
                            //cout << "j: " << j << endl;
                            //cout << "Aik: " << *Aik << endl;
                            //cout << "Bk: " << Bk->getId() << endl;
                            //cout << "Search: " << j << " in " << Bk->getId() << endl;  
                            if(Bk->findAdjacent(j)) {
                                temp->addAdjacent(j);
                                //cout << "added" << endl;
                                sw = true;
                            }
                        }
                    }
                }
            }
        }
        return C;
    }

    Graph* matrix_multiplication(Graph* A, Graph* B, Biclique* X)
    {
        assert(not X->S.empty());
        assert(not X->C.empty());
        assert(X->C_w.empty());

        auto C = matrix_multiplication(A, B); 
        
        cout << "A-biclique x B: " << endl; 
        C->printAsMatrix();

        cout << endl << "vector x B " << endl; 
        auto vec = vector_matrix_multiplication(X->C, B); 
        for (auto i : *vec) {
            cout << i << " " ;
        }
        cout << endl;

        for (auto i : X->S) {
            auto node = C->find(i); 

            if (node == nullptr) {
                node = new Node(i, true); 
                C->insert(node);
            }
            for (auto j : *vec) {
                node->addAdjacent(j);
            }
            node->sort();
        }

        cout << endl <<"C Matrix: " << endl; 
        C->printAsMatrix();
        return nullptr;
    }

    std::vector<uInt>* vector_matrix_multiplication(std::vector<uInt> v, Graph* B)
    {
        vector<uInt>* res = new vector<uInt>(); 
        uint64_t jMax = v.back(); 
        for (size_t j = 0; j <= jMax + 1; j++) {
            bool sw = false; 
            for (auto vk = v.begin(); vk != v.end(); vk++) {
                if (sw) break;
                for (size_t k = 0; k < B->size(); k++) {
                    if (sw) break;
                    auto Bk = B->at(k);
                    if (Bk->getId() == *vk) {
                        if (Bk->findAdjacent(j)) {
                            res->push_back(j);
                            sw = true; 
                        }
                    }
                }
            }
        }
        return res;
    }   
}

