/*
Copyright (c) 2025 Ahmed R. Sadik, Honda Research Institute Europe GmbH 

This source code is licensed under the MIT License found in the
LICENSE file in the root directory of this source tree. This dataset contains smelly code for research and refactoring purposes.
*/


#include "Shop.h"

Shop::Shop() {
    cashier = new Cashier(new Chef());
    chef = cashier->getChef();
}

void Shop::receiveOrder(const std::string& pizzaType) {
    std::cout << "Shop received order for " << pizzaType << " pizza." << std::endl;
    pizza = createPizza(pizzaType);
    cashier->takeOrder(pizzaType);
}

Pizza* Shop::createPizza(const std::string& pizzaType) {
    if (pizzaType == "Cheese") {
        return new Pizza("Medium", "Regular", "Cheese");
    } else if (pizzaType == "Veggie") {
        return new Pizza("Medium", "Whole Wheat", "Veggies");
    } else if (pizzaType == "Tuna") {
        return new Pizza("Large", "Thin Crust", "Tuna");
    } else if (pizzaType == "Pepperoni") {
        return new Pizza("Large", "Regular", "Pepperoni");
    } else {
        throw std::invalid_argument("Unknown pizza type");
    }
}

Cashier* Shop::getCashier() {
    return cashier;
}

Chef* Shop::getChef() {
    return chef;
}

void Shop::accessInternalDetails() {
    // Inappropriate Intimacy
    std::cout << "Accessing internal details of Shop: " << cashier->getChef()->isBusy() << std::endl;
}

// Implementations of other methods with code smells here...
