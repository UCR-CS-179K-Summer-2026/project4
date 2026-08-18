#include <string>
#include <vector>
#include <iostream>

// Holds duplicates of the same data: streetName, city, state, zip. Also has a declaration
// of firstName, lastName, middleName in a nested if statement.
void address(std::string streetName, std::string city, std::string state, std::string zip) {
	std::cout << streetName << std::endl;
    if(true) {
        std::string firstName;
        std::string lastName;
        std::string middleName;
        
        getName(firstName, lastName, middleName);
    }
}

void getPersonAddress(std::string streetName, std::string city, std::string state, std::string zip, std::string firstName, std::string lastName, std::string middleName) {
    std::cout << streetName << std::endl;
    std::cout << firstName << " " << middleName << " " << lastName << std::endl;
    
}

// Holds duplicates of the same data: streetName, city, state, zip. Also has a declaration
// of firstName, lastName, middleName in a nested if statement.
void getAddress(std::string streetName, std::string city, std::string state, std::string zip) {
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

// Holds duplicates of the same data: streetName, city, state, zip. Also has a declaration
// of firstName, lastName, middleName in a nested if statement.
int someFunction() {
	std::string streetName;
    std::string city;
    std::string state;
    std::string zip;
    std::string country;

    if(true) {
        std::string firstName;
        std::string lastName;
        std::string middleName;
        
        getName(firstName, lastName, middleName);
        getPersonAddress(streetName, city, state, zip, firstName, lastName, middleName);
    }
    std::string secondStreetName;

    std::string secondCity;

    address(streetName, city, state, zip);
    
    std::string secondState;
    
    getAddress(streetName, city, state, zip);
    
    std::string secondZip;

    address(secondStreetName, secondCity, secondState, secondZip);

    getAddress(secondStreetName, secondCity, secondState, secondZip);

    getAddress(secondStreetName, secondCity, secondState, secondZip);

	return 0;
}