#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <sstream>
#include <string>
#include <algorithm>
#include <random>
#include <numeric>
#include <set>

using namespace std;

#define POP_SIZE 200
#define NR_GEN 2000
#define CROSS_RATE 0.8



struct Point {
    double x;
    double y;
};

vector<vector<double>> distance_matrix;

double euclidean_distance(const Point& a, const Point& b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

vector<Point> read_node_coord_section(ifstream& infile) {
    vector<Point> points;
    string line;

    while (getline(infile, line)) {
        if (line == "EOF" || line.empty()) {
            break;
        }
        stringstream ss(line);
        int id;
        double x, y;
        ss >> id >> x >> y;
        points.push_back({ x, y });
    }
    return points;
}

vector<vector<int>> read_edge_weight_section(ifstream& infile, int dimension) {
    vector<int> distance_values;
    string line;

    while (getline(infile, line)) {
        if (line == "EOF" || line.empty()) {
            break;
        }
        stringstream ss(line);
        int value;
        while (ss >> value) {
            distance_values.push_back(value);
        }
    }

    // Build the distance matrix from the linear values
    vector<vector<int>> distance_matrix(dimension, vector<int>(dimension, 0));
    int index = 0;
    for (int i = 0; i < dimension; ++i) {
        for (int j = 0; j <= i; ++j) {
            distance_matrix[i][j] = distance_matrix[j][i] = distance_values[index++];
        }
    }
    return distance_matrix;
}

pair<string, vector<vector<double>>> read_instances(const string& filename) {
    ifstream infile(filename);

    if (!infile.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        exit(1);
    }

    string line;
    int dimension = 0;
    string section_type;

    // Read metadata
    while (getline(infile, line)) {
        if (line.find("DIMENSION") != string::npos) {
            dimension = stoi(line.substr(line.find(":") + 1));
        }
        else if (line.find("NODE_COORD_SECTION") != string::npos) {
            section_type = "NODE_COORD_SECTION";
            break;
        }
        else if (line.find("EDGE_WEIGHT_SECTION") != string::npos) {
            section_type = "EDGE_WEIGHT_SECTION";
            break;
        }
    }

    if (section_type == "NODE_COORD_SECTION") {
        // Process NODE_COORD_SECTION
        vector<Point> points = read_node_coord_section(infile);

        // Create distance matrix from points
        vector<vector<double>> distance_matrix(points.size(), vector<double>(points.size(), 0.0));
        for (size_t i = 0; i < points.size(); ++i) {
            for (size_t j = 0; j < points.size(); ++j) {
                if (i != j) {
                    distance_matrix[i][j] = euclidean_distance(points[i], points[j]);
                }
            }
        }
        return { "NODE_COORD_SECTION", distance_matrix };
    }
    else if (section_type == "EDGE_WEIGHT_SECTION") {
        // Process EDGE_WEIGHT_SECTION
        vector<vector<int>> int_distance_matrix = read_edge_weight_section(infile, dimension);

        // Convert to double matrix for consistency
        vector<vector<double>> distance_matrix(dimension, vector<double>(dimension, 0.0));
        for (int i = 0; i < dimension; ++i) {
            for (int j = 0; j < dimension; ++j) {
                distance_matrix[i][j] = static_cast<double>(int_distance_matrix[i][j]);
            }
        }
        return { "EDGE_WEIGHT_SECTION", distance_matrix };
    }
    else {
        cerr << "Error: Unknown or unsupported section type." << endl;
        exit(1);
    }
}


/*GA FUNCTIONS*/



vector<int> generate_random_permutation(int n) {
    vector<int> perm(n);
    for (int i = 0; i < n; ++i) {
        perm[i] = i;
    }
    random_device rd;
    mt19937 g(rd()); // Generator de numere aleatoare
    shuffle(perm.begin(), perm.end(), g);
    return perm;
}

vector<vector<int>> generate_initial_population(int num_cities, int population_size) {
    set<vector<int>> unique_population;
    while (unique_population.size() < population_size) {
        vector<int> perm = generate_random_permutation(num_cities);
        unique_population.insert(perm); // Asigură unicitatea permutărilor
    }
    return vector<vector<int>>(unique_population.begin(), unique_population.end());
}

double total_dist_individual(const vector<int>& route, const vector<vector<double>>& cost_matrix) {
    double total_cost = 0.0;

    for (size_t i = 0; i < route.size() - 1; ++i) {
        total_cost += cost_matrix[route[i]][route[i + 1]];
    }

    // Adaugă costul de întoarcere la orașul inițial
    total_cost += cost_matrix[route.back()][route.front()];
    return total_cost;
}



vector<double> fitness_prob(const vector<vector<int>>& population, const vector<vector<double>>& cost_matrix) {
    vector<double> total_dist_all_individuals(population.size());

    // Calculează costurile fiecărui individ
    for (size_t i = 0; i < population.size(); ++i) {
        total_dist_all_individuals[i] = total_dist_individual(population[i], cost_matrix);
    }

    // Determină cel mai mare cost din populație
    double max_population_cost = *max_element(total_dist_all_individuals.begin(), total_dist_all_individuals.end());

    // Transformă costurile în valori de fitness
    vector<double> population_fitness(population.size());
    for (size_t i = 0; i < population.size(); ++i) {
        population_fitness[i] = max_population_cost - total_dist_all_individuals[i];
    }

    // Suma tuturor valorilor de fitness
    double population_fitness_sum = accumulate(population_fitness.begin(), population_fitness.end(), 0.0);

    // Calculează probabilitățile de fitness
    vector<double> population_fitness_probs(population.size());
    for (size_t i = 0; i < population.size(); ++i) {
        population_fitness_probs[i] = population_fitness[i] / population_fitness_sum;
    }

    return population_fitness_probs;
}



vector<int> rank_selection(const vector<vector<int>>& population, const vector<double>& fitness_probs) {
    size_t population_size = population.size();

    // Sortează indivizii după fitness, păstrând indicii
    vector<size_t> sorted_indices(population_size);
    iota(sorted_indices.begin(), sorted_indices.end(), 0);
    sort(sorted_indices.begin(), sorted_indices.end(), [&](size_t a, size_t b) {
        return fitness_probs[a] > fitness_probs[b];
        });

    // Creează un vector de ranguri: cel mai bun individ are rangul 1, al doilea rangul 2, etc.
    vector<double> ranks(population_size);
    for (size_t i = 0; i < population_size; ++i) {
        ranks[sorted_indices[i]] = population_size - i;
    }

    // Normalizează rangurile pentru a obține probabilități
    double rank_sum = accumulate(ranks.begin(), ranks.end(), 0.0);
    for (double& rank : ranks) {
        rank /= rank_sum;
    }

    // Calculează suma cumulativă a probabilităților bazate pe ranguri
    vector<double> cumulative_probs(population_size, 0.0);
    partial_sum(ranks.begin(), ranks.end(), cumulative_probs.begin());

    // Generează un număr aleator între 0 și 1 pentru selecție
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);
    double rand_value = dis(gen);

    // Găsește individul selectat pe baza probabilităților cumulative
    for (size_t i = 0; i < cumulative_probs.size(); ++i) {
        if (rand_value <= cumulative_probs[i]) {
            return population[i];
        }
    }

    // În caz improbabil, returnează ultimul individ
    return population.back();
}


