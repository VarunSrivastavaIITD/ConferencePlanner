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
    session_distance_matrix.resize(sessions_in_track * parallel_tracks * papers_in_session);
    for (auto &v : session_distance_matrix)
        v.resize(parallel_tracks * sessions_in_track);

    auto sessions = state_to_sessions(initial_state);

    for (auto i = 0; i != session_distance_matrix.size(); ++i)
    {
        for (auto j = 0; j != sessions.size(); ++j)
        {
            auto dist = 0.0;
            for (const auto &e : sessions[j])
            {
                dist += distance_matrix[i][e];
            }
            session_distance_matrix[i][j] = dist;
        }
    }
}

std::vector<std::vector<int>> HillClimb::state_to_sessions(State state)
{
    std::vector<std::vector<int>> blocks;
    for (const auto &row : state)
    {
        for (auto it = row.cbegin(); it != row.cend(); it += papers_in_session)
            blocks.push_back(std::vector<int>(it, it + papers_in_session));
    }
    return blocks;
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