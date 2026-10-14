#include <iostream>

int main() {
    // int*** хранит количества: отдел -> стеллаж -> полка.
    // Все три размера задаются при запуске, поэтому выделяем память через new.
    // void* нужен для общего каталога двух разных наборов данных:
    // трёхмерного массива чисел и массива буквенных меток.
    // Пользователь выбирает набор по номеру; затем восстанавливаем его тип.

    int n, m, k;

    int*** a = new int**[n];
    for (int i = 0; i < n; i++) {
        a[i] = new int*[m];
        for (int j = 0; j < m; j++) {
            a[i][j] = new int[k];
        }
    }
    char* marks = new char[n];

    bool ok = true;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            for (int t = 0; t < k; t++) {
                if (!(std::cin >> a[i][j][t])) {
                    ok = false;
                }
            }
        }
    }
    for (int i = 0; i < n; i++) {
        if (!(std::cin >> marks[i])) {
            ok = false;
        }
    }

    void* data[2] = {a, marks};
    int type;
    while (ok && std::cin >> type) {
        if (type == -1) {
            break;
        }
        if (type < 0 || type > 1) {
            std::cout << "error\n";
            continue;
        }

        void* p = data[type];
        if (type == 0) {
            int*** counts = static_cast<int***>(p);
            for (int i = 0; i < n; i++) {
                long long sum = 0;
                for (int j = 0; j < m; j++) {
                    for (int t = 0; t < k; t++) {
                        sum += counts[i][j][t];
                    }
                }
                std::cout << i << ' ' << sum << '\n';
            }
        } else {
            char* names = static_cast<char*>(p);
            for (int i = 0; i < n; i++) {
                std::cout << i << ' ' << names[i] << '\n';
            }
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            delete[] a[i][j];
        }
        delete[] a[i];
    }
    delete[] a;
    delete[] marks;
    if (!ok) {
        return 1;
    }
    return 0;
}