vector<int> tournament_selection(const vector<vector<int>>& population, const vector<double>& fitness_probs, int tournament_size) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, population.size() - 1);

    vector<int> best_individual;
    double best_fitness = -1.0;

    for (int i = 0; i < tournament_size; ++i) {
        int idx = dis(gen);
        if (fitness_probs[idx] > best_fitness) {
            best_fitness = fitness_probs[idx];
            best_individual = population[idx];
        }
    }

    return best_individual;
}




pair<vector<int>, vector<int>> crossover(const vector<int>& parent_1, const vector<int>& parent_2) {
    int n_cities = parent_1.size();

    // Generează un punct de tăiere aleator între 1 și n_cities - 1
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, n_cities - 1);
    int cut = dis(gen);

    // Inițializează urmașii
    vector<int> offspring_1(parent_1.begin(), parent_1.begin() + cut);
    vector<int> offspring_2(parent_2.begin(), parent_2.begin() + cut);

    // Completează urmașul 1 cu orașele rămase din parent_2
    for (int city : parent_2) {
        if (find(offspring_1.begin(), offspring_1.end(), city) == offspring_1.end()) {
            offspring_1.push_back(city);
        }
    }

    // Completează urmașul 2 cu orașele rămase din parent_1
    for (int city : parent_1) {
        if (find(offspring_2.begin(), offspring_2.end(), city) == offspring_2.end()) {
            offspring_2.push_back(city);
        }
    }

    return { offspring_1, offspring_2 };
}

