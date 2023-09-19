#include "matrix_multiplication.hpp"
#include "boost_multiplication.hpp"

int main(int argc, char const *argv[])
{

    std::unordered_map<std::string, std::string> input_arguments{
        {"graphA", ""},
        {"graphB", ""}, 
        {"compact", ""},
        {"biclique", ""}, 
        {"boost", "0"},
        {"optimize", "0"},
        {"weighted", "1"},
        {"print", "0"},
        {"buildTxt", "0"}
        };

    auto arguments = parseArguments(argc, argv, &input_arguments);

    /*
    boost::ublas::matrix<uInt> matrixA;
    boost::ublas::matrix<uInt> matrixB;
    boost::ublas::matrix<uInt> biclique_b;
    boost::ublas::matrix<uInt> C;

    if (argc == 1) return 0;
    if (argc >= 2) matrixA = boost::createMatrix(argv[1]);
    if (argc >= 3) matrixB = boost::createMatrix(argv[2]);
    if (argc >= 4) biclique_b = boost::createMatrix(argv[3]); 
    

    TIMERSTART(BOOST_MULTIPLICATION);
    if (argc == 2) {
        C = boost::ublas::prod(matrixA, matrixA);
    } else if (argc == 3) {
        boost::normalizeSize(&matrixA, &matrixB);
        std::cout << "normalized" << std::endl;
        C = boost::ublas::prod(matrixA, matrixB);
    } else if (argc == 4) {
        auto C_temp = boost::ublas::prod(matrixA, matrixB); 
        auto C_b_temp = boost::ublas::prod(biclique_b, matrixB);
        C = C_temp + C_b_temp;
    }
    //std::cout << std::endl << "Matrix result" << std::endl;
    //printMatrix(C);
    TIMERSTOP(BOOST_MULTIPLICATION);

    */

    
    
    
    
    vector<Lisa::Biclique*>* std_bicliques = nullptr; 
    Lisa::CompactBicliqueWeighted* compBicl = nullptr;

    if (arguments["biclique"].size() > 0) {
        std_bicliques = Lisa::load_biclique_w(arguments["biclique"]);
    }

    if (arguments["compact"].size() > 0) {
        compBicl = Lisa::load_CompactBiclique_w(arguments["compact"]);
    }

    bool optimize = atoi(arguments["optimize"].c_str());
    bool boost = atoi(arguments["boost"].c_str());
    bool weighted = atoi(arguments["weighted"].c_str());
    bool print = atoi(arguments["print"].c_str());
    bool buildTxt = atoi(arguments["buildTxt"].c_str());

    if (weighted) {
        GraphWeighted* A = nullptr; 
        GraphWeighted* B = nullptr; 
        GraphWeighted* C = nullptr;

        if (arguments["graphA"].size() > 0) A = new GraphWeighted(arguments["graphA"]); 
        if (arguments["graphB"].size() > 0) B = new GraphWeighted(arguments["graphB"]);


        if (A == nullptr and B == nullptr) {
            std::cerr << "No graph file" << std::endl; 
            return 0;
        }
        
        if ((A != nullptr and B == nullptr) or (A == nullptr and B != nullptr)) {
            if (A == nullptr) {
                A = new GraphWeighted(arguments["graphB"]);
            } else {
                B = new GraphWeighted(arguments["graphA"]); 
            }
            if (compBicl) {
                std::cout << "case 1" << std::endl;
                TIMERSTART(compact_struct);
                C = Lisa::matrix_multiplication_w(A, B, compBicl, optimize);
                TIMERSTOP(compact_struct)
                if (print) C->print();
                delete C;
            }
            if (std_bicliques) {
                std::cout << "case 2" << std::endl;
                TIMERSTART(stdbicliques);
                C = Lisa::matrix_multiplication_w(A, B, std_bicliques, optimize);
                TIMERSTOP(stdbicliques);
                if (print) C->print();
                delete C; 
            } 
            if (compBicl == nullptr and std_bicliques == nullptr) {
                std::cout << "case 3" << std::endl;
                TIMERSTART(standard);
                C = Lisa::matrix_multiplication_w(A, B, optimize);
                TIMERSTOP(standard);
                if (print) C->print();
                delete C; 
            }
        } 

        if (A != nullptr and B != nullptr) {
            if (compBicl) { 
                std::cout << "case 4" << std::endl;
                TIMERSTART(compact_struct);
                C = Lisa::matrix_multiplication_w(A, B, compBicl, optimize);
                TIMERSTOP(compact_struct);
                if (print) C->print();
                delete C;
            }
            if (std_bicliques) {
                std::cout << "case 5" << std::endl;
                TIMERSTART(stdbicliques);
                C = Lisa::matrix_multiplication_w(A, B, std_bicliques, optimize);                 
                TIMERSTOP(stdbicliques);
                if (print) C->print();
                delete C;
            } 
            if (not compBicl and not std_bicliques) {
                std::cout << "case 6" << std::endl;
                TIMERSTART(standard);
                C = Lisa::matrix_multiplication_w(A, B, optimize);
                TIMERSTOP(standard);
                if (print) C->print();
                delete C;
            }
        }

        delete A;
        delete B; 
    }
    
    
    /*

    // A Declaration 
    GraphWeighted* A = new GraphWeighted();
    Node* n; 

    n = new Node(0, true);
    A->insert(n);

    n = new Node(1, true); 
    n->addAdjacent(2,2);
    n->addAdjacent(3,3);
    n->addAdjacent(5,5);
    n->addAdjacent(6,6);
    n->addAdjacent(8,8);
    A->insert(n);
    
    n = new Node(2, true);
    n->addAdjacent(3,3);
    n->addAdjacent(5,5);
    n->addAdjacent(6,6);
    n->addAdjacent(7,7);
    A->insert(n);

    n = new Node(3, true);
    n->addAdjacent(1,1);
    n->addAdjacent(4,4);
    n->addAdjacent(5,5);
    n->addAdjacent(8,8);
    A->insert(n);

    n = new Node(4, true);
    n->addAdjacent(1,1);
    n->addAdjacent(2,2);
    n->addAdjacent(3,3);
    n->addAdjacent(5,5);
    n->addAdjacent(6,6);
    A->insert(n);

    n = new Node(5, true);
    n->addAdjacent(2,2);
    n->addAdjacent(3,3);
    n->addAdjacent(4,4);
    n->addAdjacent(8,8);
    A->insert(n);

    n = new Node(6, true);
    n->addAdjacent(1,1);
    n->addAdjacent(3,3);
    n->addAdjacent(4,4);
    n->addAdjacent(5,5);
    n->addAdjacent(6,6);
    n->addAdjacent(8,8);
    A->insert(n);

    n = new Node(7, true);
    n->addAdjacent(1,1);
    n->addAdjacent(2,2);
    n->addAdjacent(3,3);
    n->addAdjacent(4,4);
    n->addAdjacent(5,5);
    n->addAdjacent(6,6);
    n->addAdjacent(8,8);
    A->insert(n);

    //A->printAsMatrix();
    
    // B declaration 
    auto B = new GraphWeighted();

    n = new Node(0, true);
    B->insert(n);

    n = new Node(1, true);
    n->addAdjacent(4,4);
    n->addAdjacent(5,5);
    n->addAdjacent(6,6);
    B->insert(n);

    n = new Node(2, true);
    n->addAdjacent(1,1);
    n->addAdjacent(3,3);
    n->addAdjacent(7,7);
    B->insert(n);

    n = new Node(3, true);
    n->addAdjacent(2,2);
    n->addAdjacent(5,5);
    n->addAdjacent(8,8);
    B->insert(n);

    n = new Node(4, true);
    n->addAdjacent(3,3);
    n->addAdjacent(5,5);
    n->addAdjacent(7,7);
    B->insert(n);

    n = new Node(5, true);
    n->addAdjacent(1,1);
    n->addAdjacent(3,3);
    n->addAdjacent(6,6);
    B->insert(n);

    n = new Node(6, true);
    n->addAdjacent(1,1);
    n->addAdjacent(4,4);
    n->addAdjacent(5,5);
    B->insert(n);

    n = new Node(7, true); 
    n->addAdjacent(2,2);
    n->addAdjacent(6,6);
    n->addAdjacent(8,8);
    B->insert(n);

    //B->printAsMatrix();


    auto C = Lisa::matrix_multiplication_w(A, B, true);
    //C->printAsMatrix();
    delete C;   
    //std::cout << "*****" << endl;
    C = Lisa::matrix_multiplication_w(A, B, false);
    C->printAsMatrix();
    cout << "*****" << endl;
    delete C;



    auto A_b = new GraphWeighted();

    n = new Node(0, true);
    A_b->insert(n);

    n = new Node(1, true);
    n->addAdjacent(2,2);
    n->addAdjacent(8,8);
    A_b->insert(n);

    n = new Node(2, true);
    n->addAdjacent(7,7);
    A_b->insert(n);

    n = new Node(3, true);
    n->addAdjacent(5,5);
    A_b->insert(n);

    n = new Node(4,true);
    n->addAdjacent(1,1);
    n->addAdjacent(2,2);
    A_b->insert(n);

    n = new Node(5,true); 
    n->addAdjacent(2,2);
    n->addAdjacent(3,3);
    n->addAdjacent(4,4);
    n->addAdjacent(8,8);
    A_b->insert(n);

    n = new Node(7, true);
    n->addAdjacent(2,2);
    A_b->insert(n);

    //A_b->printAsMatrix();

 


    Lisa::Biclique b1; 

    b1.S.push_back(1);
    b1.S.push_back(2);
    b1.S.push_back(4);
    b1.S.push_back(6);
    b1.S.push_back(7);

    b1.C_w.push_back(make_pair(3,3));
    b1.C_w.push_back(make_pair(5,5));
    b1.C_w.push_back(make_pair(6,6));

    Lisa::Biclique b2; 

    b2.S.push_back(3);
    b2.S.push_back(6);
    b2.S.push_back(7);

    b2.C_w.push_back(make_pair(1,1));
    b2.C_w.push_back(make_pair(4,4));
    b2.C_w.push_back(make_pair(8,8));


    auto bicl = new vector<Lisa::Biclique*>();
    bicl->push_back(&b1);
    bicl->push_back(&b2);

    TIMERSTART(normal)
    auto C_res = Lisa::matrix_multiplication_w(A_b, B, bicl, true);
    TIMERSTOP(normal);

    cout << "&&&&" << endl;
    C_res->printAsMatrix();
    cout << "&&&&" << endl;


    

    Lisa::CompactBicliqueWeighted compBicl; 

    compBicl.weights_values.insert(0);
    compBicl.weights_values.insert(1);
    compBicl.weights_values.insert(2);
    compBicl.weights_values.insert(3);
    compBicl.weights_values.insert(4);
    compBicl.weights_values.insert(5);
    compBicl.weights_values.insert(6);
    compBicl.weights_values.insert(7);
    compBicl.weights_values.insert(8);

    Lisa::CompactBicliqueWeighted::C_values b1_comp; 

    b1_comp.push_back(make_pair(3,3));
    b1_comp.push_back(make_pair(5,5));
    b1_comp.push_back(make_pair(6,6));
    
    Lisa::CompactBicliqueWeighted::C_values b2_comp; 

    b2_comp.push_back(make_pair(1,1));
    b2_comp.push_back(make_pair(4,4));
    b2_comp.push_back(make_pair(8,8));

    compBicl.c_bicliques.push_back(b1_comp);
    compBicl.c_bicliques.push_back(b2_comp);


    compBicl.linked_s.push_back(make_pair(1, vector<uInt>{0}));
    compBicl.linked_s.push_back(make_pair(2, vector<uInt>{0}));
    compBicl.linked_s.push_back(make_pair(3, vector<uInt>{1}));
    compBicl.linked_s.push_back(make_pair(4, vector<uInt>{0}));
    compBicl.linked_s.push_back(make_pair(6, vector<uInt>{0,1}));
    compBicl.linked_s.push_back(make_pair(7, vector<uInt>{0,1}));

    //Lisa::printCompactStructure(&compBicl);

    TIMERSTART(comp);
    auto C_comp = Lisa::matrix_multiplication_w(A_b, B, &compBicl, false); 
    TIMERSTOP(comp);
    C_comp->printAsMatrix();








    */







    /*
    Test Multiplicacion sin pesos
    Graph A;
    Graph B;
    Node* n; 

    n = new Node(0);
    A.insert(n);

    n = new Node(1);
    n->addAdjacent(3);
    A.insert(n);

    n = new Node(2);
    n->addAdjacent(1);
    n->addAdjacent(3);
    A.insert(n);

    n = new Node(3);
    n->addAdjacent(2);
    A.insert(n);

    n = new Node(4);
    n->addAdjacent(2);
    n->addAdjacent(3);
    A.insert(n);

    n = new Node(0);
    B.insert(n);

    n = new Node(1);
    n->addAdjacent(2);
    B.insert(n);

    n = new Node(2);
    n->addAdjacent(3);
    n->addAdjacent(4);
    B.insert(n);

    n = new Node(3);
    n->addAdjacent(1);
    n->addAdjacent(2);
    B.insert(n);

    n = new Node(4);
    n->addAdjacent(1);
    n->addAdjacent(3);
    B.insert(n);

    A.printAsMatrix();
    B.printAsMatrix();


    cout << " *****  " << endl; 
    cout << A.maxValueEdge() << endl;
    auto C = matrix_multiplication(&A,&B);
    C->printAsMatrix();
    C->print();

    */

    /*
    Test Multiplicacion con pesos
    GraphWeighted A;
    GraphWeighted B;
    Node* n; 

    n = new Node(0, true);
    A.insert(n);

    n = new Node(1, true);
    n->addAdjacent(3,1);
    A.insert(n);

    n = new Node(2, true);
    n->addAdjacent(1,4);
    n->addAdjacent(3,2);
    A.insert(n);

    n = new Node(3, true);
    n->addAdjacent(2,2);
    A.insert(n);

    n = new Node(4, true);
    n->addAdjacent(2,1);
    n->addAdjacent(3,4);
    A.insert(n);

    n = new Node(0, true);
    B.insert(n);

    n = new Node(1, true);
    n->addAdjacent(2,3);
    B.insert(n);

    n = new Node(2, true);
    n->addAdjacent(3,1);
    n->addAdjacent(4,2);
    B.insert(n);

    n = new Node(3, true);
    n->addAdjacent(1,2);
    n->addAdjacent(2,1);
    B.insert(n);

    n = new Node(4, true);
    n->addAdjacent(1,3);
    n->addAdjacent(3,2);
    B.insert(n);

    A.printAsMatrix();
    B.printAsMatrix();


    cout << " *****  " << endl; 
    cout << A.maxValueEdge() << endl;

    auto C = matrix_multiplication_w(&A,&B);
    C->printAsMatrix();
    C->print();
    */


    
    //Test con bicliques con pesos.

    // Con A un grafo al cual ya se le extrajeron los bicliques, 
    // B un grafo cualquiera.
    // y X un Biclique(S,C).
    // Realizar una multiplicacion vector matriz con CxB y luego adicionar a cada par resultante en AxB presente en S. 


    /*
    GraphWeighted A; 
    GraphWeighted B;
    vector<Lisa::Biclique*>* bicliques = new vector<Lisa::Biclique*>();
    bicliques->push_back(new Lisa::Biclique());

    //Build A 
    Node* n = new Node(1, true);
    n->addAdjacent(3,3);
    n->addAdjacent(4,4);
    A.insert(n);

    n = new Node(2, true);
    //n->addAdjacent(1,1);
    //n->addAdjacent(3,3);
    //n->addAdjacent(4,4);
    n->addAdjacent(6,6); 
    A.insert(n);

    n = new Node(3, true);
    n->addAdjacent(2,2);
    A.insert(n);

    n = new Node(4, true);
    n->addAdjacent(2,2);
    n->addAdjacent(3,3);
    A.insert(n);

    n = new Node(5, true);
    //n->addAdjacent(1,1);
    n->addAdjacent(2,2);
    //n->addAdjacent(3,3);
    //n->addAdjacent(4,4);
    A.insert(n);

    n = new Node(6, true);
    n->addAdjacent(3,3);
    n->addAdjacent(4,4);
    A.insert(n);

    // Build B

    n = new Node(1, true);
    n->addAdjacent(3,3);
    n->addAdjacent(5,5);
    B.insert(n);

    n = new Node(2, true);
    n->addAdjacent(1,1);
    n->addAdjacent(3,3);
    n->addAdjacent(5,5);
    B.insert(n);

    n = new Node(3, true);
    n->addAdjacent(1,1);
    n->addAdjacent(2,2);
    n->addAdjacent(4,4);
    B.insert(n);

    n = new Node(4,true);
    n->addAdjacent(1,1);
    n->addAdjacent(3,3);
    B.insert(n);

    n = new Node(5, true);
    n->addAdjacent(2,2);
    n->addAdjacent(3,3);
    B.insert(n);


    // Build Biclique
    bicliques->front()->S.push_back(2);
    bicliques->front()->S.push_back(5); 

    bicliques->front()->C_w.push_back(make_pair(1,1));
    bicliques->front()->C_w.push_back(make_pair(3,3));
    bicliques->front()->C_w.push_back(make_pair(4,4));

    //
    cout << "Matriz A: " << endl; 
    A.printAsMatrix();

    cout << endl << "Matriz B: " << endl;
    B.printAsMatrix();
    
    

    //cout << endl << "Biclique: " << endl; 
    //Lisa::printBicliqueAsMatrix(bicliques->front());
    //cout << endl; 

    //auto C = Lisa::matrix_multiplication_w(&A, &B, bicliques);
    //C->printAsMatrix();
    

    cout << "Res:" << endl;
    TIMERSTART(one);
    auto D = Lisa::matrix_multiplication_w(&A, &B); 
    TIMERSTOP(one);
    D->printAsMatrix();

    cout << endl << "Matrix B transpose" << endl;
    B.transpose(); 
    B.printAsMatrix();
    
    cout << "Res: " << endl;
    TIMERSTART(two);
    auto G = Lisa::matrix_multiplication_w(&A, &B, true); 
    TIMERSTOP(two);
    G->printAsMatrix();

    //auto F = Lisa::vector_matrix_multiplication_w(bicliques->front()->C_w, &B);
    //for (auto i : *F) {
    //    cout << "(" <<i.first << "," << i.second << ") ";
    //}
    cout << endl;    
    
    */
    //if (argc != 4) return 0; 

    return 0;
}


//./matrix_multiplicator ../datasets/dblp-2011_weighted.txt ../datasets/dblp-2011_weighted.txt ../datasets/dblp-2011_weighted_bicliques.txt