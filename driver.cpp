/////////////////////////////////////////////////////////////////////
/// @file driver.cpp
/// @author Joshua McCarville-Schueths CS348 Fall 2011
/// @brief Implementation for random search, Assignment 1a.
/////////////////////////////////////////////////////////////////////

#include "driver.h"
using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;
using std::ofstream;

int main(int argc, char * argv[])
{
  // Declaring and Setting the Configuration Variables.
  string config_name = "default.cfg";
  config params;
  if(argc == 2)
    config_name = argv[1];
  ConfigFile config_file(config_name);
  params.init(config_file);
  srand(params.seed());
  
  // File I/O variables
  ifstream network;
  ofstream log, solution;
  
  // Declaring the data.
  int num_hosts;              // Number of total hosts
  int num_routers;            // Number of total routers
  
  bool ** routers;            // Array to store which routers are enabled/disabled.
  bool ** connections;        // Multidimensional array to store the network paths.
       
  bool * global_best_solution;// Best solution found so far.
  int cur_fit, best_fit = 0;  // To compare fitness evaluations.
  int prev_fit;
  int last_eval_change = 0;
  int global_best = 0;
  int avg_fit = 0;
  
  
  // Read in the data files and populate the data fields.
  network.open(params.input_file().c_str());
  if(!network.is_open())
  {
    cerr << "Data file does not exist!" << endl << "Check configuration file." << endl;
    return 0;
  }
  read_network(network, num_hosts, num_routers);
  
  // Initialize memory for the solution population
  routers = new bool*[params.mu()];
  for(int i = 0; i < params.mu(); i++)
  {
    routers[i] = new bool[num_routers];
    for(int j = 0; j < num_routers; j++)
      routers[i][j] = 0;
  }
  global_best_solution = new bool[num_routers];
  
  // Initialize memory for the children population
  bool ** children;
  children = new bool*[params.lambda()];
  for(int i = 0; i < params.lambda(); i++)
    children[i] = new bool[num_routers];
  
  // Initialize memory for the connection table
  connections = new bool*[num_hosts];
  for(int i = 0; i < num_hosts; i++)
  {
    connections[i] = new bool[num_routers];
    for(int j = 0; j < num_routers; j++)
      connections[i][j] = 0;
  }
  
  // Initialize memory to keep track of parent and children fitness.
  int * parent_fitness = new int[params.mu()];
  int * children_fitness = new int[params.lambda()];
  int * parents_selected = new int[params.num_parents()];

  // Populate the connections table here.
  read_connections(network, num_hosts, connections);
  
  // Open the log and solution file
  log.open(params.log_file().c_str());
  solution.open(params.solution_file().c_str());
  
  
  // Output the parameters to the log.
  log << "Result Log" << endl << endl;
  log << "Data Set Name:\t" << params.input_file() << endl;
  log << "RNG Seed:\t" << params.seed() << endl;
  log << "Penalty Coefficient:\t" << params.penalty() << endl;
  log << "Runs:\t" << params.runs() << endl;
  log << "Termination Condition:\t" << params.term() << endl;
  log << "Evals:\t" << params.evals() << endl;
  log << "Mu:\t" << params.mu() << endl;
  log << "Lambda:\t" << params.lambda() << endl;
  log << "Num parents:\t" << params.num_parents() << endl;
  log << "Selection type:\t" << params.select_type() << endl;
  if(params.select_type() == "tournament")
    log << "Tournament Size:\t" << params.tourney_size() << endl;
  log << "Recombination:\t" << params.recombination() << endl;
  if(params.recombination() == "n-point")
    log << "N-Point Crossover Breaks:\t" << params.num_crossovers() << endl << endl;
  log << "Num routers: " << num_routers << endl;
  log << "Num hosts: " << num_hosts << endl << endl;
  
  // Begin the EA
  for(int i = 0; i < params.runs(); i++)
  {
    int num_evals = 0;
    
    log << "Run " << i + 1 << endl;
    
    // Randomly initialize the starting population
    for(int j = 0; j < params.mu(); j++)
    {
      for(int k = 0; k < num_routers; k++)
      {
        if(rand() % 2) // Basically a coin flip to disable a router.
          routers[j][k] = 1;
      }
    }
    
    // Evaluate the fitness of the mu solutions.
    for(int j = 0; j < params.mu(); j++)
    {
      cur_fit = fitness(routers[j], num_routers);
      cur_fit = cur_fit - penalty(routers[j], num_routers, connections, num_hosts, params.penalty());
      parent_fitness[j] = cur_fit;
      num_evals++;
      avg_fit += cur_fit;
    
      // If better than previous eval, save the solution and output the eval.
      if(cur_fit > global_best)
      {
        global_best = cur_fit;
        for(int k = 0; k < num_routers; k++)
          global_best_solution[k] = routers[j][k];
      }
      if(cur_fit > best_fit)
        best_fit = cur_fit;
    }
    avg_fit = avg_fit / params.mu();
    log << num_evals << "\t" << avg_fit << "\t" << best_fit << endl;
    prev_fit = avg_fit;
    do
    {
      // Select the parents
      // The function populates the parents selected array.
      parent_selection(params, routers, parent_fitness, parents_selected);
      
      // Recombine the parents
      recombine(params, parents_selected, routers, children, num_routers);
      
      // Mutate the children
      for(int j = 0; j < params.lambda(); j++)
        mutate(params.probability(), children[j], num_routers);
      
      // Evaluate the fitness of the lambda solutions.
      for(int j = 0; j < params.lambda(); j++)
      {
        cur_fit = fitness(children[j], num_routers);
        cur_fit = cur_fit - penalty(children[j], num_routers, connections, num_hosts, params.penalty());
        children_fitness[j] = cur_fit;
        if(prev_fit == cur_fit)
        {
          last_eval_change++;
        }
        else
        {
          prev_fit = cur_fit;
          last_eval_change = 0;
        }
        num_evals++;
      
        // If better than previous eval, save the solution and output the eval.
        if(cur_fit > global_best)
        {
          global_best = cur_fit;
          for(int k = 0; k < num_routers; k++)
            global_best_solution[k] = children[j][k];
        }
      }

      // Survival selection
      bool swapped = true;
      while(swapped)
      {
        swapped = false;
        int best = children_fitness[0];
        int best_index = 0;
        int worst = parent_fitness[0];
        int worst_index = 0;
        int temp_fit;
        bool * temp_solution;
        
        // Take the best child, and if it is better than the worst in the mu pop, swap it
        for(int j = 0; j < params.lambda(); j++)
        {
          if(best < children_fitness[j])
          {
            best = children_fitness[j];
            best_index = j;
          }
        }
        for(int j = 0; j < params.mu(); j++)
        {
          if(worst > parent_fitness[j])
          {
            worst = parent_fitness[j];
            worst_index = j;
          }
        }
        if(best > worst)
        {
          swapped = true;
          temp_fit = worst;
          parent_fitness[worst_index] = best;
          children_fitness[best_index] = worst;
          temp_solution = routers[worst_index];
          routers[worst_index] = children[best_index];
          children[best_index] = temp_solution;
        }
      }
      
      // Find the average population fitness and the best local fitness
      best_fit = parent_fitness[0];
      avg_fit = 0;
      for(int j = 0; j < params.mu(); j++)
      {
        if(parent_fitness[j] > best_fit)
          best_fit = parent_fitness[j];
        avg_fit += parent_fitness[j];
      }
      avg_fit = avg_fit / params.mu();
      
      // Log the population's fitness.
      log << num_evals << "\t" << avg_fit << "\t" << best_fit << endl;
      best_fit = 0;

    // Check termination conditions
    }while(!termination(params, num_evals, last_eval_change));
    
    // Reset the router data for next eval.
    for(int j = 0; j < params.mu(); j++)
    {
      for(int k = 0; k < num_routers; k++)
        routers[j][k] = 0;
    }
  }
  
  // Output the solution.
  for(int i = 0; i < num_routers; i++)
  {
    if(global_best_solution[i])
      solution << (i + num_hosts) << "\t";
  }
  
  // Cleanup and exit.
  log.close();
  solution.close();
  network.close();
  for(int i = 0; i < params.mu(); i++)
    delete [] routers[i];
  delete [] routers;
  delete [] global_best_solution;
  for(int i = 0; i < num_hosts; i++)
    delete [] connections[i];
  delete [] connections;
  delete [] parent_fitness;
  delete [] children_fitness;
  for(int i = 0; i < params.lambda(); i++)
    delete [] children[i];
  delete [] children;
  delete [] parents_selected;
  return 0;
}

