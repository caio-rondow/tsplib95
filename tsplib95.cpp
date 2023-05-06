#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <climits>
#include <numeric>
#include <chrono>

using namespace std;

/*OK*/
void print_matrix(const vector<vector<int>>&matrix, int n){
    std::cout << n << "\n";
    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            std::cout << matrix[i][j] << " ";
        }
        std::cout << "\n";
    }
}

/*OK*/
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

/*OK*/
int total_path_cost(vector<int>route, const vector<vector<int>>&distance, const vector<vector<int>>&penalty, int n){
    int cost = 0;
    int src, tar;

    for(int i=0; i<n; i++){
        src = route[i];
        tar = route[(i+1)%n];
        cost += (distance[src][tar] + penalty[src][i]);
    }

    return cost;
}

/*OK*/
vector<int> nearest_neighbor_v1(const vector<vector<int>>&distance, int n){

	vector<int> route(n);
	bool visited[n] = {false};
    int current_city = 0;
	
    route[0] = current_city;
	visited[current_city] = true;

	/* visit all src nodes */
	for(int i=0; i<n; i++){
		int nearest = INT_MAX;
		int next_city = 0;

		/* visit all tar nodes */
		for(int j=0; j<n; j++){

			if(!visited[j]){
				/* get nearest neighbor */
				int cost = distance[current_city][j];
				if(cost < nearest){
					nearest = cost;
					next_city = j;
				}
			}
		}
		/* visit next neighbor */
		visited[next_city] = true;
		route[(i+1)%n]     = next_city;
        current_city       = next_city;
	}

	return route;
}

vector<int> nearest_neighbor_v2(const vector<vector<int>>&distance, int n){
    
    vector<int> route;
    vector<bool> visited(n,false);
    int start=0;

    /* start at city 0 */
    route.push_back(0);
    visited[0] = true;

    /* find city x, nearest of city 0 */
    int next_city = -1;
    int nearest   = INT_MAX;
    for(int i=0; i<n; i++){
        if(!visited[i] && distance[0][i] < nearest){
            nearest = distance[0][i];
            next_city = i;
        }
    }
    route.push_back(next_city);
    visited[next_city] = true;

    /* 
        find city y, nearest of city at begin or
        find city z, nearest of city at end 
    */
    for(int i=2; i<n; i++){
        
        next_city = -1;
        nearest   = INT_MAX;
        bool in_front = false;

        for(int j=0; j<n; j++){
            
            int cost_back  = distance[route.back()][j];
            int cost_front = distance[route.front()][j];

            if(!visited[j] && (cost_back < nearest || cost_front < nearest)){

                if(cost_back < cost_front){
                    in_front = false;
                    nearest  = cost_back;
                } else{
                    in_front = true;
                    nearest  = cost_front;
                }
                next_city = j;
            }
        }

        visited[next_city] = true;
        if(in_front){
            start++;
            route.insert(route.begin(), next_city);
        } else{
            route.insert(route.end(), next_city);
        }
    }

    rotate(route.begin(), route.begin()+start, route.end());

    return route;
}

int evaluate(int initial_cost, int beg, int end, const vector<int>&route, const vector<vector<int>>&distance, const vector<vector<int>>&penalty, int n){

	/* calculate new distance cost */
	int dist_cost = -distance[route[beg]][route[(beg+1)%n]]
					-distance[route[end]][route[(end+1)%n]]
					+distance[route[beg]][route[end]]
					+distance[route[(beg+1)%n]][route[(end+1)%n]];

	/* calculate new penalty cost */
	int k = (beg+1)%n;
	int pen_cost=0;
	for(int i=end; i>beg; i--){
		pen_cost += (penalty[route[i]][k] - penalty[route[i]][i]);
		k++;
	} 

	return initial_cost + dist_cost + pen_cost;
}

int solve_TSPP(const vector<vector<int>>&distance, const vector<vector<int>>&penalty, int n){

    /* INITIAL SOLUTION */
    vector<int> route(n); iota(route.begin(), route.end(), 0);   // 0,1,2,3, ..., N
	// vector<int> route = nearest_neighbor_v1(distance, n);     // GREEDY 1
	// vector<int> route = nearest_neighbor_v2(distance, n);     // GREEDY 2

    /* INITIAL COST */
    int initial_cost = total_path_cost(route,distance,penalty,n);
    int curr_cost = initial_cost;
    int reverse_beg, reverse_end;

	// return initial_cost;
    std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
    start = std::chrono::high_resolution_clock::now();
    /* LOCAL SEARCH */
    bool isImproving=true;
    while(isImproving){

        isImproving=false;

        /* search in the neighborhood space */
        for(int i=0; i<n-2; i++){
            for(int j=i+2; j<n; j++){

                /* calculate 2opt new cost */
				int solution_cost = evaluate(initial_cost,i,j,route,distance,penalty,n);

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
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    // std::cout << "Execution time: " << duration.count() << " seconds." << std::endl;

	return curr_cost;
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
