#include <iostream>
#include <string>

// Collects the same customer data group used by the other record functions.
void prepareCustomer(std::string firstName, std::string lastName, std::string city) {
    std::cout << firstName << lastName << city << std::endl;
}

// Collects the same customer data group for a receipt.
void printCustomer(std::string firstName, std::string lastName, std::string city) {
    std::cout << firstName << lastName << city << std::endl;
}

// Collects the same customer data group for delivery.
void deliverCustomer(std::string firstName, std::string lastName, std::string city) {
    std::cout << firstName << lastName << city << std::endl;
}

// Exercises the long-parameter-list detector.
int calculateDelivery(int itemCount, int unitPrice, int discount, int tax, int shipping) {
    return itemCount * unitPrice - discount + tax + shipping;
}

// Exercises redundant boolean comparison.
bool acceptsOrder(bool isAvailable) {
    return isAvailable == true;
}

// Exercises repeated statements inside one function.
int repeatDeliverySteps(int deliveryCount) {
    deliveryCount += 1;
    deliveryCount += 2;
    deliveryCount += 3;
    deliveryCount += 1;
    deliveryCount += 2;
    deliveryCount += 3;
    return deliveryCount;
}

// Exercises deeply nested conditional handling.
int inspectDelivery(int deliveryCount) {
    int result = deliveryCount;
    if (deliveryCount > 0) {
        if (deliveryCount > 1) {
            if (deliveryCount > 2) {
                if (deliveryCount > 3) {
                    result += 1;
                }
            }
        }
    }
    return result;
}

// Exercises the allocation checker.
int* allocateDeliveryCount() {
    int* deliveryCount = new int(1);
    return deliveryCount;
}

// This function is intentionally unreachable from main.
void unusedDeliveryHelper() {}

class DeliveryBase {
public:
    // Dispatches a delivery for the base interface.
    virtual void dispatch() {}
};

class UnusedDeliveryInheritance : public DeliveryBase {
public:
    // Prepares the delivery without using the base interface.
    void prepare() {}
};

// Runs the supported detector variety case.
int main() {
    prepareCustomer("A", "B", "C");
    printCustomer("A", "B", "C");
    deliverCustomer("A", "B", "C");
    std::cout << calculateDelivery(2, 10, 1, 2, 3) << std::endl;
    std::cout << acceptsOrder(true) << std::endl;
    std::cout << repeatDeliverySteps(0) << std::endl;
    std::cout << inspectDelivery(4) << std::endl;
    UnusedDeliveryInheritance delivery;
    delivery.prepare();
    return *allocateDeliveryCount();
}