vector<int> mutation(vector<int> offspring) {
    int n_cities = offspring.size();

    // Generează două indecși aleatori între 0 și n_cities - 1
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, n_cities - 1);
    int index_1 = dis(gen);
    int index_2 = dis(gen);

    // Asigură-te că cei doi indecși nu sunt egali
    while (index_1 == index_2) {
        index_2 = dis(gen);
    }

    // Schimbă elementele de la indecșii selectați
    swap(offspring[index_1], offspring[index_2]);

    return offspring;
}


double calculate_individual_cost(const vector<int>& individual, const vector<vector<double>>& cost_matrix) {
    double total_cost = 0.0;
    int num_cities = individual.size();

    // Parcurge orașele din traseu
    for (int i = 0; i < num_cities - 1; ++i) {
        total_cost += cost_matrix[individual[i]][individual[i + 1]];
    }

    // Adaugă costul pentru întoarcerea la orașul inițial
    total_cost += cost_matrix[individual[num_cities - 1]][individual[0]];

    return total_cost;
}


void two_opt(vector<int>& route, const vector<vector<double>>& cost_matrix) {
    bool improved = true;
    while (improved) {
        improved = false;
        for (size_t i = 1; i < route.size() - 1; ++i) {
            for (size_t j = i + 1; j < route.size(); ++j) {
                double old_cost = cost_matrix[route[i - 1]][route[i]] + cost_matrix[route[j]][route[(j + 1) % route.size()]];
                double new_cost = cost_matrix[route[i - 1]][route[j]] + cost_matrix[route[i]][route[(j + 1) % route.size()]];
                if (new_cost < old_cost) {
                    reverse(route.begin() + i, route.begin() + j + 1);
                    improved = true;
                }
            }
        }
    }
}


double dynamic_mutation_rate(int generation, int max_generations) {
    return 0.5 * (1.0 - static_cast<double>(generation) / max_generations); // Starts at 0.5 and decreases to 0
}



vector<vector<int>> run_ga(int n_cities, int n_population, int n_generations,
    double crossover_per, double mutation_per, int elite_size) {
    // Initialize the population
    vector<vector<int>> population = generate_initial_population(n_cities, n_population);

    // Calculate fitness probabilities
    vector<double> fitness_probs = fitness_prob(population, distance_matrix);

    for (int generation = 0; generation < n_generations; ++generation) {
        // Sort population by fitness
        vector<size_t> sorted_indices(population.size());
        iota(sorted_indices.begin(), sorted_indices.end(), 0);
        sort(sorted_indices.begin(), sorted_indices.end(), [&](size_t a, size_t b) {
            return fitness_probs[a] > fitness_probs[b];
            });

        // Keep the elite individuals
        vector<vector<int>> new_population;
        for (int i = 0; i < elite_size; ++i) {
            new_population.push_back(population[sorted_indices[i]]);
        }

        // Create parents for crossover
        vector<vector<int>> parents_list;
        for (int i = 0; i < int(crossover_per * n_population); ++i) {
            //parents_list.push_back(tournament_selection(population, fitness_probs, 5)); // Replace rank_selection
            parents_list.push_back(rank_selection(population, fitness_probs));

        }

        // Create offspring from parents
        vector<vector<int>> offspring_list;
        for (size_t i = 0; i < parents_list.size(); i += 2) {
            if (i + 1 < parents_list.size()) {
                auto offspring_pair = crossover(parents_list[i], parents_list[i + 1]);
                vector<int> offspring_1 = offspring_pair.first;
                vector<int> offspring_2 = offspring_pair.second;

                // Apply mutation
                random_device rd;
                mt19937 gen(rd());
                uniform_real_distribution<> dis(0.0, 1.0);
                double mutation_rate = dynamic_mutation_rate(generation, n_generations); // Dynamic mutation rate

                if (dis(gen) < mutation_rate) {
                    offspring_1 = mutation(offspring_1);
                }
                if (dis(gen) < mutation_rate) {
                    offspring_2 = mutation(offspring_2);
                }

                offspring_list.push_back(offspring_1);
                offspring_list.push_back(offspring_2);
            }
        }

        // Fill the population with offspring
        while (new_population.size() < n_population) {
            if (!offspring_list.empty()) {
                new_population.push_back(offspring_list.back());
                offspring_list.pop_back();
            }
            else {
                new_population.push_back(generate_random_permutation(n_cities));
            }
        }

        // Apply 2-opt refinement every 10 generations
        if (generation % 10 == 0) {
            for (auto& individual : new_population) {
                two_opt(individual, distance_matrix);
            }
        }

        // Update population and fitness probabilities
        population = new_population;
        fitness_probs = fitness_prob(population, distance_matrix);

        // Find the best individual in the current generation
        double best_cost = numeric_limits<double>::max();
        vector<int> best_individual;

        for (const auto& individual : population) {
            double total_cost = calculate_individual_cost(individual, distance_matrix);
            if (total_cost < best_cost) {
                best_cost = total_cost;
                best_individual = individual;
            }
        }

        // Print progress
        cout << "Generation " << generation + 1 << " - Best Cost: " << best_cost << "\n";
        /*cout << "Best Route: ";
        for (int city : best_individual) {
            cout << city << " ";
        }*/
        cout << endl;
    }

    return population;
}



