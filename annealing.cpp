#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <climits>
#include <numeric>
#include <chrono>
#include <cmath>

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

int evaluate(vector<int> route, const vector<vector<int>> &distance, const vector<vector<int>> &penalty, int n)
{
    int cost = 0;
    int src, tar;

    for (int i = 0; i < n; i++)
    {
        src = route[i];
        tar = route[(i + 1) % n];
        cost += (distance[src][tar] + penalty[src][i]);
    }

    return cost;
}

float temperature(int temp)
{
    return temp * 0.99999;
}

float acc_probability(int deltaC, double temp)
{
    // return ( deltaC<0 ? 1 : exp((-1*deltaC)/temp) );
    return (1 / ( 1 + exp(deltaC / 1000.0)) );
}

int annealing(const vector<vector<int>> &distance, const vector<vector<int>> &penalty, int n)
{

    /* RANDOM INITIAL SOLUTION */
    vector<int> route(n);
    iota(route.begin(), route.end(), 0);
    // random_shuffle(route.begin()+1, route.end());

    /* INITIAL COST */
    int curr_cost = evaluate(route, distance, penalty, n);
    double temp = 1000.0;
    int nswaps = 0;
    int besti = -1, bestj = -1;

    cout << "INITIAL COST: " << curr_cost << "\n";

    while (temp >= 0.0001)
    {
        for (int i = 1; i < n; i++)
        {
            for (int j = i + 1; j < n; j++)
            {

                /* EVALUATE NEW COST */
                swap(route[i], route[j]);

                int new_cost = evaluate(route, distance, penalty, n);
                float prob = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

                /* EFFECT SWAP */
                if (new_cost < curr_cost || prob < acc_probability(curr_cost - new_cost, temp))
                {
                    curr_cost = new_cost;
                    besti = i;
                    bestj = j;
                    nswaps++;
                }

                /* UNDO SWAP */
                swap(route[i], route[j]);
            }
        }

        swap(route[besti], route[bestj]);

        // for (auto &r : route)
        //     cout << r << " ";
        // cout << "\n";

        /* UPDATE TEMPERATURE */
        temp = temperature(temp);
    }

    cout << "NUM. SWAPS: " << nswaps << "\n";

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

    int ans=-1;
    for(int i=0; i<25; i++){
        ans = max( ans, annealing(distance, penalty, distance.size()) );
    }
    cout << "FINAL COST: " << ans << "\n";

    return 0;
}
