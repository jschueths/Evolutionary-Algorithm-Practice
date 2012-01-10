/////////////////////////////////////////////////////////////////////
/// @file config.hpp
/// @author Joshua McCarville-Schueths CS348 Fall 2011
/// @brief Implementation for config data type, Assignment 1b.
/////////////////////////////////////////////////////////////////////

void config::init(ConfigFile &confile)
{
  // Read the file names
  m_input_file = confile.read<string>("data_file");
  m_solution_file = confile.read<string>("solution_file");
  m_log_file = confile.read<string>("log_file");
  
  // Read the parameters
  m_mu = confile.read<int>("mu");
  m_lambda = confile.read<int>("lambda");
  m_penalty = confile.read<int>("penalty");
  m_random = confile.read<bool>("random");
  m_runs = confile.read<int>("runs");
  m_evals = confile.read<int>("evals");
  m_parents = confile.read<int>("num_parents");
  m_select_type = confile.read<string>("selection_type");
  if(m_select_type == "tournament")
    m_tourney_size = confile.read<int>("tournament_size");
  m_recombination = confile.read<string>("recombination");
  if(m_recombination == "n-point")
    m_crossovers = confile.read<int>("num_crossovers");
  m_term = confile.read<string>("termination_type");
  m_probability = confile.read<float>("mutation_probability");
  if(!m_random)
    m_seed = confile.read<unsigned int>("seed");
  else
    m_seed = time(NULL);
}

const int& config::mu()
{
  return m_mu;
}

const int& config::lambda()
{
  return m_lambda;
}

const int& config::penalty()
{
  return m_penalty;
}

const int& config::runs()
{
  return m_runs;
}

const int& config::evals()
{
  return m_evals;
}

const int& config::num_parents()
{
  return m_parents;
}

const int& config::num_crossovers()
{
  return m_crossovers;
}

const int& config::tourney_size()
{
  return m_tourney_size;
}

const float& config::probability()
{
  return m_probability;
}

const string& config::input_file()
{
  return m_input_file;
}

const string& config::log_file()
{
  return m_log_file;
}

const string& config::solution_file()
{
  return m_solution_file;
}

const string& config::select_type()
{
  return m_select_type;
}

const string& config::recombination()
{
  return m_recombination;
}

const string& config::term()
{
  return m_term;
}

const bool& config::random()
{
  return m_random;
}

const unsigned int& config::seed()
{
  return m_seed;
}