float ga_3(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <path_to_tsp_file>" << endl;
        return 1;
    }

    string filename = argv[1]; // Get the file path from the command-line argument
    auto result = read_instances(filename);
    string section_type = result.first;
    distance_matrix = result.second;

    cout << "Section Type: " << section_type << "\n";
    cout << "Distance Matrix:\n";
    for (const auto& row : distance_matrix) {
        for (double dist : row) {
            cout << dist << " ";
        }
        cout << "\n";
    }

    // Genetic Algorithm execution
    int num_cities = distance_matrix.size();
    double MUTATION_RATE = 0.2;
    int elite_size = 7;
    vector<vector<int>> final_population = run_ga(num_cities, POP_SIZE, NR_GEN, CROSS_RATE, MUTATION_RATE, elite_size);

    double best_cost = numeric_limits<double>::max();
    for (const auto& individual : final_population) {
        double total_cost = calculate_individual_cost(individual, distance_matrix);
        if (total_cost < best_cost) {
            best_cost = total_cost;
        }
    }

    // Output the best cost for external consumption
    cout << "Final best cost: " << best_cost << endl;

    return best_cost;
}




int main() {
    string filename = "C:\\Users\\Valea\\Desktop\\MMO\\Tema2\\st70.tsp"; // Update with the correct path to your file
    auto result = read_instances(filename);
    string section_type = result.first;
    distance_matrix = result.second;

    cout << "Section Type: " << section_type << "\n";
    cout << "Distance Matrix:\n";
    for (const auto& row : distance_matrix) {
        for (double dist : row) {
            cout << dist << " ";
        }
        cout << "\n";
    }

    // Generare populație inițială
    int num_cities = distance_matrix.size();
    double MUTATION_RATE = 0.2;
    double total_cost = 0;
    int elite_size = 7; // Numărul de indivizi de elită
    vector<vector<int>> final_population = run_ga(num_cities, POP_SIZE, NR_GEN, CROSS_RATE, MUTATION_RATE, elite_size);


    // Afișare populație finală
    cout << "Final population (best individuals):\n";
    double best_cost = numeric_limits<double>::max();
    vector<int> best_individual;

    for (const auto& individual : final_population) {
        total_cost = calculate_individual_cost(individual, distance_matrix);
        if (total_cost < best_cost) {
            best_cost = total_cost;
            best_individual = individual;
        }
    }

    // Afișare cel mai bun individ
    cout << "\nBest individual:\n";
    cout << "Total cost: " << best_cost << "\n";
    cout << "Route: ";
    for (int city : best_individual) {
        cout << city << " ";
    }
    cout << endl;

    return 0;
}




