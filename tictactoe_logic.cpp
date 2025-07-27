#include "tictactoe_ui.h"
#include <string>

// Removed the global g_pTicTacToeUIInstance as it's no longer needed
// with the correct WM_NCCREATE instance handling.

TicTacToeUI::TicTacToeUI(HINSTANCE hInstance) : user(""), ai_turn(false),
    hFontStatus(NULL), hFontButtons(NULL), hFontControlButtons(NULL) {

    // No need to set g_pTicTacToeUIInstance = this; here anymore.

    // Register window class
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW); // Set cursor
    wc.lpszClassName = "TicTacToeWindow";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // Default window background
    RegisterClass(&wc);

    // Create main window
    // Adjusted size for better layout
    // *** CRITICAL FIX: Pass 'this' as the last parameter (lpParam) to CreateWindow ***
    hwnd = CreateWindow(
        "TicTacToeWindow", "Tic-Tac-Toe",
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX, // Disable maximize and resize
        CW_USEDEFAULT, CW_USEDEFAULT, 320, 420, // Increased window size slightly
        NULL, NULL, hInstance, this // <-- THIS IS THE FIX: Pass 'this' here
    );

    // Create fonts
    hFontStatus = CreateFont(
        24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial"
    );

    hFontButtons = CreateFont(
        36, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial"
    );

    hFontControlButtons = CreateFont(
        18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial"
    );

    create_controls();
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd); // Ensure window is drawn
}

TicTacToeUI::~TicTacToeUI() {
    // Clean up GDI objects
    if (hFontStatus) DeleteObject(hFontStatus);
    if (hFontButtons) DeleteObject(hFontButtons);
    if (hFontControlButtons) DeleteObject(hFontControlButtons);
}

void TicTacToeUI::create_controls() {
    // Create player selection buttons
    play_x_button = CreateWindow(
        "BUTTON", "Play as X",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        40, 200, 100, 40, // Larger buttons, adjusted position
        hwnd, (HMENU)200, NULL, NULL
    );
    SendMessage(play_x_button, WM_SETFONT, (WPARAM)hFontControlButtons, TRUE);

    play_o_button = CreateWindow(
        "BUTTON", "Play as O",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        180, 200, 100, 40, // Larger buttons, adjusted position
        hwnd, (HMENU)201, NULL, NULL
    );
    SendMessage(play_o_button, WM_SETFONT, (WPARAM)hFontControlButtons, TRUE);

    // Create status label
    status_label = CreateWindow(
        "STATIC", "Play Tic-Tac-Toe",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        20, 50, 280, 40, // Larger and wider status label
        hwnd, NULL, NULL, NULL
    );
    SendMessage(status_label, WM_SETFONT, (WPARAM)hFontStatus, TRUE);

    // Create game board buttons (initially hidden)
    // Centered the board visually
    int board_start_x = 60;
    int board_start_y = 120;
    int button_size = 60; // Keep individual button size for 3x3 grid

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int pos = i * 3 + j;
            buttons[pos] = CreateWindow(
                "BUTTON", " ",
                WS_CHILD | BS_PUSHBUTTON,
                board_start_x + j * button_size, board_start_y + i * button_size, button_size, button_size,
                hwnd, (HMENU)(100 + pos), NULL, NULL
            );
            SendMessage(buttons[pos], WM_SETFONT, (WPARAM)hFontButtons, TRUE);
        }
    }

    // Create play again button (initially hidden)
    play_again_button = CreateWindow(
        "BUTTON", "Play Again",
        WS_CHILD | BS_PUSHBUTTON,
        110, 320, 100, 40, // Larger button, adjusted position
        hwnd, (HMENU)202, NULL, NULL
    );
    SendMessage(play_again_button, WM_SETFONT, (WPARAM)hFontControlButtons, TRUE);
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
    // Force a repaint to draw the board lines
    InvalidateRect(hwnd, NULL, TRUE);
    UpdateWindow(hwnd);

    // FIX: If AI is 'X' and user is 'O', trigger AI's first move
    if (game.player() != user) { // game.player() will be "X" at the start
        update_status("Computer thinking...");
        ai_turn = true;
    }
}

void TicTacToeUI::button_click(int position) {
    int i = position / 3, j = position % 3;
    std::set<std::pair<int, int> > valid_actions = game.actions();
    if (valid_actions.find(std::make_pair(i, j)) != valid_actions.end() && user == game.player() && !game.terminal()) {
        game.set_board(game.result(std::make_pair(i, j)));
        SetWindowText(buttons[position], game.get_cell(i, j).c_str());
        EnableWindow(buttons[position], FALSE); // Disable button after click
        if (game.terminal()) {
            std::string winner = game.winner();
            if (winner.empty()) {
                update_status("Game Over: Tie.");
            } else {
                update_status("Game Over: " + winner + " wins!");
            }
            ShowWindow(play_again_button, SW_SHOW);
        } else if (game.player() != user) {
            update_status("Computer thinking...");
            ai_turn = true;
        }
    }
}

