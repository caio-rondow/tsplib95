#include <iostream>
#include <string>
#include <fstream>

void print_matrix(int **matrix, int n){
    std::cout << n << "\n";
    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            std::cout << matrix[i][j] << " ";
        }
        std::cout << "\n";
    }
}

void load_problem(int **& matrix, int& n, const std::string&filename){

    /* open file */
    std::ifstream fin(filename);
    if(!fin.is_open()){
        std::cerr << "Could not open " << filename << "\n";
        exit(1);
    }

    /* get first line */
    std::string row;
    std::getline(fin,row);
    n = stoi(row);
    
    /* allocate matrix */
    matrix = new int*[n];
    for(int i=0; i<n; i++){
        matrix[i] = new int[n];
    }

    /* init matrix */
    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            fin >> matrix[i][j];
        }
    }

    fin.close();
}

void solveTSPP(int **distance, int **penalty, int n){
    /* first route 1,2,...,n */
    int route[n];
    for(int i=0; i<n; i++){
        route[i] = i;
    }
}

int main(int argc, char **argv){

    /* read distance and penalty matrix */ 
    std::string problem_name = argv[1];
    std::string penalty_name = argv[2];

    int **distance;
    int **penalty;
    int n;

    /* init matrices */
    load_problem(distance, n, problem_name);
    load_problem(penalty, n, penalty_name);
    // print_matrix(distance, n);
    // print_matrix(penalty, n);

    solveTSPP(distance, penalty, n);

    for(int i=0; i<n; i++){
        delete[] distance[i];
        delete[] penalty[i];
    }
    delete[] distance;
    delete[] penalty;

    return 0;
}