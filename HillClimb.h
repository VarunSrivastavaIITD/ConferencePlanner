/* 
 * File:   HillClimb.h
 * Author: Varun Srivastava
 *
 */

#ifndef HILLCLIMB_H
#define HILLCLIMB_H

#include <vector>

using std::vector;
using State = vector<int>;

class HillClimb
{
private:
  double **distance_matrix;
  int parallel_tracks;
  int sessions_in_track;
  int papers_in_session;
  double trade_of_coefficient;

  vector<vector<double>> session_distance_matrix;

  void construct_session_matrix(State);

  // Initialization Schemes
  State random_initialize(int);
  State greedy_initialize();
  State next_state();

  std::vector<std::vector<int>> state_to_sessions(State);

public:
  // Constructors
  HillClimb(double **, int, int, int, int);

  // Main hill climb algorithm
  State hill_climb(bool);

  // Increment in score when going from state 1 to state 2 by single swap
  double score_increment(int, int, State) const;

  //Update state and session distance matrix after single swap
  void update_state(int, int, State&);
};

#endif