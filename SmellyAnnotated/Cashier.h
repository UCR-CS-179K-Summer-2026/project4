/*
Copyright (c) 2025 Ahmed R. Sadik, Honda Research Institute Europe GmbH 

This source code is licensed under the MIT License found in the
LICENSE file in the root directory of this source tree. This dataset contains smelly code for research and refactoring purposes.
*/


#ifndef CASHIER_H
#define CASHIER_H

#include "Chef.h"
#include <string>
#include <iostream>

class Cashier {
public:
    Cashier(Chef* chef);
    void takeOrder(const std::string& pizzaType);
    void hurryUpChef();
    void calmCustomerDown();
    void deliverPizzaToCustomer();
    Chef* getChef();  // Add this method

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
    void accessInternalDetails();
    void refusedBequest();

private:
    Chef* chef;
    bool frequentCustomerDiscount;
    std::string firstName;
    std::string lastName;
    std::string address;
    std::string phoneNumber;
    std::string email;
    std::string tempDiscountCode;
    std::string tempOrderNote;
};

#endif // CASHIER_H
