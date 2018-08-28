/* 
 * File:   HillClimb.h
 * Author: Varun Srivastava
 *
 */

#include "HillClimb.h"

HillClimb::HillClimb(double **matrix, int p, int t, int k)
{
    distance_matrix = matrix;
    parallel_tracks = p;
    sessions_in_track = t;
    papers_in_session = k;
}

HillClimb::HillClimb(double **matrix, int p, int t, int k, State state)
{
    distance_matrix = matrix;
    parallel_tracks = p;
    sessions_in_track = t;
    papers_in_session = k;
    initial_state = state;
}

void HillClimb::construct_session_matrix()
{
    session_distance_matrix.resize(sessions_in_track);
    for (auto &v : session_distance_matrix)
        v.resize(parallel_tracks);

    if (!initial_state.empty())
    {
        }
}

void HillClimb::greedy_initialize()
{
}

void HillClimb::random_initialize()
{
}

double HillClimb::score_increment(State state1, State state2) const
{
    return 0;
}