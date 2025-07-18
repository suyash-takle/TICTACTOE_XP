#include "tictactoe_logic.h"
#include <climits>
#include <algorithm>

const std::string TicTacToe::X = "X";
const std::string TicTacToe::O = "O";
const std::string TicTacToe::EMPTY = "";

TicTacToe::TicTacToe() {
    board = initial_state();
}

std::vector<std::vector<std::string> > TicTacToe::initial_state() {
    std::vector<std::vector<std::string> > new_board(3, std::vector<std::string>(3, EMPTY));
    return new_board;
}

std::string TicTacToe::player() {
    int countX = 0, countO = 0;
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            if (board[row][col] == X) countX++;
            else if (board[row][col] == O) countO++;
        }
    }
    return (countX == countO) ? X : O;
}

std::set<std::pair<int, int> > TicTacToe::actions() {
    std::set<std::pair<int, int> > action_set;
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            if (board[row][col] == EMPTY) {
                action_set.insert(std::make_pair(row, col));
            }
        }
    }
    return action_set;
}

std::vector<std::vector<std::string> > TicTacToe::result(const std::pair<int, int>& action) {
    int i = action.first, j = action.second;
    std::set<std::pair<int, int> > valid_actions = actions();
    if (valid_actions.find(action) == valid_actions.end()) {
        throw std::string("Invalid Action");
    }
    std::vector<std::vector<std::string> > new_board = board;
    new_board[i][j] = player();
    return new_board;
}

bool TicTacToe::check_row_winner(const std::string& player) {
    for (int row = 0; row < 3; row++) {
        if (board[row][0] == player && board[row][1] == player && board[row][2] == player) {
            return true;
        }
    }
    return false;
}

bool TicTacToe::check_col_winner(const std::string& player) {
    for (int col = 0; col < 3; col++) {
        if (board[0][col] == player && board[1][col] == player && board[2][col] == player) {
            return true;
        }
    }
    return false;
}

bool TicTacToe::check_diag_winner(const std::string& player) {
    if (board[0][0] == player && board[1][1] == player && board[2][2] == player) {
        return true;
    }
    if (board[0][2] == player && board[1][1] == player && board[2][0] == player) {
        return true;
    }
    return false;
}

std::string TicTacToe::winner() {
    if (check_row_winner(X) || check_col_winner(X) || check_diag_winner(X)) {
        return X;
    }
    if (check_row_winner(O) || check_col_winner(O) || check_diag_winner(O)) {
        return O;
    }
    return "";
}

bool TicTacToe::terminal() {
    if (!winner().empty()) return true;
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            if (board[row][col] == EMPTY) return false;
        }
    }
    return true;
}

int TicTacToe::utility() {
    std::string win = winner();
    if (win == X) return 1;
    if (win == O) return -1;
    return 0;
}

int TicTacToe::max_value() {
    if (terminal()) return utility();
    int v = INT_MIN;
    std::set<std::pair<int, int> > action_set = actions();
    for (std::set<std::pair<int, int> >::iterator it = action_set.begin(); it != action_set.end(); ++it) {
        TicTacToe temp;
        temp.set_board(result(*it));
        v = std::max(v, temp.min_value());
    }
    return v;
}

int TicTacToe::min_value() {
    if (terminal()) return utility();
    int v = INT_MAX;
    std::set<std::pair<int, int> > action_set = actions();
    for (std::set<std::pair<int, int> >::iterator it = action_set.begin(); it != action_set.end(); ++it) {
        TicTacToe temp;
        temp.set_board(result(*it));
        v = std::min(v, temp.max_value());
    }
    return v;
}

std::pair<int, int> TicTacToe::minimax() {
    if (terminal()) return std::make_pair(-1, -1);
    std::vector<std::pair<int, std::pair<int, int> > > plays;
    std::set<std::pair<int, int> > action_set = actions();
    if (player() == X) {
        for (std::set<std::pair<int, int> >::iterator it = action_set.begin(); it != action_set.end(); ++it) {
            TicTacToe temp;
            temp.set_board(result(*it));
            plays.push_back(std::make_pair(temp.min_value(), *it));
        }
        std::sort(plays.begin(), plays.end());
        return plays.back().second;
    } else {
        for (std::set<std::pair<int, int> >::iterator it = action_set.begin(); it != action_set.end(); ++it) {
            TicTacToe temp;
            temp.set_board(result(*it));
            plays.push_back(std::make_pair(temp.max_value(), *it));
        }
        std::sort(plays.begin(), plays.end());
        return plays.front().second;
    }
}

void TicTacToe::set_board(const std::vector<std::vector<std::string> >& new_board) {
    board = new_board;
}

std::string TicTacToe::get_cell(int i, int j) const {
    if (i >= 0 && i < 3 && j >= 0 && j < 3) {
        return board[i][j];
    }
    return "";
}
