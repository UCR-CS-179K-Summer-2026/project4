/*
Copyright (c) 2025 Ahmed R. Sadik, Honda Research Institute Europe GmbH 

This source code is licensed under the MIT License found in the
LICENSE file in the root directory of this source tree. This dataset contains smelly code for research and refactoring purposes.
*/


#ifndef CHEF_H
#define CHEF_H

#include "Pizza.h"
#include <string>

class Chef {
public:
    Chef();
    void bakePizza(const std::string& pizzaType);
    void cutPizzaAndPutInBox();
    void deliverPizza();
    void hurryUp();
    void cleanKitchen();
    bool isBusy() const;

    // Methods with code smells
    void updateContactInfo(const std::string& firstName, const std::string& lastName, const std::string& address, const std::string& phoneNumber, const std::string& email);
    void notifyForPromotion();
    void notifyForDiscount();
    void notifyForNewArrivals();
    void applyDiscount();
    void applyLoyaltyPoints();
    void handleComplaint(const std::string& complaint);
    void askForReceipt();
    void anotherUnusedMethod();
    void longMethod();
    void orderWithUnnecessaryDetails();
    void duplicateMethod();
    void chainOfMethods();
    void middlemanMethod();
    void middleMethod();
    void realMethod();
    void accessInternalDetails();
    void refusedBequest();

private:
    Pizza* pizza;  // Change to pointer
    bool busy;
    int completedOrders;
    int breaksTaken;
    bool kitchenClean;

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

#endif // CHEF_H
