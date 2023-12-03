#include <stdio.h>

// O(n^2) time complexity
void nSquared(int n) {
    long long result = 0;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf(".");
        }
    }
}

int main() {
    int size = 10000000;
    nSquared(size);

    return 0;
}
