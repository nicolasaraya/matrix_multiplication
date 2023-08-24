#include "matrix_multiplication.hpp"

int main(int argc, char const *argv[])
{
    load_biclique("");
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

    /*
    Con A un grafo al cual ya se le extrajeron los bicliques, 
    B un grafo cualquiera.
    Y X un Biclique(S,C).
    Realizar una multiplicacion vector matriz con CxB y luego adicionar a cada par resultante en AxB presente en S. 
    */


    GraphWeighted A; 
    GraphWeighted B;
    Biclique X; 

    //Build A 
    Node* n = new Node(1, true);
    n->addAdjacent(3,3);
    n->addAdjacent(4,4);
    A.insert(n);

    n = new Node(2, true);
    /*
    n->addAdjacent(1,1);
    n->addAdjacent(3,3);
    n->addAdjacent(4,4);
    */
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
    /*
    n->addAdjacent(3,3);
    n->addAdjacent(4,4);
    */
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
    X.S.push_back(2);
    X.S.push_back(5); 

    X.C_w.push_back(make_pair(1,1));
    X.C_w.push_back(make_pair(3,3));
    X.C_w.push_back(make_pair(4,4));

    //
    cout << "Matriz A: " << endl; 
    A.printAsMatrix();

    cout << endl << "Matriz B: " << endl;
    B.printAsMatrix();
     
    cout << endl << "Biclique: " << endl; 
    printBicliqueAsMatrix(&X);
    cout << endl; 

    matrix_multiplication_w(&A, &B, &X);

    

    return 0;
}
