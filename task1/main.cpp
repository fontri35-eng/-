#include <iostream>

struct Result {
    int min;
    int max;
};

double average(const int* a, int n) {
    long long sum = 0;
    for (int i = 0; i < n; i++) {
        sum += a[i];
    }
    return static_cast<double>(sum) / n;
}

Result minMax(const int* a, int n) {
    Result result = {a[0], a[0]};
    for (int i = 1; i < n; i++) {
        if (a[i] < result.min) {
            result.min = a[i];
        }
        if (a[i] > result.max) {
            result.max = a[i];
        }
    }
    return result;
}

int frost(const int* a, int n) {
    int count = 0;
    for (int i = 0; i < n; i++) {
        if (a[i] < 0) {
            count++;
        }
    }
    return count;
}

int main() {
    int n;

    int* a = new int[n];

    double mean = average(a, n);
    Result result = minMax(a, n);
    int count = frost(a, n);

    std::cout << std::fixed;
    std::cout.precision(1);
    std::cout << mean << '\n';
    std::cout << result.min << ' ' << result.max << '\n';
    std::cout << count << '\n';

    delete[] a;
    return 0;
}
