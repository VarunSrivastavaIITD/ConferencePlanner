/* 
 * File:   HillClimb.h
 * Author: Varun Srivastava
 *
 */

#include "HillClimb.h"

HillClimb::HillClimb(double **matrix, int p, int t, int k, int c)
{
    distance_matrix = matrix;
    parallel_tracks = p;
    sessions_in_track = t;
    papers_in_session = k;
    trade_of_coefficient = c;

}

HillClimb::HillClimb(double **matrix, int p, int t, int k, int c, State state)
{
    distance_matrix = matrix;
    parallel_tracks = p;
    sessions_in_track = t;
    papers_in_session = k;
    trade_of_coefficient = c;
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

void HillClimb::updateState(int index_a, int index_b, State state){
    int temp = state[index_a];
    int session_seq_a = (index_a + papers_in_session)/papers_in_session - 1;
    int session_seq_b = (index_b + papers_in_session)/papers_in_session - 1;
    state[index_a] = state[index_b];
    state[index_b] = temp;
    session_distance_matrix[a][session_seq_a] += distance_matrix[a][b];
    session_distance_matrix[a][session_seq_b] -= distance_matrix[a][b];
    session_distance_matrix[b][session_seq_b] += distance_matrix[a][b];
    session_distance_matrix[b][session_seq_a] -= distance_matrix[a][b];
}

double HillClimb::score_increment(int index_a, int a, int index_b, int b) const
{
    double change = 0;
    int session_seq_a = (index_a + papers_in_session)/papers_in_session - 1;
    int session_seq_b = (index_b + papers_in_session)/papers_in_session - 1;
    int papers_in_time_slot = papers_in_session*parallel_tracks;
    int time_slot_a = (index_a+papers_in_time_slot)/(papers_in_time_slot)-1;
    int time_slot_b = (index_b+papers_in_time_slot)/(papers_in_time_slot)-1;
    if(time_slot_a == time_slot_b)
        change = (trade_of_coefficient+1)*(session_distance_matrix[a][session_seq_a] 
                                          +session_distance_matrix[b][session_seq_b] 
                                          -session_distance_matrix[a][session_seq_b]
                                          -session_distance_matrix[b][session_seq_a]
                                          -2*distance_matrix[a][b]);
                    

    else{
        change = (trade_of_coefficient+1)*(session_distance_matrix[a][session_seq_a]
                 +session_distance_matrix[b][session_seq_b]
                 -session_distance_matrix[a][session_seq_b]
                 -session_distance_matrix[b][session_seq_a]
                 +2*distance_matrix[a][b]);

        for(int i=0;i<p;++i)
            change += trade_of_coefficient*(session_distance_matrix[a][time_slot_b*parallel_tracks+i]
                                           +session_distance_matrix[b][time_slot_a*parallel_tracks+i]
                                           -session_distance_matrix[a][time_slot_a*parallel_tracks+i]
                                           -session_distance_matrix[b][time_slot_b*parallel_tracks+i]);
        
    }

    return change;
}