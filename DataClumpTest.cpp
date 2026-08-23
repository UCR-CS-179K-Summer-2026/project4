#include <string>
#include <vector>
#include <iostream>

// Holds duplicates of the same data: streetName, city, state, zip. Also has a declaration
// of firstName, lastName, middleName in a nested if statement.
void address(std::string streetName, std::string city, std::string state) {
	std::cout << streetName << std::endl;
    if(true) {
        std::string firstName;
        std::string lastName;
        std::string middleName;
        
        getName(firstName, lastName, middleName);
    }
}

void getPersonAddress(std::string streetName, std::string city, std::string state, std::string firstName, std::string lastName, std::string middleName) {
    std::cout << streetName << std::endl;
    std::cout << firstName << " " << middleName << " " << lastName << std::endl;
    std::cout << city << ", " << state << " " << firstName << std::endl;
    std::cout << city << ", " << state << " " << firstName << std::endl;
    std::cout << city << ", " << state << " " << firstName << std::endl;
}

// Holds duplicates of the same data: streetName, city, state, zip. Also has a declaration
// of firstName, lastName, middleName in a nested if statement.
void getAddress(std::string streetName, std::string city, std::string state) {
	std::cout << streetName << std::endl;
    std::string firstName;
    std::string lastName;
    int age;
    std::string middleName;


    getName(firstName, lastName, middleName);
}

void getName(std::string firstName, std::string lastName, std::string middleName) {
    std::cout << firstName << " " << middleName << " " << lastName << std::endl;
}

void getStreetNumber(std::string streetName, std::string city, std::string state, int firstNumber, int secondNumber) {
    std::cout << streetName << std::endl;
    std::cout << city << ", " << state << std::endl;
}

// Holds duplicates of the same data: streetName, city, state, zip. Also has a declaration
// of firstName, lastName, middleName in a nested if statement.
int someFunction() {
	std::string streetName;
    std::string city;
    std::string state;
    std::string country;
    int first;
    int second;
    int third;

    if(true) {
        std::string firstName;
        std::string lastName;
        std::string middleName;
        
        getName(firstName, lastName, middleName);
        // getPersonAddress(streetName, city, state, firstName, lastName, middleName);
    }
    std::string secondStreetName;

    std::string secondCity;

    // address(streetName, city, state);
    
    std::string secondState;
    
    // getAddress(streetName, city, state);
    
    std::string secondZip;

    address(secondStreetName, secondCity, secondState);

    getAddress(secondStreetName, secondCity, secondState);

    getAddress(secondStreetName, secondCity, secondState);

    getStreetNumber(streetName, city, state, first, second);
    getStreetNumber(streetName, city, state, second, third);
    getStreetNumber(streetName, city, state, third, first);

	return 0;
}