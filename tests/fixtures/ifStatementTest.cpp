int invalidDeepNestedIf() { // FLAGS for deeply nested if statements
    int number = 5;
    if (number > 0) {
        if (number < 10) {
            if (number % 2 == 0) {
                if (number == 6) {
                    if(number == 5) {
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