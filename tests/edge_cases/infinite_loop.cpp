// Known limitation: infinite loops are not currently modeled as exit paths.
int neverReturns() {
    while (true) {
    }
    return 1; // Expected desired behavior: unreachable-code warning.
}

int main() {
    return 0;
}
