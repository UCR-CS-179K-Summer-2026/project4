/*
Copyright (c) 2025 Ahmed R. Sadik, Honda Research Institute Europe GmbH 

This source code is licensed under the MIT License found in the
LICENSE file in the root directory of this source tree. This dataset contains smelly code for research and refactoring purposes.
*/


#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "Shop.h"
#include "Drink.h"
#include <string>
#include <iostream>

class Customer {
public:
    Customer(Shop* pizzaShop);
    void orderPizza(const std::string& pizzaType);
    void complain(const std::string& complaint);

    // Code smells methods
    void notifyForPromotion();
    void notifyForDiscount();
    void notifyForNewArrivals();
    void applyDiscount();
    void applyLoyaltyPoints();
    void handleComplaint(const std::string& complaint);
    void askForReceipt();
    void anotherUnusedMethod();
    void longMethod();
    void orderWithUnnecessaryDetails(const std::string& pizzaType, const std::string& size, const std::string& crustType, const std::string& toppings, bool extraCheese, const std::string& discountCode);
    void duplicateMethod();
    void chainOfMethods();
    void middlemanMethod();
    void middleMethod();
    void realMethod();
    void accessInternalDetails();
    void refusedBequest();

private:
    Shop* pizzaShop;
    Drink* drinkOrder;

    // Primitive Obsession
    bool frequentCustomerDiscount;

    // Data Clumps
    std::string firstName;
    std::string lastName;
    std::string address;
    std::string phoneNumber;
    std::string email;

    // Temporary Fields
    std::string tempDiscountCode;
    std::string tempOrderNote;
};

#endif // CUSTOMER_H
