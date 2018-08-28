/* 
 * File:   HillClimb.h
 * Author: Varun Srivastava
 *
 */

#include <algorithm>
#include <numeric>
#include <random>
#include "HillClimb.h"

HillClimb::HillClimb(double **matrix, int p, int t, int k, int c)
{
    distance_matrix = matrix;
    parallel_tracks = p;
    sessions_in_track = t;
    papers_in_session = k;
    trade_of_coefficient = c;
}

void HillClimb::construct_session_matrix(State initial_state)
{
    session_distance_matrix.resize(sessions_in_track * parallel_tracks * papers_in_session);
    for (auto &v : session_distance_matrix)
        v.resize(parallel_tracks * sessions_in_track);

    auto sessions = state_to_sessions(initial_state);

    for (size_t i = 0; i != session_distance_matrix.size(); ++i)
    {
        for (size_t j = 0; j != sessions.size(); ++j)
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

    for (auto it = state.cbegin(); it != state.cend(); it += papers_in_session)
        blocks.push_back(std::vector<int>(it, it + papers_in_session));

    return blocks;
}

State HillClimb::greedy_initialize()
{
    return State();
}

State HillClimb::random_initialize(int seed = 0)
{
    State random_state(parallel_tracks * sessions_in_track * papers_in_session);
    std::iota(random_state.begin(), random_state.end(), 0);

    std::mt19937_64 rng(seed);
    std::shuffle(random_state.begin(), random_state.end(), rng);
    return random_state;
}

State HillClimb::next_state()
{
    return State();
}

void HillClimb::update_state(int index_a, int index_b, State &state)
{
    int a = state[index_a];
    int b = state[index_b];
    state[index_a] = b;
    state[index_b] = a;
    int session_seq_a = ((index_a + papers_in_session) / papers_in_session) - 1;
    int session_seq_b = ((index_b + papers_in_session) / papers_in_session) - 1;
    session_distance_matrix[a][session_seq_a] += distance_matrix[a][b];
    session_distance_matrix[a][session_seq_b] -= distance_matrix[a][b];
    session_distance_matrix[b][session_seq_b] += distance_matrix[a][b];
    session_distance_matrix[b][session_seq_a] -= distance_matrix[a][b];
}

double HillClimb::score_increment(int index_a, int index_b, State state) const
{
    double change = 0;
    int a = state[index_a];
    int b = state[index_b];
    int session_seq_a = ((index_a + papers_in_session) / papers_in_session) - 1;
    int session_seq_b = ((index_b + papers_in_session) / papers_in_session) - 1;
    int papers_in_time_slot = papers_in_session * parallel_tracks;
    int time_slot_a = ((index_a + papers_in_time_slot) / papers_in_time_slot) - 1;
    int time_slot_b = ((index_b + papers_in_time_slot) / papers_in_time_slot) - 1;
    if (time_slot_a == time_slot_b)
        change = (trade_of_coefficient + 1) * (session_distance_matrix[a][session_seq_a] + session_distance_matrix[b][session_seq_b] - session_distance_matrix[a][session_seq_b] - session_distance_matrix[b][session_seq_a] - 2 * distance_matrix[a][b]);

    else
    {
        change = (trade_of_coefficient + 1) * (session_distance_matrix[a][session_seq_a] + session_distance_matrix[b][session_seq_b] - session_distance_matrix[a][session_seq_b] - session_distance_matrix[b][session_seq_a] + 2 * distance_matrix[a][b]);

        for (int i = 0; i < parallel_tracks; ++i)
            change += trade_of_coefficient * (session_distance_matrix[a][time_slot_b * parallel_tracks + i] + session_distance_matrix[b][time_slot_a * parallel_tracks + i] - session_distance_matrix[a][time_slot_a * parallel_tracks + i] - session_distance_matrix[b][time_slot_b * parallel_tracks + i]);
    }

    return change;
}