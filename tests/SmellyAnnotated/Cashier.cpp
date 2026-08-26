/*
Copyright (c) 2025 Ahmed R. Sadik, Honda Research Institute Europe GmbH 

This source code is licensed under the MIT License found in the
LICENSE file in the root directory of this source tree. This dataset contains smelly code for research and refactoring purposes.
*/


#include "Cashier.h"

Cashier::Cashier(Chef* chef) : chef(chef), frequentCustomerDiscount(false) {}

void Cashier::takeOrder(const std::string& pizzaType) {
    std::cout << "Cashier is taking order for " << pizzaType << " pizza." << std::endl;
    chef->bakePizza(pizzaType);
}

void Cashier::hurryUpChef() {
    std::cout << "Cashier is hurrying up the chef." << std::endl;
    chef->hurryUp();
}

void Cashier::calmCustomerDown() {
    std::cout << "Cashier is calming the customer down." << std::endl;
}

void Cashier::deliverPizzaToCustomer() {
    std::cout << "Cashier is delivering pizza to the customer." << std::endl;
}

Chef* Cashier::getChef() {  // Implementation of getChef method
    return chef;
}

// Methods with code smells
void Cashier::notifyForPromotion() {
    std::cout << "Notifying customer for promotion" << std::endl;
}

void Cashier::notifyForDiscount() {
    std::cout << "Notifying customer for discount" << std::endl;
}

void Cashier::notifyForNewArrivals() {
    std::cout << "Notifying customer for new arrivals" << std::endl;
}

void Cashier::applyDiscount() {
    std::cout << "Applying discount for customer" << std::endl;
}

void Cashier::applyLoyaltyPoints() {
    std::cout << "Applying loyalty points for customer" << std::endl;
}

void Cashier::handleComplaint(const std::string& complaint) {
    if (complaint == "cold pizza") {
        std::cout << "Handling complaint: Pizza is cold" << std::endl;
    } else if (complaint == "late delivery") {
        std::cout << "Handling complaint: Pizza is late" << std::endl;
    } else if (complaint == "wrong order") {
        std::cout << "Handling complaint: Wrong pizza delivered" << std::endl;
    } else {
        std::cout << "Handling complaint: General complaint" << std::endl;
    }
}

void Cashier::askForReceipt() {
    std::cout << "Customer is asking for a receipt." << std::endl;
}

void Cashier::anotherUnusedMethod() {}

void Cashier::longMethod() {
    std::cout << "Cashier is handling many tasks in a single method" << std::endl;
    this->takeOrder("Cheese");
    this->notifyForPromotion();
    this->notifyForDiscount();
    this->notifyForNewArrivals();
    this->applyDiscount();
    this->applyLoyaltyPoints();
    this->handleComplaint("cold pizza");
}

void Cashier::orderWithUnnecessaryDetails(const std::string& pizzaType, const std::string& size, const std::string& crustType, const std::string& toppings, bool extraCheese, const std::string& discountCode) {
    std::cout << "Placing a detailed order for " << pizzaType << " pizza with " << size << ", " << crustType << ", " << toppings << ", extra cheese: " << extraCheese << ", discount code: " << discountCode << std::endl;
    this->takeOrder(pizzaType);
    this->applyDiscount();
    this->applyLoyaltyPoints();
}

void Cashier::duplicateMethod() {
    std::cout << "Cashier is making a duplicate order" << std::endl;
    this->takeOrder("Cheese");
    this->takeOrder("Cheese");
}

void Cashier::chainOfMethods() {
    std::cout << "Cashier is initiating a message chain" << std::endl;
    this->accessInternalDetails();
}

void Cashier::middlemanMethod() {
    std::cout << "Cashier is calling a middleman method" << std::endl;
    this->middleMethod();
}

void Cashier::middleMethod() {
    std::cout << "Middleman method delegating to another method" << std::endl;
    this->realMethod();
}

void Cashier::realMethod() {
    std::cout << "Real method doing the actual work" << std::endl;
}

void Cashier::accessInternalDetails() {
    // Inappropriate Intimacy
    std::cout << "Accessing internal details of Chef: " << chef->isBusy() << std::endl;
}

void Cashier::refusedBequest() {}
