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
using namespace std;

// Definindo o tamanho máximo da lista tabu
#define TABU_SIZE 10

// Definindo o número máximo de iterações
#define MAX_ITERATIONS 100

// Definindo o valor máximo para a função de avaliação
#define MAX_EVALUATION std::numeric_limits<int>::max()

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

// Implemente a função de avaliação do seu problema
int evaluateSolution(int initial_cost, int beg, int end, const vector<int>&route, const vector<vector<int>>&distance, const vector<vector<int>>&penalty, int n) {
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

// Implemente o critério de parada do algoritmo
bool stopCriterion(int iterations) {
    return (iterations >= MAX_ITERATIONS);
}

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

int local_search(vector<int>&route,const vector<vector<int>>&distance, const vector<vector<int>>&penalty, int curr_cost, int best_i, int best_j, int n){

    int initial_cost = curr_cost;
    int reverse_beg  = best_i;
    int reverse_end  = best_j;
    bool isImproving=true;
    while(isImproving){

        isImproving=false;

        /* search in the neighborhood space */
        for(int i=0; i<n-2; i++){
            for(int j=i+2; j<n; j++){

                /* calculate 2opt new cost */
				int solution_cost = evaluateSolution(initial_cost,i,j,route,distance,penalty,n);

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

float temperature(int temp)
{
    return temp * 0.99999;
}

float acc_probability(int deltaC, double temp)
{
    return ( deltaC<0 ? 1 : exp((-1.0*deltaC)/temp) );
}

vector<int> annealing(const vector<vector<int>> &distance, const vector<vector<int>> &penalty, int n)
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
				int solution_cost = evaluateSolution(curr_cost,i,j,route,distance,penalty,n);

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

    return route;
}

// Implemente a busca tabu
int tabuSearch(
        vector<int>& initialSolution, 
        const vector<vector<int>> &distance, 
        const vector<vector<int>> &penalty, 
        int n
    ) {

    int iterations = 0;

    vector<int> bestSolution    = initialSolution;
    vector<int> currentSolution = initialSolution;
    int bestCost = total_path_cost(initialSolution, distance, penalty, n);
    int currentCost = bestCost;
    int best_i=-1, best_j=-1;

    vector<vector<int>> tabuList;
    int tabusize = 0;

    while (!stopCriterion(iterations)) {

        vector<int> bestNeighbor;
        int bestEvaluation = MAX_EVALUATION;

        // Gere os vizinhos da solução atual e avalie-os
        for(int i=0; i<n-2; i++){
            for(int j=i+2; j<n; j++){

                int evaluation = evaluateSolution(currentCost,i,j,currentSolution,distance,penalty,n);

                vector<int> neighbor = currentSolution;
                reverse(neighbor.begin()+(i+1)%n, neighbor.begin()+j+1);

                // Verifique se o vizinho não está na lista tabu
                bool isTabu = false;
                for (const auto& tabuSolution : tabuList) {
                    if (neighbor == tabuSolution) {
                        isTabu = true;
                        break;
                    }
                }

                // Atualize a melhor solução vizinha
                if (evaluation < bestEvaluation && !isTabu) {
                    bestNeighbor = neighbor;
                    bestEvaluation = evaluation;
                    best_i = i;
                    best_j = j;
                }
            }
        }

        // Atualize a melhor solução global se necessário
        if (bestEvaluation < bestCost) {
            bestSolution = bestNeighbor;
            bestCost     = bestEvaluation;
        }

        // Adicione a melhor solução vizinha à lista tabu
        tabuList.push_back(bestNeighbor);
        tabusize++;

        // Verifique se a lista tabu excedeu o tamanho máximo e remova o mais antigo
        if (tabusize > TABU_SIZE) {
            tabuList.erase(tabuList.begin());
            tabusize--;
        }

        // Atualize a solução atual para a melhor vizinha
        currentSolution = bestNeighbor;
        currentCost     = bestEvaluation;

        iterations++;
    }

    bestCost = local_search(bestSolution, distance, penalty, bestCost, best_i, best_j, n);

    return bestCost;
}

int main(int argc, char **argv){

    srand(time(0));

    /* read distance and penalty matrix */
    string problem_name = argv[1];
    string penalty_name = argv[2];

    /* init matrices */
    vector<vector<int>> distance = load_problem(problem_name);
    vector<vector<int>> penalty  = load_problem(penalty_name);

    int times = 1;
    int n = distance.size();
    int ans = INT32_MAX;

    auto start = std::chrono::system_clock::now();
    while(times--){
        vector<int> initialSolution = annealing(distance, penalty, n);
        int bestCost = tabuSearch(initialSolution, distance, penalty, n);
        ans = min(ans, bestCost); 
    }
    auto end = std::chrono::system_clock::now();
    auto elapsed = end - start;

  	cout << ans << " ";
    cout << elapsed.count()/1000000000.0 << "\n";

    return 0;
}