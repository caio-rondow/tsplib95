#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <climits>
#include <chrono>

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

vector<int> nearest_neighbor_v1(const vector<vector<int>>&distance, int n){

	vector<int> route(n);
	bool visited[n] = {false};

	route[0]   = 0;
	visited[0] = true;
    int current_city = 0;

	/* visit all src nodes */
	for(int i=0; i<n; i++){
		int nearest = INT_MAX;
		int next_city=0;

		/* visit all tar nodes */
		for(int j=0; j<n; j++){
            /* get nearest neighbor */
            int cost = distance[current_city][j];
			if(!visited[j] && cost < nearest){
                nearest = cost;
                next_city = j;		
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

    // init
    vector<int> route;
    bool visited[n] = {false};

    /* start with city 0 */
    route.insert(route.begin(),0);
    visited[0] = true;

    /* find x nearest city of 0 */
    int nearest = INT_MAX;
    int next_city = 0;
    for(int j=0; j<n; j++){
        int cost = distance[0][j];
        if(!visited[j] && cost < nearest){
            next_city = j;
            nearest   = cost;
        }
    }
    route.insert(route.end(), next_city);
    visited[next_city] = true;

    /* 
        find city y the nearest of city x or 
        city z nearest of city 0 
    */
    for(int i=2; i<n; i++){
        nearest = INT_MAX;
        bool add_front = false;

        for(int j=0; j<n; j++){
            
            if(!visited[j]){

                int cost_back  = distance[route.back()][j];
                int cost_front = distance[route.front()][j];

                /* add at the back */
                if(cost_back < cost_front && cost_back < nearest){
                    nearest   = cost_back;
                    next_city = j;
                    add_front = false;
                } else if(cost_front < cost_back && cost_front < nearest){ /* add at front */
                    nearest   = cost_front;
                    next_city = j;
                    add_front = true;
                }
            }
        }

        visited[next_city] = true;
        if(add_front){
            route.insert(route.begin(), next_city);
        }
        else
            route.insert(route.end(), next_city);
    }

    /* start from city 0 */
    int start=0;

    for(int i=0; i<n; i++){
        if(route[i]==0){
            start=i;
            break;
        }
    }

    vector<int> front; 
    front = vector<int>( route.begin()+start, route.end() );
    
    vector<int> back; 
    back = vector<int>( route.begin(), route.begin()+start );
    
    vector<int> final_route;
    for(auto &f:front){
        final_route.push_back(f);
    }

    for(auto &b:back){
        final_route.push_back(b);
    }

    return final_route;
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
    //vector<int> route(n);   ADD 1234567...N                // TRIVIAL
	vector<int> route = nearest_neighbor_v1(distance, n);  // GREEDY 1
	// vector<int> route = nearest_neighbor_v2(distance, n);    // GREEDY 2

    /* INITIAL COST */
    int initial_cost = total_path_cost(route,distance,penalty,0,-1,n);
    int curr_cost = initial_cost;
    int reverse_beg, reverse_end;

    // for(int i=0; i<n; i++)
    //     cout << route[i] << " ";
    // cout << "\n";
 
    // greedy2
    // return initial_cost;

    /* LOCAL SEARCH */
    bool isImproving=true;
    while(isImproving){

        isImproving=false;

        /* search in the neighborhood space */
        for(int i=0; i<n-2; i++){
            for(int j=i+2; j<n; j++){

                /* calculate 2opt new cost */
	        	// int solution_cost = total_path_cost(route, distance, penalty, (i+1)%n, j, n);
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

    auto start = std::chrono::system_clock::now();
    
    int ans = solve_TSPP(distance, penalty, distance.size());
    
    auto end = std::chrono::system_clock::now();
    auto elapsed = end - start;

    cout << ans << " ";
    cout << elapsed.count()/1000000000.0 << "\n";
    
    return 0;
}