// Reads in the network parameters.
void read_network(ifstream& network, int &num_hosts, int &num_routers)
{
  network.ignore(256, ':');
  network >> num_hosts;
  network.ignore(256, ':');
  network >> num_routers;
  network.ignore(256, ':');
  return;
}


// Reads in the connections between hosts and routers
void read_connections(ifstream& network, int hosts, bool** connections)
{
  int temp;  // Buffer for reading in values
  for(int i = 0; i < hosts; i++)
  {
    do
    {
      network >> temp;
      if(temp >= hosts)
        connections[i][temp - hosts] = 1;
    }while((temp > hosts || temp == i) && !network.eof());
  }
  return;
}

// Determines the fitness of the solution by adding up all
// the routers that were not disabled.
int fitness(bool * router, int num_routers)
{
  int value = 0;
  for(int i = 0; i < num_routers; i++)
  {
    if(!router[i])
      value++;
  }
  return value;
}

// Determines the penalty as the number of connections to hosts
// that were not severed, along with the penalty coefficient.
int penalty(bool * router, int num_routers, bool ** connection, int num_hosts, int coeff)
{
  int value = 0;
  for(int i = 0; i < num_hosts; i++)
  {
    bool severed = false;
    for(int j = 0; j < num_routers; j++)
    {
      if(connection[i][j])
      {
        for(int k = 0; k < num_routers; k++)
        {
          if(router[k])
          {
            severed = true;   // Sets severed to true, and since
            k = num_routers;  // it is severed you can go to the
            j = num_routers;  // next host.
          }
        }
      }
    }
    if(!severed)
      value++;
  }
  value = value * coeff;
  return value;
}

