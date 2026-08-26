/*
Copyright (c) 2025 Ahmed R. Sadik, Honda Research Institute Europe GmbH 

This source code is licensed under the MIT License found in the
LICENSE file in the root directory of this source tree. This dataset contains smelly code for research and refactoring purposes.
*/


#include "Drink.h"

Drink::Drink() {
    drinkType = "Water";
    size = "Medium";
    ice = false;
    lemon = false;
}

void Drink::orderDrink(const std::string& drinkType) {
    std::cout << "Ordering a " << drinkType << std::endl;
    this->drinkType = drinkType;
    prepareDrink();
}

void Drink::prepareDrink() {
    std::cout << "Preparing " << drinkType << std::endl;
}

void Drink::serveDrink() {
    std::cout << "Serving " << drinkType << std::endl;
}

void Drink::cleanup() {
    std::cout << "Cleaning up after preparing " << drinkType << std::endl;
}

// Code smells methods
void Drink::notifyForPromotion() {
    // Divergent Change
    std::cout << "Notifying customer for promotion" << std::endl;
}

void Drink::notifyForDiscount() {
    // Divergent Change
    std::cout << "Notifying customer for discount" << std::endl;
}

void Drink::notifyForNewArrivals() {
    // Divergent Change
    std::cout << "Notifying customer for new arrivals" << std::endl;
}

void Drink::applyDiscount() {
    // Parallel Inheritance Hierarchies
    std::cout << "Applying discount for customer" << std::endl;
}

void Drink::applyLoyaltyPoints() {
    // Parallel Inheritance Hierarchies
    std::cout << "Applying loyalty points for customer" << std::endl;
}

void Drink::handleComplaint(const std::string& complaint) {
    // Switch Statements
    if (complaint == "no ice") {
        std::cout << "Handling complaint: No ice" << std::endl;
    } else if (complaint == "too much ice") {
        std::cout << "Handling complaint: Too much ice" << std::endl;
    } else if (complaint == "wrong drink") {
        std::cout << "Handling complaint: Wrong drink delivered" << std::endl;
    } else {
        std::cout << "Handling complaint: General complaint" << std::endl;
    }
}

void Drink::askForReceipt() {
    // Speculative Generality
    std::cout << "Customer is asking for a receipt." << std::endl;
}

void Drink::anotherUnusedMethod() {
    // Dead Code
}

void Drink::longMethod() {
    // Long Method
    std::cout << "Drink is handling many tasks in a single method" << std::endl;
    this->orderDrink("Lemonade");
    this->prepareDrink();
    this->serveDrink();
    this->cleanup();
    this->notifyForPromotion();
    this->notifyForDiscount();
    this->notifyForNewArrivals();
    this->applyDiscount();
    this->applyLoyaltyPoints();
    this->handleComplaint("no ice");
}

void Drink::orderWithUnnecessaryDetails(const std::string& drinkType, const std::string& size, bool ice, bool lemon, const std::string& discountCode) {
    // Long Parameter List
    std::cout << "Placing a detailed order for " << drinkType << " with " << size << ", ice: " << ice << ", lemon: " << lemon << ", discount code: " << discountCode << std::endl;
    this->orderDrink(drinkType);
}

void Drink::duplicateMethod() {
    // Duplicate Code
    std::cout << "Customer is making a duplicate order" << std::endl;
    this->orderDrink("Water");
    this->orderDrink("Water");
}

void Drink::chainOfMethods() {
    // Message Chain
    std::cout << "Drink is initiating a message chain" << std::endl;
    this->cleanup();
}

void Drink::middlemanMethod() {
    // Middle Man
    std::cout << "Drink is calling a middleman method" << std::endl;
    this->middleMethod();
}

void Drink::middleMethod() {
    std::cout << "Middleman method delegating to another method" << std::endl;
    this->realMethod();
}

void Drink::realMethod() {
    std::cout << "Real method doing the actual work" << std::endl;
}

void Drink::accessInternalDetails() {
    // Inappropriate Intimacy
    std::cout << "Accessing internal details of Drink: " << this->size << ", " << this->drinkType << std::endl;
}

void Drink::refusedBequest() {
    // Refused Bequest
}
