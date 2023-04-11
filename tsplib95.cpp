#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <climits>

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

/* recalculate all path cost */
int total_path_cost(vector<int>route, const vector<vector<int>>&distance, const vector<vector<int>>&penalty, int beg, int end, int n){
    int cost = 0;
    int src, tar;

    reverse(route.begin()+beg, route.begin()+end+1);

    for(int i=0; i<n; i++){
        src = route[i];
        tar = route[(i+1)%n];
        cost += (distance[src][tar] + penalty[src][i]);
    }

    return cost;
}

vector<int> nearest_neighbor_heuristic(const vector<vector<int>>&distance, const vector<vector<int>>&penalty, int n){

	vector<int> route(n);
	bool visited[n] = {false};

	route[0] = 0;
	visited[0] = true;

	/* visit all src nodes */
	for(int i=0; i<n; i++){
		int nearest = INT_MAX;
		int next=0;

		/* visit all tar nodes */
		for(int j=0; j<n; j++){

			if(!visited[j]){
				/* get nearest neighbor */
				int cost = distance[i][j] + penalty[ route[i] ][i];
				if(cost < nearest){
					nearest = cost;
					next = j;
				}
			}
		}
		/* visit next neighbor */
		visited[next] = true;
		route[(i+1)%n]=next;
	}

	return route;
}

int solve_TSPP(const vector<vector<int>>&distance, const vector<vector<int>>&penalty, int n){

    //vector<int> route(n);
    bool isImproving=true;

    /* initial solution */
	vector<int> route(n);
    for(int i=0; i<n; i++){
        route[i] = i;
    }

	//vector<int> route = nearest_neighbor_heuristic(distance, penalty, n);

    /* initial cost */
    int initial_cost = total_path_cost(route,distance,penalty,0,-1,n);
    int curr_cost = initial_cost;
    int reverse_beg, reverse_end;

    //std::cout << initial_cost << "\n";
	return initial_cost;
	//for(int i=0; i<n; i++){
	//	cout << route[i] << " ";
	//} cout << "\n";

    while(isImproving){

        isImproving=false;

        /* search in the neighborhood space */
        for(int i=0; i<n-2; i++){
            for(int j=i+2; j<n; j++){

                /* calculate 2opt new cost */
	        	int solution_cost = total_path_cost(route, distance, penalty, (i+1)%n, j, n);

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
    //cout << "best route: ";
    for(int i=0; i<n; i++){
        cout << route[i]+1 << " ";
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

    int ans = solve_TSPP(distance, penalty, distance.size());
	cout << ans << "\n";
    return 0;
}