void TicTacToeUI::handle_ai_move() {
    // Only proceed if it's AI's turn, game is not terminal, and AI is not the user
    // The condition 'game.player() != user' is crucial here.
    if (ai_turn && !game.terminal() && game.player() != user) {
        // Sleep for a short duration to simulate "thinking"
        Sleep(500); // Mimic Python's time.sleep(0.5)

        std::pair<int, int> move = game.minimax();
        // Check if minimax returned a valid move (e.g., not (-1, -1) for a terminal state)
        if (move.first != -1 || move.second != -1) {
            game.set_board(game.result(move));
            int pos = move.first * 3 + move.second;
            SetWindowText(buttons[pos], game.get_cell(move.first, move.second).c_str());
            EnableWindow(buttons[pos], FALSE); // Disable button after AI move
        }
        ai_turn = false; // AI's turn ends

        if (game.terminal()) {
            std::string winner = game.winner();
            if (winner.empty()) {
                update_status("Game Over: Tie.");
            } else {
                update_status("Game Over: " + winner + " wins!");
            }
            ShowWindow(play_again_button, SW_SHOW);
        } else {
            update_status("Play as " + user + " (Your turn)");
        }
    }
}

void TicTacToeUI::update_status(const std::string& text) {
    SetWindowText(status_label, text.c_str());
}

void TicTacToeUI::play_again() {
    game.set_board(game.initial_state()); // Reset game board
    user = ""; // Clear user choice
    ai_turn = false; // Reset AI turn flag

    for (int i = 0; i < 9; i++) {
        SetWindowText(buttons[i], " "); // Clear button text
        EnableWindow(buttons[i], TRUE); // Re-enable buttons
        ShowWindow(buttons[i], SW_HIDE); // Hide buttons
    }
    ShowWindow(play_again_button, SW_HIDE); // Hide play again button
    ShowWindow(play_x_button, SW_SHOW); // Show player selection buttons
    ShowWindow(play_o_button, SW_SHOW); // Show player selection buttons
    update_status("Play Tic-Tac-Toe");
    InvalidateRect(hwnd, NULL, TRUE); // Invalidate to clear drawn board lines
    UpdateWindow(hwnd);
}

// New function to draw the Tic-Tac-Toe grid lines
void TicTacToeUI::draw_board_lines(HDC hdc) {
    // Only draw board lines if the game has started (i.e., user has chosen a player)
    // and the board buttons are visible.
    if (!user.empty() && IsWindowVisible(buttons[0])) {
        HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0)); // Black pen, 2 pixels wide
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

        int board_start_x = 60;
        int board_start_y = 120;
        int button_size = 60;
        int board_dim = button_size * 3; // Total width/height of the board

        // Draw vertical lines
        MoveToEx(hdc, board_start_x + button_size, board_start_y, NULL);
        LineTo(hdc, board_start_x + button_size, board_start_y + board_dim);

        MoveToEx(hdc, board_start_x + 2 * button_size, board_start_y, NULL);
        LineTo(hdc, board_start_x + 2 * button_size, board_start_y + board_dim);

        // Draw horizontal lines
        MoveToEx(hdc, board_start_x, board_start_y + button_size, NULL);
        LineTo(hdc, board_start_x + board_dim, board_start_y + button_size);

        MoveToEx(hdc, board_start_x, board_start_y + 2 * button_size, NULL);
        LineTo(hdc, board_start_x + board_dim, board_start_y + 2 * button_size);

        SelectObject(hdc, hOldPen); // Restore old pen
        DeleteObject(hPen); // Delete created pen
    }
}


LRESULT CALLBACK TicTacToeUI::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    // Retrieve the TicTacToeUI instance from the window's user data
    // This is the correct way to associate a class instance with a window.
    TicTacToeUI* pThis = (TicTacToeUI*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (msg) {
        case WM_NCCREATE: {
            // Store the 'this' pointer in the window's user data
            LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
            // The lpCreateParams field of CREATESTRUCT contains the lpParam passed to CreateWindow.
            pThis = (TicTacToeUI*)lpcs->lpCreateParams;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
            return DefWindowProc(hwnd, msg, wParam, lParam);
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            if (pThis) {
                pThis->draw_board_lines(hdc);
            }
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_COMMAND: {
            if (pThis) { // Ensure pThis is valid
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
            }
            return 0;
        }
        case WM_TIMER:
            if (pThis) {
                pThis->handle_ai_move();
            }
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int TicTacToeUI::run() {
    // Set a timer to periodically check for AI moves
    // This allows the UI to remain responsive while AI "thinks"
    SetTimer(hwnd, 1, 100, NULL); // Timer ID 1, 100ms interval
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    KillTimer(hwnd, 1); // Destroy the timer when message loop ends
    return (int)msg.wParam;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    TicTacToeUI app(hInstance);
    return app.run();
}
