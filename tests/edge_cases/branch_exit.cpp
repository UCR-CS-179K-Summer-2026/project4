// Negative control: a return in an if without an else does not make the block exit.
int conditionalReturn(int value) {
    if (value > 0) {
        return value;
    }
    return 0; // Expected: reachable, so no unreachable-code warning.
}

int main() {
    return conditionalReturn(1);
}
