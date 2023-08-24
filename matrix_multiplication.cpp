#include "matrix_multiplication.hpp"

Biclique* load_biclique(std::string) 
{
    return nullptr;
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

GraphWeighted* matrix_multiplication_w(GraphWeighted* A, GraphWeighted* B) 
{
    GraphWeighted* C = new GraphWeighted();
    Node* temp = nullptr; 
    uint64_t jMax = A->maxValueEdge(); 
    for (size_t i = 0; i < A->size(); i++) {
        Node* Ai = A->at(i);
        temp = new Node(Ai->getId(), true);
        C->insert(temp);
        for(size_t j = 0; j <= jMax + 1; j++) {
            uint64_t sum = 0; 
            for(auto Aik = Ai->wAdjacentsBegin(); Aik != Ai->wAdjacentsEnd(); Aik++) {
                //if(sw) break; 
                for (size_t k = 0; k < B->size(); k++) {
                    //if(sw) break;
                    Node* Bk = B->at(k);
                    if (Bk->getId() == (*Aik).first) {
                        //cout << "Node: " << temp->getId() << endl;
                        //cout << "j: " << j << endl;
                        //cout << "Aik: " << *Aik << endl;
                        //cout << "Bk: " << Bk->getId() << endl;
                        //cout << "Search: " << j << " in " << Bk->getId() << endl;  
                        auto Bkj_w = Bk->findAdjacentWeighted(j);
                        if (Bkj_w > 0) {
                            sum += (*Aik).second * Bkj_w; 
                        } 
                    }
                }
            }
            //cout << "push: " << temp->getId() << endl;
            //cout << j << " " << sum << endl;
            if(sum > 0) {
                temp->addAdjacent(j, sum);
            }
        }
    }
    return C;
}

GraphWeighted* matrix_multiplication_w(GraphWeighted* A, GraphWeighted* B, Biclique* X)
{   
    assert(not X->S.empty());
    assert(X->C.empty());
    assert(not X->C_w.empty());

    auto C = matrix_multiplication_w(A, B); 
    
    cout << "A-biclique x B: " << endl; 
    C->printAsMatrix();

    cout << endl << "vector x B " << endl; 
    auto vec = vector_matrix_multiplication_w(X->C_w, B); 
    for (auto i : *vec) {
        cout << "(" << i.first << "," << i.second << ") " ;
    }
    cout << endl;

    for (auto i : X->S) {
        auto node = C->find(i); 

        if (node == nullptr) {
            node = new Node(i, true); 
            C->insert(node);
        }
        for (auto j : *vec) {
            if ( not node->increaseWeight(j.first, j.second) ) {
                node->addAdjacent(j.first, j.second);
            }
        }
        node->sort();
    }

    cout << endl <<"C Matrix: " << endl; 
    C->printAsMatrix();

    return C;
}

vector<pair<uInt,uInt>>* vector_matrix_multiplication_w(vector<pair<uInt, uInt>> v, GraphWeighted * B)
{
    vector<pair<uInt,uInt>>* res = new vector<pair<uInt,uInt>>();
    Graph* C = new Graph();
    uint64_t jMax = v.back().first; 
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

Graph *matrix_multiplication(Graph* A, Graph* B, Biclique* X)
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
