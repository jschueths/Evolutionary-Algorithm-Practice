/////////////////////////////////////////////////////////////////////
/// @file driver.cpp
/// @author Joshua McCarville-Schueths CS348 Fall 2011
/// @brief Header for random search, Assignment 1a.
/////////////////////////////////////////////////////////////////////

#ifndef __DRIVER_H__
#define __DRIVER_H__

#include <iostream>
#include <fstream>
#include <ctime>
#include <cstring>
#include <cstdlib>
#include "ConfigFile.h"
#include "config.h"

using std::ifstream;


void read_network(ifstream& network, int &num_hosts, int &num_routers);
void read_connections(ifstream& network, int hosts, bool** connections);
int fitness(bool * router, int num_routers);
int penalty(bool * router, int num_routers, bool ** connection, int num_hosts, int coeff);

void mutate(float probability, bool * solution, int size);
void parent_selection(config &params, bool ** mu_pop, int * fitness, int * list);
void sort_pop(bool ** mu_pop, int * fitness, config params);
void recombine(config params, int * parents_selected, bool ** parents, bool ** children, int size);
void uniform_crossover(config params, int * parents_selected, bool ** parents, bool ** children, int size);
void n_crossover(config params, int * parents_selected, bool ** parents, bool ** children, int size);
bool termination(config params, int total_evals, int evals_change);


#endif
