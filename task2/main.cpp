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

void deposit(long long& sum) {
    long long k;
    if (!(std::cin >> k)) {
        std::cin.clear();
        std::cout << "error\n";
        return;
    }
    if (k <= 0) {
        std::cout << "error\n";
        return;
    }
    sum += k;
}

void withdraw(long long& sum) {
    long long k;
    if (!(std::cin >> k)) {
        std::cin.clear();
        std::cout << "error\n";
        return;
    }
    if (k <= 0 || k > sum) {
        std::cout << "error\n";
        return;
    }
    sum -= k;
}

void balance(long long& sum) {
    std::cout << sum << '\n';
}

void reset(long long& sum) {
    sum = 0;
}

void inc(long long& sum) {
    sum++;
}

struct Command {
    const char* name;
    void (*action)(long long&);
};

int main() {
    long long sum = 0;
    Command commands[] = {
        {"deposit", deposit}, {"withdraw", withdraw}, {"balance", balance},
        {"reset", reset},     {"inc", inc},
    };
    int n = sizeof(commands) / sizeof(commands[0]);
    char cmd[32];

    while (std::cin >> cmd) {
        if (same(cmd, "stop")) {
            break;
        }

        bool found = false;
        for (int i = 0; i < n; i++) {
            if (same(cmd, commands[i].name)) {
                commands[i].action(sum);
                found = true;
                break;
            }
        }
        if (!found) {
            std::cout << "error\n";
        }

        char c;
        while (std::cin.get(c) && c != '\n') {
        }
    }
    return 0;
}
