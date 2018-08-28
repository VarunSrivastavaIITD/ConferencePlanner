/* 
 * File:   HillClimb.h
 * Author: Varun Srivastava
 *
 */

#ifndef HILLCLIMB_H
#define HILLCLIMB_H

#include <vector>

using std::vector;
using State = vector<vector<int>>;

class HillClimb
{
  private:
    double **distance_matrix;
    int parallel_tracks;
    int sessions_in_track;
    int papers_in_session;

    vector<vector<double>> session_distance_matrix;
    State initial_state;

    void construct_session_matrix();

    // Initialization Schemes
    void random_initialize();
    void greedy_initialize();

    std::vector<std::vector<int>> state_to_sessions(State);

  public:
    // Constructors
    HillClimb(double **, int, int, int);
    HillClimb(double **, int, int, int, State);

    // Main hill climb algorithm
    State hill_climb(bool);

    // Increment in score when going from state 1 to state 2
    double score_increment(State, State) const;
};

#endif