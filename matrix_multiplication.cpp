#include "matrix_multiplication.hpp"

namespace mw
{
    vector<Biclique*>* load_biclique(std::string path) 
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
                    temp->S->push_back(atoi(components[i].c_str()));
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
                    temp->C->push_back(make_pair(
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

    vector<Biclique*>* load_biclique_bin(std::string path) 
    {
        vector<Biclique*>* bicliques = new vector<Biclique*>();
        
        return bicliques;
    }

    CompactBiclique* load_CompactBiclique(std::string path) 
    {
        if (not validateExtension(path, "txt")) {
            return nullptr;
        }

        CompactBiclique* compBicl = new CompactBiclique();
        ifstream file; 
        file.open(path);
        std::string line;
        getline(file,line);
        auto w = splitString(line, " "); // weights
        //for(size_t i = 1; i < w.size(); i++) {
        //    compBicl->weights_values.insert(atoi(w.at(i).c_str()));
        //}

        while(getline(file, line) and line.front() != 'S') {
            auto vec = new C_Values(); 
            auto temp = splitString(line, " ");
            temp.erase(temp.begin()); //erase B[i];

            for (auto i : temp) {
                auto sp = splitString(i, ",");
                sp[0].erase(sp[0].begin());
                sp[1].pop_back();
                vec->push_back(make_pair(atoi(sp[0].c_str()), atoi(sp[1].c_str())));
            };
            compBicl->c_collection.push_back(vec);
            auto bicl = new Biclique(); 
            bicl->setC(vec); 
            compBicl->biclique_collection.push_back(bicl); 
        }

        while (getline(file, line)) {
            auto spl = splitString(line, " ");
            if (spl.empty()) break;
            spl[0].pop_back();
            vector<uInt> vec; 
            uInt index = atoi(spl[0].c_str()); 
            for (size_t j = 1; j < spl.size(); j++) {
                uInt value = atoi(spl[j].c_str());
                vec.push_back(value);
                compBicl->biclique_collection.at(value)->S->push_back(index);
            }
            compBicl->linked_s.push_back(make_pair(index, vec));
        }
        return compBicl;
    }

    CompactBiclique* load_CompactBiclique_bin(std::string path) 
    {
        if (not validateExtension(path, "bin")) {
            return nullptr;
        }
        ifstream S, SS, C, CC;
        while(path.back() != '_'){
            path.pop_back();
        }
        S.open(path + "S.bin", ios::in | ios::binary);
        SS.open(path + "SS.bin", ios::in | ios::binary);
        C.open(path + "C.bin", ios::in | ios::binary);
        CC.open(path + "CC.bin", ios::in | ios::binary);

        std::cout << "open file: " << path + "S.bin" << std::endl;
        std::cout << "open file: " << path + "SS.bin" << std::endl;
        std::cout << "open file: " << path + "C.bin" << std::endl;
        std::cout << "open file: " << path + "CC.bin" << std::endl;


        assert(S.is_open());
        assert(SS.is_open());
        assert(C.is_open());
        assert(CC.is_open());
        
        CompactBiclique* compBicl = new CompactBiclique();
        typedef bool binVar;
        uInt* buffer = new uInt(0); 
        binVar* binBuffer = new binVar(0);
        uInt S_size = 0; 

       

        CC.read((char*)binBuffer, sizeof(binVar)); // 1
        uInt C_size = 0;
        while (not CC.eof()) {
            CC.read((char*)binBuffer, sizeof(binVar)); // 1
            if (*binBuffer == 1 or CC.eof()) {
                //cout << C_size << endl;
                auto vec = new C_Values(); 
                auto bicl = new Biclique();
                bicl->setC(vec); 
                for (size_t i = 0; i < C_size; i++) {
                    C.read((char*)buffer, sizeof(uInt));
                    uInt first = *buffer;
                    C.read((char*)buffer, sizeof(uInt));
                    uInt second = *buffer;
                    vec->push_back(make_pair(first,second));
                }
                compBicl->c_collection.push_back(vec);
                compBicl->biclique_collection.push_back(bicl); 
                C_size = 0;
            }
            else {
                C_size++;
            }
        }
        
        SS.read((char*)binBuffer, sizeof(binVar)); // 1
        while (not SS.eof()) {
            SS.read((char*)binBuffer, sizeof(binVar));
            if (*binBuffer == 1 or SS.eof()) {
                //cout << S_size << endl;
                vector<uInt> vec; 
                uInt index = 0;
                for (size_t i = 0; i <= S_size; i++) { 
                    S.read((char*)buffer, sizeof(uInt));
                    if (i == 0) {
                        index = *buffer;
                        //cout << endl<< "size: " << S_size  <<", index: " << index << ": "; 
                    } else {
                        //cout << *buffer << " ";
                        vec.push_back(*buffer);
                        compBicl->biclique_collection.at(*buffer)->S->push_back(index);
                        //if (debug) std::cout << "push in " << *buffer << ": " << index << endl;
                    }
                }
                compBicl->linked_s.push_back(make_pair(index, vec));
                S_size = 0;
            } else {
                S_size++;
            }
        }


        S.close();
        SS.close();
        C.close();
        CC.close();
        //printCompactStructure(compBicl);
        delete buffer;
        delete binBuffer;




        return compBicl;
    }

    GraphWeighted* transpose_CompactBiclique(CompactBiclique* compBicl)
    {
        if (compBicl == nullptr) return nullptr;  

        auto transposed = new GraphWeighted(); 
        transposed->transpose();

        vector<Node*> temp; 

        for (auto Si = compBicl->linked_s.begin(); Si != compBicl->linked_s.end(); Si++) {
            for (auto Sij = (*Si).second.begin(); Sij != (*Si).second.end(); Sij++) {
                for (auto k = compBicl->c_collection.at(*Sij)->begin(); k != compBicl->c_collection.at(*Sij)->end(); k++) {
                    while(temp.size() <= (*k).first) {
                        temp.push_back(new Node(temp.size(), true));
                    }
                    //cout << "add in: " << (*k).first << " = (" << (*Si).first << "," << (*k).second << ")" << endl;
                    temp.at((*k).first)->addAdjacent((*Si).first, (*k).second); 
                }
            }
        }

        for (auto i : temp) {
            if (i->edgesSize() > 0) {
                transposed->insert(i); 
            }
            else {
                delete i;
            }
        }

        temp.clear(); 

        return transposed;
    }

    void printBicliqueAsMatrix(Biclique* B)
    {
        for (size_t i = 0; i < B->S->size(); i++) {
            if(i < B->S->at(i) - 1) {
                uInt temp = i; 
                while(temp < B->S->at(i)) {
                    std::cout << "Node " << temp << ": -" << std::endl;
                    temp++; 
                }
            }
           std::cout << "Node " << B->S->at(i) << ": " ;
            if (not B->C->empty()) {
                for(size_t j = 0; j < B->C->size(); j++) {
                    if (j < B->C->at(j).first ) {
                        uInt temp2 = j; 
                        while(temp2 < B->C->at(j).first - 1) {
                            std::cout << "0 ";
                            temp2++; 
                        }
                    }
                    std::cout << B->C->at(j).second  << " ";
                }
            }
            std::cout << std::endl;   
        }
        return; 
    }

    void printCompactStructure(CompactBiclique* B)
    {
        std::cout << std::endl; 
        std::cout << "C collection" << std::endl;
        for (size_t i = 0; i < (B->c_collection).size(); i++) {
            std::cout << "C[" << i << "]: ";
            for (auto j : *(B->c_collection).at(i)) {
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
            std::cout << std::endl;
        } 
        std::cout << "Bicliques Collection " << std::endl; 
        
        uInt count = 0; 
        for (auto i : B->biclique_collection) {
            std::cout << "S[" << count << "]: ";
            for (auto j : *(i->S)) {
                std::cout << j << " " ; 
            }
            
            std::cout << std::endl << "C[" << count << "]: ";
            for (auto j : *(i->C)) {
                std::cout << "(" << j.first << "," << j.second << ") " ; 
            }
            std::cout << std::endl;
            count++;
        }


        return; 
    }

    GraphWeighted* matrix_multiplication(GraphWeighted* A, GraphWeighted* B, std::vector<Biclique*>* bicliques) // (A'+B) x A = A^2 
    {
        assert(A != nullptr and B != nullptr and bicliques != nullptr);
        GraphWeighted* C = nullptr;
        
        C = matrix_multiplication(A, B); 

        TIMERSTART(remplazing_std_bicl);
        assert(not bicliques->empty());
        for (auto X : *bicliques) {
            assert(not X->S->empty());
            assert(not X->C->empty());
            std::vector<std::pair<uInt,uInt>>* vec = nullptr; 
            
            //TIMERSTART(compute);
            vec = vector_matrix_multiplication(X->C, B);
            //TIMERSTOP(compute);

            for (auto i : *(X->S)) {
                Node* node = C->find(i); 
                if (node == nullptr) {
                    node = new Node(i, true); 
                    C->insert(node);
                    C->sort(); //necesario por que puede haber un S[i] repetido en otro biclique.
                }
                for (auto j : *vec) {
                    if ( not node->increaseWeight(j.first, j.second) ) {
                        node->addAdjacent(j.first, j.second);
                    }
                    node->sort();
                }
            }
            delete vec; 
        } 
        TIMERSTOP(remplazing_std_bicl);
        return C;
    }

    GraphWeighted* matrix_multiplication(GraphWeighted* A, GraphWeighted* B, CompactBiclique* compBicl) 
    {
        GraphWeighted* C = nullptr; 
        assert(A != nullptr and B != nullptr and compBicl != nullptr);
        
        C = matrix_multiplication(A, B);
        
        TIMERSTART(compute);
        std::vector<std::vector<std::pair<uInt,uInt>>*>* res = compute_compact_struct(compBicl, B); 
        TIMERSTOP(compute);
        TIMERSTART(remplazing_comp);
        appendInGraph(res, compBicl, C);

        /*
        for (auto i : compBicl->linked_s) {
            Node* node = C->find(i.first);
            if (node == nullptr) {
                node = new Node(i.first, true);
                C->insert(node);
            }
            for (auto j : i.second) {
                for (auto k : *(res->at(j))) {
                    if (not node->increaseWeight(k.first, k.second)) {
                        node->addAdjacent(k.first, k.second);       
                    }
                }
                node->sort();
            }
        }

        
        C->sort();
        */
        TIMERSTOP(remplazing_comp);
        return C; 
    }

    GraphWeighted* matrix_pow(GraphWeighted* A, CompactBiclique* compBicl)
    {
        //A->printAsMatrix();
        std::cout << A->getPath() << std::endl;
        auto B = new GraphWeighted(A->getPath());
        B->transpose();
        
        //printCompactStructure(compBicl);

        TIMERSTART(AXA);
        auto aa = matrix_multiplication(A, B); //AxA
        //auto aa = new GraphWeighted();
        TIMERSTOP(AXA);
        delete B;
        TIMERSTART(BXA);
        auto ba = compute_compact_struct(compBicl, A);
        appendInGraph(ba, compBicl, aa);
        TIMERSTOP(BXA);
        TIMERSTART(AXB);
        compute_compact_struct(A, compBicl, aa);
        TIMERSTOP(AXB);
        TIMERSTART(BXB);
        biclique_pow(compBicl, aa);
        TIMERSTOP(BXB);

        return aa;
    }
    
  
    
    GraphWeighted* matrix_multiplication(GraphWeighted* A, GraphWeighted* B) // row x row  multiplication (B transposed)
    {
        #if defined(parallel)
            omp_set_num_threads(THREADS);
            std::cout << "Using " << THREADS << " threads" << std::endl;
        #endif

        GraphWeighted* C = new GraphWeighted();
        if (not B->isTransposed()) {
            B->transpose(); 
            //std::cout << "Ok" << std::endl;
        }

        if (A->isTransposed()) {
            A->transpose(); 
            //std::cout << "ok" << std::endl;
        }


        #if defined(parallel)
        std::mutex mtx;
        #pragma omp parallel for
        #endif

        for (auto Ai = A->begin(); Ai != A->end(); Ai++) {
            // if ((*Ai)->edgesSize() == 0) continue;
            Node* temp = new Node((*Ai)->getId(), true);
            //uint64_t jMax = Ai->getBackAdjacent();

            #if defined(parallel)
            mtx.lock();
            #endif
            C->insert(temp);
            #if defined(parallel)
            mtx.unlock();
            #endif

            auto adjA_begin = (*Ai)->wAdjacentsBegin();
            auto adjA_end = (*Ai)->wAdjacentsEnd();

            for (auto Bj = B->begin(); Bj != B->end(); Bj++){
                //if ((*Bj)->edgesSize() == 0) continue;
                uInt sum = 0; 
                auto adjB = (*Bj)->wAdjacentsBegin(); 
                auto adjB_end = (*Bj)->wAdjacentsEnd();
                auto adjA = adjA_begin;

                while (adjA != adjA_end and adjB != adjB_end) {
                    if ((*adjA).first == (*adjB).first) {
                        sum += (*adjA).second * (*adjB).second; 
                        if (debug > 2) std::cout << "sum: " << sum << std::endl << std::endl;
                        adjA++;
                        adjB++;
                    } else if ((*adjA).first < (*adjB).first) {
                        adjA++; 
                    } else {
                        adjB++;
                    }

                }


                /*
                for (auto adjA = Ai->wAdjacentsBegin(); adjA != Ai->wAdjacentsEnd(); adjA++) {
                    while (adjB != (*Bj)->wAdjacentsEnd() and (*adjB)->first < (*adjA)->first) {
                        adjB++; 
                    }

                    if (adjB == (*Bj)->wAdjacentsEnd()) break; 

                    if ((*adjA)->first == (*adjB)->first) {
                        if(debug > 2){
                            std::cout << "Node A" << Ai->getId() << std::endl; 
                            std::cout << "Node B" << (*Bj)->getId() << std::endl;
                            std::cout << "(*adjA).first:" << (*adjA)->first << " && " << "(*adjB).first: " << (*adjB)->first << std::endl;
                            std::cout << "(*adjA).second:" << (*adjA)->second << " && " << "(*adjB).sec: " << (*adjB)->second << std::endl;
                            std::cout << "sum prev: " << sum << endl;
                        }
                        sum += (*adjA)->second * (*adjB)->second; 
                        if (debug > 2) std::cout << "sum: " << sum << std::endl << std::endl;
                        adjB++;
                    }  
                }
                */
                if (sum > 0) {
                    temp->addAdjacent((*Bj)->getId(), sum);
                }
            }
            //temp->print();
        }
        //C->sort();
        return C;
    }


    vector<pair<uInt, uInt>>* vector_matrix_multiplication(vector<pair<uInt, uInt>>* v, GraphWeighted *B)
    {
        if (not B->isTransposed()) B->transpose();
        vector<pair<uInt,uInt>>* res = new vector<pair<uInt,uInt>>();
        
        for (auto Bj = B->begin(); Bj != B->end(); Bj++) { // Bj j-esima fila en B; 
            if ((*Bj)->edgesSize() == 0) continue;
            uInt sum = 0;
            auto adjB = (*Bj)->wAdjacentsBegin();  //adjB iterador de lista de adyacencia de la fila j-esima en B.
            for (auto adjA = v->begin(); adjA != v->end(); adjA++) {
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


    std::vector<std::vector<std::pair<uInt,uInt>>*>* compute_compact_struct(CompactBiclique* compBicl, GraphWeighted* B)
    {
        auto res = new resArray();
        if (not B->isTransposed()) B->transpose();

        for (size_t i = 0; i < compBicl->c_collection.size(); i++) {
            auto r = new std::vector<std::pair<uInt,uInt>>();
            res->push_back(r);
            for (auto Bj = B->begin(); Bj != B->end(); Bj++) {
                if ((*Bj)->edgesSize() == 0) continue;
                auto adjB = (*Bj)->wAdjacentsBegin(); 
                uInt sum = 0;
                for (auto biclIt : *(compBicl->c_collection).at(i)) {
                    while ((adjB != (*Bj)->wAdjacentsEnd()) and (*adjB).first < (biclIt).first) {
                        adjB++;
                    }

                    if (adjB == (*Bj)->wAdjacentsEnd()) break; 

                    if (biclIt.first == (*adjB).first) {
                        
                        if (debug > 2){
                            std::cout << "Bicl " << i << std::endl; 
                            std::cout << "Node B" << (*Bj)->getId() << std::endl;
                            std::cout << "(*adjA).first:" << (biclIt).first << " && " << "(*adjB).first: " << (*adjB).first << std::endl;
                            std::cout << "(*adjA).second:" << (biclIt).second << " && " << "(*adjB).sec: " << (*adjB).second << std::endl;
                            std::cout << "sum prev: " << sum << std::endl;
                        }

                        sum += (biclIt).second * (*adjB).second; 

                        if (debug > 2) std::cout << "sum: " << sum << std::endl << std::endl; 
                    }
                    
                }
                if (sum > 0) {
                    r->push_back(make_pair((*Bj)->getId(), sum));
                }
            }
        }
        
        return res;
    }


    GraphWeighted* compute_compact_struct(GraphWeighted* A, CompactBiclique* compBicl, GraphWeighted* res) 
    {
        //auto res = new resArray();
        if (A->isTransposed()) A->transpose();

        typedef std::vector<std::pair<uInt,uInt>> transposedRow; 

        if (res == nullptr) res = new GraphWeighted();
    
        uInt index = 0; 
        for (auto i : compBicl->biclique_collection) {
            std::vector<transposedRow> rows;  //same size as S 
            for (auto Cij : *(i->C)) {
                transposedRow row; 
                for (auto Sik : *(i->S)) {
                    row.push_back(make_pair(Sik, Cij.second));
                }
                rows.push_back(row);

                if (debug) {
                    std::cout << "C[" << index << "]t: ";
                    for (auto k : row) {
                        std::cout << " ("<< k.first << " " << k.second << ") ";
                    }
                    std::cout << std::endl;
                }
                index++;
            }


            for (auto Aj = A->begin(); Aj != A->end(); Aj++) {
                if ((*Aj)->edgesSize() == 0) continue; 

                for(size_t j = 0; j < rows.size(); j++) {
                    auto aij = (*Aj)->wAdjacentsBegin();
                    uInt sum = 0; 
                    for (auto r : rows.at(j)) {
                        while(aij != (*Aj)->wAdjacentsEnd() and (*aij).first < r.first ) {
                            aij++;
                        }
                        if (aij == (*Aj)->wAdjacentsEnd()) break; 

                        if (r.first == (*aij).first) {
                            sum += r.second * (*aij).second;
                        }
                    }
                    if (sum > 0) {
                        auto node = res->find((*Aj)->getId()); 
                        if (node == nullptr) {
                            node = new Node((*Aj)->getId(), true);
                            res->insert(node);
                        }

                        if (not node->increaseWeight(i->C->at(j).first, sum)) {
                            node->addAdjacent(i->C->at(j).first, sum);
                            node->sort();
                        }
                    }
                }     
                
            }
        }
        return res; 
    }
    

    void appendInGraph(std::vector<std::vector<std::pair<uInt,uInt>>*>* res, CompactBiclique* compBicl, GraphWeighted* graph)
    {
        for (auto i : compBicl->linked_s) {
            Node* node = graph->find(i.first);
            if (node == nullptr) {
                node = new Node(i.first, true);
                graph->insert(node);
            }
            for (auto j : i.second) {
                for (auto k : *(res->at(j))) {
                    if (not node->increaseWeight(k.first, k.second)) {
                        node->addAdjacent(k.first, k.second);       
                    }
                }
                node->sort();
            }
            //std::cout << "$$" << std::endl;
            //graph->print();
            //std::cout << "%%" <<std::endl;
        }
        graph->sort();

        while (not res->empty()) {
            delete res->back();
            res->pop_back();
        }
        delete res; 
    }   


    void mergeGraph(GraphWeighted* A, GraphWeighted* B)
    {
        A->sort();
        B->sort();

        auto itA = A->begin();
        auto itB = B->begin();

        while (itA != A->end() and itB != B->end()) {
            if ( (*itA)->getId() == (*itB)->getId()) {
                for (auto adjB = (*itB)->wAdjacentsBegin(); adjB != (*itB)->wAdjacentsEnd(); adjB++) {
                    if (not (*itA)->increaseWeight((*adjB).first, (*adjB).second)){
                        (*itA)->addAdjacent((*adjB).first, (*adjB).second); 
                    }
                }
                (*itA)->sort();
                itB++;
                itA++;
            } else if ((*itB)->getId() < (*itA)->getId()) {
                A->insert(*itB);
                (*itB) = nullptr;
                itB++;
            } else {
                itA++;
            }
        }
        A->sort();
        delete B; 
        
    }

    GraphWeighted* biclique_pow(CompactBiclique* B, GraphWeighted* res)
    {
        if (res == nullptr) res = new GraphWeighted();

        auto partialRes = new std::vector<std::vector<std::pair<uInt,uInt>>*>();
        TIMERSTART(COMPUTE_BXB);
        for (size_t i = 0; i < B->biclique_collection.size(); i++) {
            for (size_t j = 0; j < B->biclique_collection.size(); j++) {
                if (i==0) {
                    partialRes->push_back(new std::vector<std::pair<uInt,uInt>>());
                }
                if (i != j) {
                    auto intersection = checkIntersection(B->biclique_collection.at(j), B->biclique_collection.at(i)); 

                    if (debug) {
                        std::cout << "intersection: "; 
                        for (auto k : *intersection) {
                            std::cout << k.first << " "; 
                        }
                        std::cout << std::endl; 
                    }

                    if (intersection->size() > 0 ) {
                        //auto vec = partialRes->back();
                        
                        //partialRes->push_back(vec);
                        for (auto c : *(B->biclique_collection.at(i)->C)) {
                            uInt value = 0;
                            for (auto inter : *intersection)  {
                                value += inter.second * c.second;   
                            }
                            //vec->push_back(make_pair(c.first, value)); 
                            partialRes->at(j)->push_back(make_pair(c.first, value)); 
                        }
                
                    }
                    delete intersection; 
                }
            }
        } 
        TIMERSTOP(COMPUTE_BXB);

        if (debug){
            uInt counter = 0; 
            for (auto i : *partialRes) {
                std::cout << "R" << counter << ":" ;
                for (auto j : *i) {
                    std::cout << "(" << j.first << "," << j.second << ") " ;
                }
                std::cout << std::endl;
                counter++;
                
            }
        }
        TIMERSTART(APPEND_BXB);
        appendInGraph(partialRes,B,res);
        TIMERSTOP(APPEND_BXB);

        return res; 
    }


    std::vector<std::pair<uInt, uInt>>* checkIntersection(Biclique* A, Biclique* B) 
    {
        auto it_S = A->S->begin();
        auto it_C = B->C->begin(); 

        auto intersection = new std::vector<std::pair<uInt, uInt>>();

        if (A->S->empty() or B->C->empty()) {
            return intersection;
        }

        if (A->S->back() <  A->C->front().first) {
            return intersection;
        }

        while (it_S != A->S->end() and it_C != B->C->end()) {
            if ( *it_S == (*it_C).first ) {
                intersection->push_back((*it_C)); 
                it_S++;
                it_C++;
            } else if ( *it_S < (*it_C).first ) {
                it_S++; 
            } else {
                it_C++; 
            }
        }
        return intersection; 
    }
    #ifdef sdsl_f
    CompactBiclique *load_CompactBiclique_sdsl(std::string path)
    {
        if (not validateExtension(path, "sdsl")) {
            return nullptr;
        }
        auto compBicl = new CompactBiclique();
        while(path.back() != '_'){
            path.pop_back();
        }

        sdsl::wm_int<sdsl::rrr_vector<15>> c_sdsl = w_sdsl::readCompressedInt(path + "C.sdsl");
        sdsl::rrr_vector<15> cc_sdsl = w_sdsl::readCompressedBitmap(path + "CC-rrr-64.sdsl");
        sdsl::wm_int<sdsl::rrr_vector<15>> s_sdsl = w_sdsl::readCompressedInt(path + "S.sdsl");
        sdsl::rrr_vector<15> ss_sdsl = w_sdsl::readCompressedBitmap(path + "SS-rrr-64.sdsl");

        sdsl::rrr_vector<15>::select_1_type cc_select(&cc_sdsl);
        sdsl::rrr_vector<15>::select_1_type ss_select(&ss_sdsl);

        sdsl::rrr_vector<15>::rank_1_type cc_rank(&cc_sdsl);
        sdsl::rrr_vector<15>::rank_1_type ss_rank(&ss_sdsl);

        uInt cc_size = cc_rank.size();
        //std::cout << "cc_size " << cc_size << std::endl;
        uInt total_rank = cc_rank.rank(cc_size);
        //std::cout << "total rank" << total_rank << std::endl;


        uInt start = 0; 
        uInt size = 0;
        uInt end = 0; 

        for (size_t i = 1; i <= total_rank; i++) {
            if (i != total_rank) {
                size = cc_select(i+1) - cc_select(i) - 1 ;
                end = start + (size*2); 
            } else {
                end = c_sdsl.size();
            }
            
            auto vec = new C_Values();
            auto bicl = new Biclique();
            bicl->setC(vec);

            for (size_t j = start; j < end; j+=2) {
                vec->push_back(make_pair(c_sdsl[j], c_sdsl[j+1]));
            }

            compBicl->c_collection.push_back(vec);
            compBicl->biclique_collection.push_back(bicl);
            start = end; 
        }

        uInt ss_size = ss_rank.size();
        //std::cout << "ss_size " << ss_size << std::endl;
        total_rank = ss_rank.rank(ss_size);
        //std::cout << "total rank" << total_rank << std::endl;

        start = 0;
        end = 0;

        for (size_t i = 1; i <= total_rank; i++) {
            if (i != total_rank) { 
                end = ss_select(i+1); 
            } else {
                end = s_sdsl.size();

            }
            vector<uInt> vec;
            uInt index = s_sdsl[start];
            //std::cout << "index: " << index << std::endl; 
            for(size_t j = start + 1; j < end; j++) {
                //std::cout << s_sdsl[j] << " " ; 
                vec.push_back(s_sdsl[j]);
                compBicl->biclique_collection.at(s_sdsl[j])->S->push_back(index);
            }
            compBicl->linked_s.push_back(make_pair(index, vec));
            start = end; 
            //std::cout << std::endl << "****" << std::endl;
        }
        return compBicl;
    }
    #endif
}
