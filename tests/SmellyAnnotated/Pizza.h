/*
Copyright (c) 2025 Ahmed R. Sadik, Honda Research Institute Europe GmbH 

This source code is licensed under the MIT License found in the
LICENSE file in the root directory of this source tree. This dataset contains smelly code for research and refactoring purposes.
*/


#ifndef PIZZA_H
#define PIZZA_H

#include <string>
#include <iostream>

class Pizza {
public:
    Pizza(const std::string& size, const std::string& doughType, const std::string& topping);

    std::string getSize() const;
    std::string getDoughType() const;
    std::string getTopping() const;

    void setSize(const std::string& size);
    void setDoughType(const std::string& doughType);
    void setTopping(const std::string& topping);

    void updateCustomerInfo(const std::string& firstName, const std::string& lastName, const std::string& address, const std::string& phoneNumber, const std::string& email);
    void updateCustomerName(const std::string& firstName, const std::string& lastName);
    void updateCustomerAddress(const std::string& address);
    void updateCustomerPhoneNumber(const std::string& phoneNumber);
    void updateCustomerEmail(const std::string& email);

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
    std::string size;
    std::string doughType;
    std::string topping;

    // Primitive Obsession
    bool extraCheese;

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

class CheesePizza : public Pizza {
public:
    CheesePizza();
};

class VeggiePizza : public Pizza {
public:
    VeggiePizza();
};

class TunaPizza : public Pizza {
public:
    TunaPizza();
};

class PepperoniPizza : public Pizza {
public:
    PepperoniPizza();
};

#endif // PIZZA_H
