/*
Copyright (c) 2025 Ahmed R. Sadik, Honda Research Institute Europe GmbH 

This source code is licensed under the MIT License found in the
LICENSE file in the root directory of this source tree. This dataset contains smelly code for research and refactoring purposes.
*/


#ifndef SHOP_H
#define SHOP_H

#include "Cashier.h"
#include "Chef.h"
#include "Pizza.h"
#include <string>

class Shop {
public:
    Shop();
    void receiveOrder(const std::string& pizzaType);
    Pizza* createPizza(const std::string& pizzaType);
    Cashier* getCashier();
    Chef* getChef();
    void accessInternalDetails(); // only one declaration for accessInternalDetails

    // Methods with code smells
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
    void refusedBequest();

private:
    Cashier* cashier;
    Chef* chef;
    Pizza* pizza;

    // Primitive Obsession
    bool extraCheese;

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

#endif // SHOP_H
