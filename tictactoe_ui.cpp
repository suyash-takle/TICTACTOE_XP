#include "tictactoe_ui.h"
#include <string>

TicTacToeUI::TicTacToeUI(HINSTANCE hInstance) : user(""), ai_turn(false) {
    // Register window class
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "TicTacToeWindow";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClass(&wc);

    // Create main window
    hwnd = CreateWindow(
        "TicTacToeWindow", "Tic-Tac-Toe",
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 400,
        NULL, NULL, hInstance, this
    );

    create_controls();
    ShowWindow(hwnd, SW_SHOW);
}

void TicTacToeUI::create_controls() {
    // Create player selection buttons
    play_x_button = CreateWindow(
        "BUTTON", "Play as X",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        50, 150, 80, 30,
        hwnd, (HMENU)200, NULL, NULL
    );
    SendMessage(play_x_button, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

    play_o_button = CreateWindow(
        "BUTTON", "Play as O",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        150, 150, 80, 30,
        hwnd, (HMENU)201, NULL, NULL
    );
    SendMessage(play_o_button, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

    // Create status label
    status_label = CreateWindow(
        "STATIC", "Play Tic-Tac-Toe",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        50, 50, 180, 30,
        hwnd, NULL, NULL, NULL
    );
    SendMessage(status_label, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

    // Create game board buttons (initially hidden)
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int pos = i * 3 + j;
            buttons[pos] = CreateWindow(
                "BUTTON", " ",
                WS_CHILD | BS_PUSHBUTTON,
                50 + j * 60, 100 + i * 60, 60, 60,
                hwnd, (HMENU)(100 + pos), NULL, NULL
            );
            SendMessage(buttons[pos], WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
        }
    }

    // Create play again button (initially hidden)
    play_again_button = CreateWindow(
        "BUTTON", "Play Again",
        WS_CHILD | BS_PUSHBUTTON,
        100, 280, 80, 30,
        hwnd, (HMENU)202, NULL, NULL
    );
    SendMessage(play_again_button, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
}

void TicTacToeUI::choose_player(const std::string& player) {
    user = player;
    // Hide player selection buttons
    ShowWindow(play_x_button, SW_HIDE);
    ShowWindow(play_o_button, SW_HIDE);
    // Show game board
    for (int i = 0; i < 9; i++) {
        ShowWindow(buttons[i], SW_SHOW);
    }
    update_status("Play as " + user);
}

void TicTacToeUI::button_click(int position) {
    int i = position / 3, j = position % 3;
    std::set<std::pair<int, int> > valid_actions = game.actions();
    if (valid_actions.find(std::make_pair(i, j)) != valid_actions.end() && user == game.player() && !game.terminal()) {
        game.set_board(game.result(std::make_pair(i, j)));
        SetWindowText(buttons[position], game.get_cell(i, j).c_str());
        EnableWindow(buttons[position], FALSE);
        if (game.terminal()) {
            std::string winner = game.winner();
            if (winner.empty()) {
                update_status("Game Over: Tie.");
            } else {
                update_status("Game Over: " + winner + " wins.");
            }
            ShowWindow(play_again_button, SW_SHOW);
        } else if (game.player() != user) {
            update_status("Computer thinking...");
            ai_turn = true;
        }
    }
}

void TicTacToeUI::handle_ai_move() {
    if (ai_turn && !game.terminal() && game.player() != user) {
        Sleep(500); // Mimic Python's time.sleep(0.5)
        std::pair<int, int> move = game.minimax();
        game.set_board(game.result(move));
        int pos = move.first * 3 + move.second;
        SetWindowText(buttons[pos], game.get_cell(move.first, move.second).c_str());
        EnableWindow(buttons[pos], FALSE);
        ai_turn = false;
        if (game.terminal()) {
            std::string winner = game.winner();
            if (winner.empty()) {
                update_status("Game Over: Tie.");
            } else {
                update_status("Game Over: " + winner + " wins.");
            }
            ShowWindow(play_again_button, SW_SHOW);
        } else {
            update_status("Play as " + user);
        }
    }
}

void TicTacToeUI::update_status(const std::string& text) {
    SetWindowText(status_label, text.c_str());
}

void TicTacToeUI::play_again() {
    game.set_board(game.initial_state());
    user = "";
    ai_turn = false;
    for (int i = 0; i < 9; i++) {
        SetWindowText(buttons[i], " ");
        EnableWindow(buttons[i], TRUE);
        ShowWindow(buttons[i], SW_HIDE);
    }
    ShowWindow(play_again_button, SW_HIDE);
    ShowWindow(play_x_button, SW_SHOW);
    ShowWindow(play_o_button, SW_SHOW);
    update_status("Play Tic-Tac-Toe");
}

LRESULT CALLBACK TicTacToeUI::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    TicTacToeUI* pThis = NULL;
    if (msg == WM_NCCREATE) {
        pThis = (TicTacToeUI*)((CREATESTRUCT*)lParam)->lpCreateParams;
        SetWindowLong(hwnd, GWL_USERDATA, (LONG)pThis);
    } else {
        pThis = (TicTacToeUI*)GetWindowLong(hwnd, GWL_USERDATA);
    }

    if (pThis) {
        switch (msg) {
            case WM_COMMAND: {
                int id = LOWORD(wParam);
                if (id >= 100 && id < 109) { // Grid buttons
                    pThis->button_click(id - 100);
                } else if (id == 200) { // Play as X
                    pThis->choose_player("X");
                } else if (id == 201) { // Play as O
                    pThis->choose_player("O");
                } else if (id == 202) { // Play Again
                    pThis->play_again();
                }
                return 0;
            }
            case WM_TIMER:
                pThis->handle_ai_move();
                return 0;
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int TicTacToeUI::run() {
    SetTimer(hwnd, 1, 100, NULL); // Timer for AI moves
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    TicTacToeUI app(hInstance);
    return app.run();
}