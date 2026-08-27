/* 4 flags found in this file */

int invalidDeepNestedIf() { // FLAGS for deeply nested if statements
    int number = 5;
    if (number > 0) {
        if (number < 10) {
            if (number % 2 == 0) {
                if (number == 6) { // Flags this line
                    if(number == 5) { // Flags this line
                        return 1;
                    }
                }
            }
        }
    }
    return -1;
}

int validNestedIf() { // DOES NOT FLAG for deeply nested if statements
    int number = 5;
    if (number > 0) {
        if (number < 10) {
            if (number % 2 == 0) {
                return 1;
            }
        }
    }
    return -1;
}

int validNestedIfWithElse() { // DOES NOT FLAG for deeply nested if statements
    int number = 5;
    if (number > 0) {
        if (number < 10) {
            if (number % 2 == 0) {
                return 1;
            } else {
                return 2;
            }
        }
    }
    return -1;
}

int invalidDeepNestedIfWithElse() { // FLAGS for deeply nested if statements
    int number = 5;
    if (number > 0) {
        if (number < 10) {
            if (number % 2 == 0) {
                number = 6;
            } else {
                if(number == 5) { // Flags this line
                    return 1;
                }
            }
        }
    }
    return -1;
}

int validInsideLoop() { // DOES NOT FLAG for deeply nested if statements
    int number = 5;
    for(int i = 0; i < 10; i++) {
        if (number > 0) {
            if (number < 10) {
                if (number % 2 == 0) {
                    return 1;
                }
            }
        }
    }
    return -1;
}

int invalidInsideLoop() { // FLAGS for deeply nested if statements
    int number = 5;
    for(int i = 0; i < 10; i++) {
        if (number > 0) {
            if (number < 10) {
                if (number % 2 == 0) {
                    if(number == 6) { // Flags this line
                        return 1;
                    }
                }
            }
        }
    }
    return -1;
}