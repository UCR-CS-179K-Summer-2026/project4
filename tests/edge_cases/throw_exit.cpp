// Known limitation fixture: throw is an exit path not currently modeled by DeadCodeChecker.
int throwsBeforeStatement() {
    throw 42;
    return 0; // Expected limitation: no unreachable-code warning is currently reported.
}

int main() {
    return 0;
}
