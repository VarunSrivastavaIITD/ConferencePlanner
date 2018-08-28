/* 
 * File:   HillClimb.h
 * Author: Varun Srivastava
 *
 */

#include <algorithm>
#include <numeric>
#include <random>
#include <utility>
#include <chrono>
#include "HillClimb.h"

typedef std::chrono::high_resolution_clock Time;
typedef std::chrono::duration<double> double_seconds;

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

State HillClimb::random_initialize(std::default_random_engine &rng)
{
    State random_state(parallel_tracks * sessions_in_track * papers_in_session);
    std::iota(random_state.begin(), random_state.end(), 0);

    std::shuffle(random_state.begin(), random_state.end(), rng);
    return random_state;
}

std::pair<int, int> HillClimb::next_state(std::default_random_engine &rng)
{
    std::uniform_int_distribution<std::default_random_engine::result_type> dist(0, papers_in_session * parallel_tracks * sessions_in_track - 1);

    int i, j;
    do
    {
        i = dist(rng);
        j = dist(rng);
    } while (((i + papers_in_session) / papers_in_session) == ((j + papers_in_session) / papers_in_session));

    return std::make_pair(i, j);
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
        change = (trade_of_coefficient + 1) * (session_distance_matrix[a][session_seq_a] + session_distance_matrix[b][session_seq_b] - session_distance_matrix[a][session_seq_b] - session_distance_matrix[b][session_seq_a] + 2 * distance_matrix[a][b]);

    else
    {
        change = (trade_of_coefficient + 1) * (session_distance_matrix[a][session_seq_a] + session_distance_matrix[b][session_seq_b] - session_distance_matrix[a][session_seq_b] - session_distance_matrix[b][session_seq_a]) + 2 * distance_matrix[a][b];

        for (int i = 0; i < parallel_tracks; ++i)
            change += trade_of_coefficient * (session_distance_matrix[a][time_slot_b * parallel_tracks + i] + session_distance_matrix[b][time_slot_a * parallel_tracks + i] - session_distance_matrix[a][time_slot_a * parallel_tracks + i] - session_distance_matrix[b][time_slot_b * parallel_tracks + i]);
    }

    return change;
}

State HillClimb::hill_climb(bool random_init, double duration, const int seed = 0)
{
    duration *= 60; // Assumed in minutes originally
    auto initial_time = Time::now();
    decltype(initial_time) now;
    decltype(now - initial_time) dur;
    decltype(std::chrono::duration_cast<double_seconds>(dur)) secs;

    State state, best_state;
    auto n = parallel_tracks * sessions_in_track * papers_in_session;

    std::default_random_engine rng;
    rng.seed(seed);

    double best_score = 0;

    while (secs.count() < duration)
    {
        if (random_init)
            state = random_initialize(rng);
        else
            state = greedy_initialize();
        construct_session_matrix(state);

        for (int cnt = 0; cnt != n / 10 && secs.count() < duration; ++cnt)
        {
            auto pair = next_state(rng);
            auto index_a = pair.first;
            auto index_b = pair.second;
            double score = 0;
            if ((score = score_increment(index_a, index_b, state)) > 0)
            {
                update_state(index_a, index_b, state);
                if (best_score < score)
                {
                    best_score = score;
                    best_state = state;
                }
                cnt = 0;
            }

            now = Time::now();
            dur = now - initial_time;
            secs = std::chrono::duration_cast<double_seconds>(dur);
        }
    };

    return best_state;
}