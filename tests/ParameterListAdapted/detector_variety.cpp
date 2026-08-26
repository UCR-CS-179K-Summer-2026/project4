#include <iostream>
#include <string>

// Collects the same checkout data group used by the other record functions.
void prepareCheckout(std::string firstName, std::string lastName, std::string city) {
    std::cout << firstName << lastName << city << std::endl;
}

// Collects the same checkout data group for a receipt.
void printCheckout(std::string firstName, std::string lastName, std::string city) {
    std::cout << firstName << lastName << city << std::endl;
}

// Collects the same checkout data group for delivery.
void deliverCheckout(std::string firstName, std::string lastName, std::string city) {
    std::cout << firstName << lastName << city << std::endl;
}

// Exercises the long-parameter-list detector.
int calculateCheckout(int itemCount, int unitPrice, int discount, int tax, int shipping) {
    return itemCount * unitPrice - discount + tax + shipping;
}

// Exercises redundant boolean comparison.
bool acceptsCheckout(bool isAvailable) {
    return isAvailable == true;
}

// Exercises repeated statements inside one function.
int repeatCheckoutSteps(int checkoutCount) {
    checkoutCount += 1;
    checkoutCount += 2;
    checkoutCount += 3;
    checkoutCount += 1;
    checkoutCount += 2;
    checkoutCount += 3;
    return checkoutCount;
}

// Exercises deeply nested conditional handling.
int inspectCheckout(int checkoutCount) {
    int result = checkoutCount;
    if (checkoutCount > 0) {
        if (checkoutCount > 1) {
            if (checkoutCount > 2) {
                if (checkoutCount > 3) {
                    result += 1;
                }
            }
        }
    }
    return result;
}

// Exercises the allocation checker.
int* allocateCheckoutCount() {
    int* checkoutCount = new int(1);
    return checkoutCount;
}

// This function is intentionally unreachable from main.
void unusedCheckoutHelper() {}

class CheckoutBase {
public:
    // Dispatches a checkout for the base interface.
    virtual void dispatch() {}
};

class UnusedCheckoutInheritance : public CheckoutBase {
public:
    // Prepares the checkout without using the base interface.
    void prepare() {}
};

// Runs the supported detector variety case.
int main() {
    prepareCheckout("A", "B", "C");
    printCheckout("A", "B", "C");
    deliverCheckout("A", "B", "C");
    std::cout << calculateCheckout(2, 10, 1, 2, 3) << std::endl;
    std::cout << acceptsCheckout(true) << std::endl;
    std::cout << repeatCheckoutSteps(0) << std::endl;
    std::cout << inspectCheckout(4) << std::endl;
    UnusedCheckoutInheritance checkout;
    checkout.prepare();
    return *allocateCheckoutCount();
}
