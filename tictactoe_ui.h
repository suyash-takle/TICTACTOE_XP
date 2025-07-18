#ifndef TICTACTOE_UI_H
#define TICTACTOE_UI_H

#include <windows.h>
#include "tictactoe_logic.h"

class TicTacToeUI {
private:
    HWND hwnd; // Main window handle
    HWND buttons[9]; // 3x3 button grid
    HWND status_label; // Status text
    HWND play_x_button, play_o_button, play_again_button; // Control buttons
    TicTacToe game; // Game logic instance
    std::string user; // User's chosen player (X or O)
    bool ai_turn; // Flag for AI's turn
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void create_controls();
    void button_click(int position);
    void choose_player(const std::string& player);
    void update_status(const std::string& text);
    void play_again();
    void handle_ai_move();

public:
    TicTacToeUI(HINSTANCE hInstance);
    int run();
};

#endif
