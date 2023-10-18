#include "matrix_multiplication.hpp"

#include "string.h"

const std::string modes[2] = {"pow", "mult"};

using namespace mw; 

int main(int argc, char const *argv[])
{
    std::unordered_map<std::string, std::string> input_arguments{
        {"graph", ""}, 
        {"graphB", ""}, 
        {"compact", ""},
        {"biclique", ""}, 
        {"print", "0"},
        {"buildFile", "0"}, 
        {"mode", "pow"}
        };

    auto arguments = parseArguments(argc, argv, &input_arguments);
    bool bin = false;

    std::cout << "**** Params ****" << std::endl; 
    for(auto i : arguments) {
        std::cout << i.first << ": " << i.second << std::endl;
    }
    std::cout << "****************" << std::endl;

    #if defined(parallel)
        std::cout << "using " << THREADS << " threads" << std::endl;
    #endif

    assert(arguments["graph"].size() > 0);
    
    if(arguments["graph"].find(".bin") != std::string::npos) {
            bin = true; 
    } else if (arguments["graph"].find(".txt") != std::string::npos) {
            bin = false;
    }

    GraphWeighted* A = nullptr; 
    GraphWeighted* B = nullptr;
    GraphWeighted* C = nullptr; 
    CompactBiclique* compact = nullptr;

    if (strcmp(arguments["mode"].c_str(), modes[0].c_str()) == 0) {
        A = new GraphWeighted(arguments["graph"]); 
        if (arguments["compact"].size() > 0)  {
            if (bin) {
                compact = load_CompactBiclique_bin(arguments["compact"]);
            } else {
                compact = load_CompactBiclique(arguments["compact"]);
            }
        }
        if (compact != nullptr) {
            std::cout << "*** Pow with compact bicliques***" << std::endl;

            TIMERSTART(POW_COMPACT_BICLIQUES);
            C = matrix_pow(A, compact);
            TIMERSTOP(POW_COMPACT_BICLIQUES);
        }
        else {
            std::cout << "*** Pow default ***" << std::endl;
            std::cout << arguments["graph"] << std::endl;
            
            B = new GraphWeighted(arguments["graph"]);
            //B->print();
            B->transpose();
            TIMERSTART(POW_DEFAULT);
            C = matrix_multiplication(A,B);
            TIMERSTOP(POW_DEFAULT);
        }
    } else if (strcmp(arguments["mode"].c_str(), modes[0].c_str()) == 0) {
        assert(arguments["graphB"].size() > 0); 
    }

    if (arguments["print"] == "1") C->print();

    if (A != nullptr) delete A;
    if (B != nullptr) delete B;
    if (C != nullptr) delete C;
    if (compact != nullptr) delete compact;
    
    

    //cout << "##########" << endl;
    //C->printAsMatrix();
    //cout << "******" << endl;
    //C->print();
    //C->setPath("res.txt");
    //C->writeAdjacencyList();



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