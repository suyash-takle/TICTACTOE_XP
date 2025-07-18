#ifndef TICTACTOE_LOGIC_H
#define TICTACTOE_LOGIC_H

#include <string>
#include <vector>
#include <set>

class TicTacToe {
private:
    std::vector<std::vector<std::string> > board; // 3x3 board
    static const std::string X;
    static const std::string O;
    static const std::string EMPTY;

public:
    TicTacToe();
    std::vector<std::vector<std::string> > initial_state();
    std::string player();
    std::set<std::pair<int, int> > actions();
    std::vector<std::vector<std::string> > result(const std::pair<int, int>& action);
    bool check_row_winner(const std::string& player);
    bool check_col_winner(const std::string& player);
    bool check_diag_winner(const std::string& player);
    std::string winner();
    bool terminal();
    int utility();
    int max_value();
    int min_value();
    std::pair<int, int> minimax();
    void set_board(const std::vector<std::vector<std::string> >& new_board);
    std::string get_cell(int i, int j) const;
};

#endif