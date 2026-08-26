/*
Copyright (c) 2025 Ahmed R. Sadik, Honda Research Institute Europe GmbH 

This source code is licensed under the MIT License found in the
LICENSE file in the root directory of this source tree. This dataset contains smelly code for research and refactoring purposes.
*/


#include "Customer.h"

Customer::Customer(Shop* pizzaShop)
    : pizzaShop(pizzaShop), drinkOrder(new Drink()), frequentCustomerDiscount(false) {}

void Customer::orderPizza(const std::string& pizzaType) {
    std::cout << "Customer is placing an order for " << pizzaType << " pizza." << std::endl;
    pizzaShop->receiveOrder(pizzaType);
}

void Customer::complain(const std::string& complaint) {
    std::cout << "Customer is complaining: " << complaint << std::endl;
    pizzaShop->getCashier()->calmCustomerDown();
}

// Code smells methods
void Customer::notifyForPromotion() {
    std::cout << "Notifying customer for promotion" << std::endl;
}

void Customer::notifyForDiscount() {
    std::cout << "Notifying customer for discount" << std::endl;
}

void Customer::notifyForNewArrivals() {
    std::cout << "Notifying customer for new arrivals" << std::endl;
}

void Customer::applyDiscount() {
    std::cout << "Applying discount for customer" << std::endl;
}

void Customer::applyLoyaltyPoints() {
    std::cout << "Applying loyalty points for customer" << std::endl;
}

void Customer::handleComplaint(const std::string& complaint) {
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

void Customer::askForReceipt() {
    std::cout << "Customer is asking for a receipt." << std::endl;
}

void Customer::anotherUnusedMethod() {}

void Customer::longMethod() {
    std::cout << "Customer is handling many tasks in a single method" << std::endl;
    this->orderPizza("Cheese");
    this->complain("Pizza is cold");
    this->notifyForPromotion();
    this->notifyForDiscount();
    this->notifyForNewArrivals();
    this->applyDiscount();
    this->applyLoyaltyPoints();
    this->handleComplaint("cold pizza");
}

void Customer::orderWithUnnecessaryDetails(const std::string& pizzaType, const std::string& size, const std::string& crustType, const std::string& toppings, bool extraCheese, const std::string& discountCode) {
    std::cout << "Placing a detailed order for " << pizzaType << " pizza with " << size << ", " << crustType << ", " << toppings << ", extra cheese: " << extraCheese << ", discount code: " << discountCode << std::endl;
    this->orderPizza(pizzaType);
    this->applyDiscount();
    this->applyLoyaltyPoints();
}

void Customer::duplicateMethod() {
    std::cout << "Customer is making a duplicate order" << std::endl;
    this->orderPizza("Cheese");
    this->orderPizza("Cheese");
}

void Customer::chainOfMethods() {
    std::cout << "Customer is initiating a message chain" << std::endl;
    this->accessInternalDetails();
}

void Customer::middlemanMethod() {
    std::cout << "Customer is calling a middleman method" << std::endl;
    this->middleMethod();
}

void Customer::middleMethod() {
    std::cout << "Middleman method delegating to another method" << std::endl;
    this->realMethod();
}

void Customer::realMethod() {
    std::cout << "Real method doing the actual work" << std::endl;
}

void Customer::accessInternalDetails() {
    // Inappropriate Intimacy
    pizzaShop->getCashier()->hurryUpChef();
    std::cout << "Accessing internal details of PizzaShop" << std::endl;
}

void Customer::refusedBequest() {}