// Mutates the child trial solution.
void mutate(float probability, bool * solution, int size)
{
  int chance = probability * 100;
  for(int i = 0; i < size; i++)
  {
    if((rand() % 100) < chance)
      solution[i] = !solution[i];
  }
  return;
}

// Selects the parents based on either truncation or tournament.
void parent_selection(config &params, bool ** mu_pop, int * fitness, int * list)
{
  int * tourney_list;
  int * tourney_fitness;
  int temp;
  bool swapped;
  if(params.select_type() == "tournament")
  {
    tourney_list = new int[params.tourney_size()];
    tourney_fitness = new int[params.tourney_size()];
    for(int i = 0; i < params.tourney_size(); i++)
    {
      // Choose randomly for the tournament.
      tourney_list[i] = rand() % params.mu();
      tourney_fitness[i] = tourney_list[i];
    }
    // Sort out the tournament.
    do
    {
      swapped = false;
      for(int i = 0; i < params.tourney_size() - 1; i++)
      {
        if(tourney_fitness[i] < tourney_fitness[i+1])
        {
          temp = tourney_fitness[i];
          tourney_fitness[i] = tourney_fitness[i+1];
          tourney_fitness[i+1] = temp;
          temp = tourney_list[i];
          tourney_list[i] = tourney_list[i+1];
          tourney_list[i+1] = temp;
          swapped = true;
        }
      }
    }while(swapped);
    
    // Copy the winners into the parent list.
    int count = 0;
    for(int i = 0; i < params.num_parents(); i++)
    {
      list[i] = tourney_list[count];
      count++;
      if(count == params.tourney_size())
        count = 0;
    }
    delete [] tourney_list;
    delete [] tourney_fitness;
  }
  else if(params.select_type() == "truncation")
  {
    sort_pop(mu_pop, fitness, params);
    for(int i = 0; i < params.num_parents(); i++)
      list[i] = i;
  }
  else
  {
    cerr << "Invalid selection method. Exiting" << endl;
    exit(0);
  }
  return;
}

// Sorts the population before truncation.
void sort_pop(bool ** mu_pop, int * fitness, config params)
{
  bool * temp;
  int temp_fitness;
  bool sorted = false;
  while(!sorted)
  {
    sorted = true;
    for(int i = 0; i < params.mu() - 1; i++)
    {
      if(fitness[i] < fitness[i + 1])
      {
        temp = mu_pop[i];
        mu_pop[i] = mu_pop[i + 1];
        mu_pop[i + 1] = temp;
        temp_fitness = fitness[i];
        fitness[i] = fitness[i + 1];
        fitness[i + 1] = temp_fitness;
        sorted = false;
      }
    }
  }
  return;
}

// Recombines the parents to make children based on uniform or n-point crossover.
void recombine(config params, int * parents_selected, bool ** parents, bool ** children, int size)
{
  if(params.recombination() == "uniform")
  {
    uniform_crossover(params, parents_selected, parents, children, size);
  }
  else if (params.recombination() == "n-point")
  {
    n_crossover(params, parents_selected, parents, children, size);
  }
  else
  {
    cerr << "Invalid recombination method. Exiting" << endl;
    exit(0);
  }
  return;
}

// Uniformly crosses two parents into one child.
void uniform_crossover(config params, int * parents_selected, bool ** parents, bool ** children, int size)
{
  int p1, p2;
  for(int i = 0; i < params.lambda(); i++)
  {
    // Randomly select 2 parents from the list.
    p1 = rand() % params.num_parents();
    p2 = rand() % params.num_parents();
    
    // Randomly cross the two parents to create a child.
    for(int j = 0; j < size; j++)
    {
      if(rand() % 2)
        children[i][j] = parents[parents_selected[p1]][j];
      else
        children[i][j] = parents[parents_selected[p2]][j];
    }
  }
  return;
}

// Crosses two parents into one child with n break points.
void n_crossover(config params, int * parents_selected, bool ** parents, bool ** children, int size)
{
  int p1, p2;
  int n = params.num_crossovers();
  bool switch_parent = true;
  int segment_size = size / n;
  int counter = 0;
  for(int i = 0; i < params.lambda(); i++)
  {
    // Randomly select 2 parents from the list.
    p1 = rand() % params.num_parents();
    p2 = rand() % params.num_parents();
    
    // Cross the two parents switching at n segments.
    for(int j = 0; j < size; j++)
    {
      if(switch_parent)
        children[i][j] = parents[parents_selected[p1]][j];
      else
        children[i][j] = parents[parents_selected[p2]][j];
      counter++;
      if(counter == segment_size)
      {
        counter = 0;
        switch_parent = !switch_parent;
      }
    }
  }
}

// Checks the termination criteria.
bool termination(config params, int total_evals, int evals_change)
{
  bool result = false;
  if((total_evals >= params.evals()) && (params.term() == "evals"))
    result = true;
  if((evals_change >= params.evals()) && (params.term() == "convergence"))
    result = true;
  return result;
}

