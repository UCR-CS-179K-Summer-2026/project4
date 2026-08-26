/*
Copyright (c) 2025 Ahmed R. Sadik, Honda Research Institute Europe GmbH 

This source code is licensed under the MIT License found in the
LICENSE file in the root directory of this source tree. This dataset contains smelly code for research and refactoring purposes.
*/


#include "Pizza.h"

Pizza::Pizza(const std::string& size, const std::string& doughType, const std::string& topping)
    : size(size), doughType(doughType), topping(topping), extraCheese(false) {}

std::string Pizza::getSize() const {
    return size;
}

std::string Pizza::getDoughType() const {
    return doughType;
}

std::string Pizza::getTopping() const {
    return topping;
}

void Pizza::setSize(const std::string& size) {
    this->size = size;
}

void Pizza::setDoughType(const std::string& doughType) {
    this->doughType = doughType;
}

void Pizza::setTopping(const std::string& topping) {
    this->topping = topping;
}

void Pizza::updateCustomerInfo(const std::string& firstName, const std::string& lastName, const std::string& address, const std::string& phoneNumber, const std::string& email) {
    // Shotgun Surgery
    this->customerFirstName = firstName;
    this->customerLastName = lastName;
    this->customerAddress = address;
    this->customerPhoneNumber = phoneNumber;
    this->customerEmail = email;
}

void Pizza::updateCustomerName(const std::string& firstName, const std::string& lastName) {
    // Shotgun Surgery
    this->customerFirstName = firstName;
    this->customerLastName = lastName;
}

void Pizza::updateCustomerAddress(const std::string& address) {
    // Shotgun Surgery
    this->customerAddress = address;
}

void Pizza::updateCustomerPhoneNumber(const std::string& phoneNumber) {
    // Shotgun Surgery
    this->customerPhoneNumber = phoneNumber;
}

void Pizza::updateCustomerEmail(const std::string& email) {
    // Shotgun Surgery
    this->customerEmail = email;
}

void Pizza::notifyForPromotion() {
    // Divergent Change
    std::cout << "Notifying customer for promotion" << std::endl;
}

void Pizza::notifyForDiscount() {
    // Divergent Change
    std::cout << "Notifying customer for discount" << std::endl;
}

void Pizza::notifyForNewArrivals() {
    // Divergent Change
    std::cout << "Notifying customer for new arrivals" << std::endl;
}

void Pizza::applyDiscount() {
    // Parallel Inheritance Hierarchies
    std::cout << "Applying discount for customer" << std::endl;
}

void Pizza::applyLoyaltyPoints() {
    // Parallel Inheritance Hierarchies
    std::cout << "Applying loyalty points for customer" << std::endl;
}

void Pizza::handleComplaint(const std::string& complaint) {
    // Switch Statements
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

void Pizza::askForReceipt() {
    // Speculative Generality
    std::cout << "Customer is asking for a receipt." << std::endl;
}

void Pizza::anotherUnusedMethod() {
    // Dead Code
}

void Pizza::longMethod() {
    // Long Method
    std::cout << "Pizza is handling many tasks in a single method" << std::endl;
    this->setSize("Large");
    this->setDoughType("Thin Crust");
    this->setTopping("Pepperoni");
    this->updateCustomerInfo("John", "Doe", "123 Street", "555-5555", "john.doe@example.com");
    this->notifyForPromotion();
    this->notifyForDiscount();
    this->notifyForNewArrivals();
    this->applyDiscount();
    this->applyLoyaltyPoints();
    this->handleComplaint("cold pizza");
}

void Pizza::orderWithUnnecessaryDetails(const std::string& pizzaType, const std::string& size, const std::string& crustType, const std::string& toppings, bool extraCheese, const std::string& discountCode) {
    // Long Parameter List
    std::cout << "Placing a detailed order for " << pizzaType << " pizza with " << size << ", " << crustType << ", " << toppings << ", extra cheese: " << extraCheese << ", discount code: " << discountCode << std::endl;
    this->setSize(size);
    this->setDoughType(crustType);
    this->setTopping(toppings);
    this->applyDiscount();
}

void Pizza::duplicateMethod() {
    // Duplicate Code
    std::cout << "Customer is making a duplicate order" << std::endl;
    this->setTopping("Cheese");
    this->setTopping("Cheese");
}

void Pizza::chainOfMethods() {
    // Message Chain
    std::cout << "Pizza is initiating a message chain" << std::endl;
    this->updateCustomerAddress("123 Street");
}

void Pizza::middlemanMethod() {
    // Middle Man
    std::cout << "Pizza is calling a middleman method" << std::endl;
    this->middleMethod();
}

void Pizza::middleMethod() {
    std::cout << "Middleman method delegating to another method" << std::endl;
    this->realMethod();
}

void Pizza::realMethod() {
    std::cout << "Real method doing the actual work" << std::endl;
}

void Pizza::accessInternalDetails() {
    // Inappropriate Intimacy
    std::cout << "Accessing internal details: " << this->size << ", " << this->doughType << ", " << this->topping << std::endl;
}

void Pizza::refusedBequest() {
    // Refused Bequest
}

CheesePizza::CheesePizza()
    : Pizza("Medium", "Regular", "Cheese") {}

VeggiePizza::VeggiePizza()
    : Pizza("Medium", "Whole Wheat", "Veggies") {}

TunaPizza::TunaPizza()
    : Pizza("Large", "Thin Crust", "Tuna") {}

PepperoniPizza::PepperoniPizza()
    : Pizza("Large", "Regular", "Pepperoni") {}
