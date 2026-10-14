#include <iostream>

bool same(const char* a, const char* b) {
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) {
            return false;
        }
        i++;
    }
    return a[i] == b[i];
}

bool inside(int x, int y) {
    return x >= 0 && x < 10 && y >= 0 && y < 5;
}

void clear(char board[5][10]) {
    for (int y = 0; y < 5; y++) {
        for (int x = 0; x < 10; x++) {
            board[y][x] = ' ';
        }
    }
}

bool put(char board[5][10], int x, int y, char c) {
    if (!inside(x, y)) {
        return false;
    }
    board[y][x] = c;
    return true;
}

bool text(char board[5][10], int x, int y, const char* word) {
    int len = 0;
    while (word[len] != '\0') {
        len++;
    }

    if (!inside(x, y) || len > 10 - x) {
        return false;
    }
    for (int i = 0; i < len; i++) {
        put(board, x + i, y, word[i]);
    }
    return true;
}

void print(const char board[5][10]) {
    for (int y = 0; y < 5; y++) {
        for (int x = 0; x < 10; x++) {
            std::cout << board[y][x];
        }
        std::cout << '\n';
    }
}

int main() {
    char board[5][10];
    clear(board);
    char cmd[32];

    while (std::cin >> cmd) {
        if (same(cmd, "quit")) {
            break;
        }
        if (same(cmd, "clear")) {
            clear(board);
        } else if (same(cmd, "put")) {
            int x, y;
            char c;
            if (!(std::cin >> x >> y >> c)) {
                return 1;
            }
            if (!put(board, x, y, c)) {
                std::cout << "error\n";
            }
        } else if (same(cmd, "text")) {
            int x, y;
            char word[12];
            if (!(std::cin >> x >> y >> word)) {
                return 1;
            }
            if (!text(board, x, y, word)) {
                std::cout << "error\n";
            }
        } else if (same(cmd, "print")) {
            print(board);
        } else {
            std::cout << "error\n";
        }

        char c;
        while (std::cin.get(c) && c != '\n') {
        }
    }
    return 0;
}
