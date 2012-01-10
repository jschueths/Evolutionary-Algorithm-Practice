/////////////////////////////////////////////////////////////////////
/// @file config.h
/// @author Joshua McCarville-Schueths CS348 Fall 2011
/// @brief Header for config data type, Assignment 1b.
/////////////////////////////////////////////////////////////////////

// This is a parameter configuration data type, to help keep the rest of the code a little tidier I chose to put all of the configuration parameters into a class. Yes, a little extra runtime and overhead, but for now I'm willing to sacrifice it for the sake of being somewhat organized.

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "ConfigFile.h"
#include <string>
#include <ctime>
#include <cstdlib>

using std::string;

class config
{
  public:    
    // Mutator
    void init(ConfigFile &confile);
    
    // Accessors
    const int& mu();
    const int& lambda();
    const int& penalty();
    const int& runs();
    const int& evals();
    const int& num_parents();
    const int& num_crossovers();
    const int& tourney_size();
    const float& probability();
    const string& input_file();
    const string& log_file();
    const string& solution_file();
    const string& select_type();
    const string& recombination();
    const string& term();
    const bool& random();
    const unsigned int& seed();
    
  private:
    int m_mu;
    int m_lambda;
    int m_penalty;
    int m_runs;
    int m_evals;
    int m_parents;
    int m_crossovers;
    int m_tourney_size;
    float m_probability;
    string m_log_file;
    string m_solution_file;
    string m_input_file;
    string m_select_type;
    string m_recombination;
    string m_term;
    unsigned int m_seed;
    bool m_random;  
};

#include "config.hpp"
#endif
