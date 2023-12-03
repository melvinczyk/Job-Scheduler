#include <stdio.h>

// O(n!) time complexity
unsigned long long factorial(int n) {
    if (n == 0 || n == 1) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}

int main() {
    int n = 30;

    printf("Factorial of %d is %llu\n", n, factorial(n));

    return 0;
}
