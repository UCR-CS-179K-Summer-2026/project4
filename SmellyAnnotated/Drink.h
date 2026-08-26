/*
Copyright (c) 2025 Ahmed R. Sadik, Honda Research Institute Europe GmbH 

This source code is licensed under the MIT License found in the
LICENSE file in the root directory of this source tree. This dataset contains smelly code for research and refactoring purposes.
*/


#ifndef DRINK_H
#define DRINK_H

#include <string>
#include <iostream>

class Drink {
public:
    Drink();
    void orderDrink(const std::string& drinkType);
    void prepareDrink();
    void serveDrink();
    void cleanup();

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
    void orderWithUnnecessaryDetails(const std::string& drinkType, const std::string& size, bool ice, bool lemon, const std::string& discountCode);
    void duplicateMethod();
    void chainOfMethods();
    void middlemanMethod();
    void middleMethod();
    void realMethod();
    void accessInternalDetails();
    void refusedBequest();

private:
    std::string drinkType;
    std::string size;

    // Primitive Obsession
    bool ice;
    bool lemon;

    // Data Clumps
    std::string customerFirstName;
    std::string customerLastName;
    std::string customerAddress;
    std::string customerPhoneNumber;
    std::string customerEmail;

    // Temporary Fields
    std::string tempDiscountCode;
    std::string tempOrderNote;
};

#endif // DRINK_H
