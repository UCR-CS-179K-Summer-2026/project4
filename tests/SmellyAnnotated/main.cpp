/*
Copyright (c) 2025 Ahmed R. Sadik, Honda Research Institute Europe GmbH 

This source code is licensed under the MIT License found in the
LICENSE file in the root directory of this source tree. This dataset contains smelly code for research and refactoring purposes.
*/


#include "Shop.h"
#include "Customer.h"
#include <iostream>
#include <chrono>

void measureExecutionTime(int runs = 10) {
    double totalTime = 0;
    for (int i = 0; i < runs; ++i) {
        auto start = std::chrono::high_resolution_clock::now();

        Shop shop;
        Customer customer(&shop);
        customer.orderPizza("Cheese");
        customer.complain("The pizza is too cold.");

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        totalTime += duration.count();
    }

    double averageTime = totalTime / runs;
    std::cout << "Average execution time over multiple runs: " << averageTime << " seconds" << std::endl;
}

int main() {
    measureExecutionTime();
    return 0;
}
