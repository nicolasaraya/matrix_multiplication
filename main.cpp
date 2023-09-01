#include "matrix_multiplication.hpp"
#include "boost_multiplication.hpp"

int main(int argc, char const *argv[])
{

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



    /*
    GraphWeighted* A = nullptr;
    GraphWeighted* B = nullptr;
    vector<Lisa::Biclique*>* bicliques = nullptr;
    if (argc >= 2) A = new GraphWeighted(argv[1]); 
    if (argc >= 3) B = new GraphWeighted(argv[2]);
    if (argc >= 4) bicliques = Lisa::load_biclique_w(argv[3]);

    //A->print();
    //A->printAsMatrix();
    TIMERSTART(mult);
    if(argc < 4) {
        auto C = Lisa::matrix_multiplication_w(A, B);
        //C->print();
    } else if (argc >= 4) {
        auto C = matrix_multiplication_w(A, B, bicliques);
        C->print();
        //C = matrix_multiplication_w(A,B);
        //C->print();
        for (auto j : *bicliques) {
            cout << "S: " ;
            for(auto k : j->S) {
                cout << k << " ";
            }
            cout << endl << "C: ";
            for(auto k : j->C_w) {
                cout << "(" << k.first << "," << k.second << ")" ;
            }
            cout << endl;
            auto v = Lisa::vector_matrix_multiplication_w(j->C_w, B);
            for (auto k : *v) {
                cout << "(" << k.first << "," << k.second << ") ";
            }
            cout << endl;
        }
        
    }
    TIMERSTOP(mult);
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


    /*
    //Test con bicliques con pesos.

    // Con A un grafo al cual ya se le extrajeron los bicliques, 
    // B un grafo cualquiera.
    // y X un Biclique(S,C).
    // Realizar una multiplicacion vector matriz con CxB y luego adicionar a cada par resultante en AxB presente en S. 

    GraphWeighted A; 
    GraphWeighted B;
    vector<Biclique*>* bicliques = new vector<Biclique*>();
    bicliques->push_back(new Biclique());

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

    cout << endl << "Biclique: " << endl; 
    printBicliqueAsMatrix(bicliques->front());
    cout << endl; 

    auto C = matrix_multiplication_w(&A, &B, bicliques);
    C->printAsMatrix();
    

    auto D = matrix_multiplication_w(&A, &B); 
    D->printAsMatrix();
    
    auto F = vector_matrix_multiplication_w(bicliques->front()->C_w, &B);
    for (auto i : *F) {
        cout << "(" <<i.first << "," << i.second << ") ";
    }
    cout << endl;    
    */ 

    //if (argc != 4) return 0; 

    return 0;
}


//./matrix_multiplicator ../datasets/dblp-2011_weighted.txt ../datasets/dblp-2011_weighted.txt ../datasets/dblp-2011_weighted_bicliques.txt