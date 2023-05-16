#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <climits>
#include <numeric>
#include <chrono>
#include <cmath>
#include <random>
#include <map>

#define EULER 2.7182818

using namespace std;

/*OK*/
void print_matrix(const vector<vector<int>> &matrix, int n)
{
    std::cout << n << "\n";
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << "\n";
    }
}

/*OK*/
vector<vector<int>> load_problem(const std::string &filename)
{

    /* open file */
    std::ifstream fin(filename);
    if (!fin.is_open())
    {
        std::cerr << "Could not open " << filename << "\n";
        exit(1);
    }

    /* get first line */
    std::string row;
    std::getline(fin, row);
    int n = stoi(row);

    /* create matrix */
    vector<vector<int>> matrix(n, vector<int>(n));
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            fin >> matrix[i][j];
        }
    }

    fin.close();

    return matrix;
}

/* recalculate all path cost */
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

float temperature(int temp)
{
    return temp * 0.99999;
}

float acc_probability(int deltaC, double temp)
{
    return ( deltaC<0 ? 1 : exp((-1.0*deltaC)/temp) );
    // return (1.0 / ( 1.0 + exp(deltaC / 1000.0)) );
}

int annealing(const vector<vector<int>> &distance, const vector<vector<int>> &penalty, int n)
{

    /* RANDOM INITIAL SOLUTION */
    vector<int> route(n);
    iota(route.begin(), route.end(), 0);

    /* INITIAL COST */
    int curr_cost = total_path_cost(route, distance, penalty, n);
    double temp = n >= 666 ? 100.0:1000.0;
    int nswaps = 0;

    /* RANDOM VALUE*/
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<float> dis(0.0, 1.0);

    int reverse_beg = 0;
    int reverse_end = 0;

    while (temp >= 0.0001){

        /* search in the neighborhood space */
        for(int i=0; i<n-2; i++){
            for(int j=i+2; j<n; j++){

                /* calculate 2opt new cost */
				int solution_cost = evaluate(curr_cost,i,j,route,distance,penalty,n);

                float randomNum = dis(gen);

                /* found better solution */
                if(solution_cost < curr_cost || randomNum < acc_probability((solution_cost - curr_cost),temp)){
                    reverse_beg = (i+1)%n;
                    reverse_end = j;
                    curr_cost = solution_cost;
                    reverse(route.begin()+reverse_beg, route.begin()+reverse_end+1);
                    nswaps++;
                }
            }
        }

        temp = temperature(temp);
    }

    return curr_cost;
}

int tannealing(const vector<vector<int>> &distance, const vector<vector<int>> &penalty, int n)
{

    /* RANDOM INITIAL SOLUTION */
    vector<int> route(n);
    iota(route.begin(), route.end(), 0);

    /* INITIAL COST */
    int curr_cost = total_path_cost(route, distance, penalty, n);
    double temp = n >= 666 ? 100.0:1000.0;
    int nswaps = 0;

    /* RANDOM VALUE*/
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<float> dis(0.0, 1.0);


    int reverse_beg = 0;
    int reverse_end = 0;

    /* SOLUTION MEMORY*/
    map<int,int> soulution_map;

    while (temp >= 0.0001){

        int acc_count = 0;

        /* search in the neighborhood space */
        for(int i=0; i<n-2; i++){
            for(int j=i+2; j<n; j++){

                /* calculate 2opt new cost */
				int solution_cost = evaluate(curr_cost,i,j,route,distance,penalty,n);

                float randomNum = dis(gen);

                /* found better solution */
                if(solution_cost < curr_cost || (randomNum < acc_probability((solution_cost - curr_cost),temp) && acc_count < 1000)){
                    if(solution_cost > curr_cost) acc_count++; 
                    reverse_beg = (i+1)%n;
                    reverse_end = j;
                    curr_cost = solution_cost;
                    reverse(route.begin()+reverse_beg, route.begin()+reverse_end+1);
                    nswaps++;
                }
            }
        }

        temp = temperature(temp);
    }


    // cout << "custo antes: " << curr_cost << "\n";
    int initial_cost = curr_cost;
    
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

    // cout << "custo depois: " << curr_cost << "\n";

    return curr_cost;
}

int main(int argc, char **argv)
{

    srand(time(0));

    /* read distance and penalty matrix */
    std::string problem_name = argv[1];
    std::string penalty_name = argv[2];

    /* init matrices */
    vector<vector<int>> distance = load_problem(problem_name);
    vector<vector<int>> penalty = load_problem(penalty_name);

    int times = 100;
    int ans = INT32_MAX;
    int n = distance.size();
    while(times--){
        ans = min(ans, tannealing(distance,penalty,n)); 
    }

    times = 100;
    int ans2 = INT32_MAX;
    while(times--){
        ans2 = min(ans2, annealing(distance,penalty,n)); 
    }

    cout << ans << " " << ans2 << "\n";

    return 0;
}
