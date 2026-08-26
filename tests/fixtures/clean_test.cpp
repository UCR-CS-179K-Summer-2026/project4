// Clean control fixture: descriptive names, comments, and no targeted smell patterns.
int calculateAverage(int firstValue, int secondValue, int thirdValue) {
    // Keep the calculation straightforward for the clean baseline.
    int totalValue = firstValue + secondValue + thirdValue;
    return totalValue / 3;
}

int main() {
    // The ordinary baseline should not produce detector warnings.
    return calculateAverage(3, 6, 9);
}
