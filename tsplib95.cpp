#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

void print_matrix(const vector<vector<int>>&matrix, int n){
    std::cout << n << "\n";
    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            std::cout << matrix[i][j] << " ";
        }
        std::cout << "\n";
    }
}

vector<vector<int>> load_problem(const std::string&filename){

    /* open file */
    std::ifstream fin(filename);
    if(!fin.is_open()){
        std::cerr << "Could not open " << filename << "\n";
        exit(1);
    }

    /* get first line */
    std::string row;
    std::getline(fin,row);
    int n = stoi(row);
    
    /* create matrix */
    vector<vector<int>> matrix(n,vector<int>(n));
    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            fin >> matrix[i][j];
        }
    }

    fin.close();

    return matrix;
}

/* calcula o custo inicial */
int path_cost(const vector<int>&route, const vector<vector<int>>&distance, const vector<vector<int>>&penalty, int n){
    int cost = 0;
    int src, tar;
    
    for(int i=0; i<n; i++){
        src = route[i];
        tar = route[(i+1)%n];
        cost += distance[src][tar];
    }

    return cost;
}

void solve_TSPP(const vector<vector<int>>&distance, const vector<vector<int>>&penalty, int n){

    vector<int> route(n);
    bool isImproving=true;

    /* initial solution */
    for(int i=0; i<n; i++){
        route[i] = i;
    }
    
    /* initial cost */
    int initial_cost = path_cost(route,distance,penalty,n);
    int curr_cost = initial_cost;
    int reverse_beg, reverse_end;

    std::cout << "initial cost: " << initial_cost << "\n";
    while(isImproving){

        isImproving=false;
        
        /* search in the neighborhood space */
        for(int i=0; i<n-2; i++){  
            for(int j=i+2; j<n; j++){

                /* calculate 2opt new cost */
                int solution_cost = initial_cost+
                                    -distance[ route[i] ][ route[(i+1)%n] ]
                                    -distance[ route[j] ][ route[(j+1)%n] ]
                                    +distance[ route[i] ][ route[j] ]
                                    +distance[ route[(i+1)%n] ][ route[(j+1)%n] ];
                /* found better solution */
                if(solution_cost < curr_cost){
                    reverse_beg = (i+1)%n;
                    reverse_end = j;
                    curr_cost = solution_cost;
                    isImproving = true;
                }
            }
        }

        /* set new best solution */
        if(isImproving){
            reverse(route.begin()+reverse_beg, route.begin()+reverse_end+1);
            initial_cost = curr_cost;
        }
    }

    cout << "best cost: " << curr_cost << "\n";
    cout << "best route: ";
    for(int i=0; i<n; i++){
        cout << route[i] << " -> ";
    }
    cout << "\n";
}

int main(int argc, char **argv){

    /* read distance and penalty matrix */ 
    std::string problem_name = argv[1];
    std::string penalty_name = argv[2];

    /* init matrices */
    vector<vector<int>> distance = load_problem(problem_name);
    vector<vector<int>> penalty  = load_problem(penalty_name);
    // print_matrix(distance, distance.size());
    // print_matrix(penalty, penalty.size());

    solve_TSPP(distance, penalty, distance.size());

    return 0;
}