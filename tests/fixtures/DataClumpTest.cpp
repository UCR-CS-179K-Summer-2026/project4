#include <string>
#include <vector>
#include <iostream>
/*  OCCURENCES OF CLUMPS FOUND IN THE CODE:
        Group 1: streetName, city, state (flags this group)
        Group 2: firstName, lastName, middleName (flags this group)
        Group 3: streetName, city, state, firstName, lastName, middleName (will not flag)
        Group 4: city, state, firstName (flags this group)
        Group 5: streetName, city, state, firstNumber, secondNumber (will not flag this group)
        Group 6: streetName, city, state, country (flags this group)
        Group 7: employeeId, firstName, lastName, middleName, streetName, city, state, zip (flags this group)
        Group 8: secondStreetName, secondCity, secondState (flags this group)
        Group 9: streetName, city, state, first, second (will not flag this group)
        Group 10: streetName, city, state, second, third (will not flag this group)
        Group 11: streetName, city, state, third, first (will not flag this group)

        Total flags: 6
*/

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

void getSomeInfo(std::string streetName, std::string city, std::string state, std::string country) {
    std::cout << streetName << std::endl;
    std::cout << city << ", " << state << " " << country << std::endl;
}

void dataClumpOfEight(int employeeId, std::string firstName, std::string lastName, std::string middleName, std::string streetName, std::string city, std::string state, int zip) {
    std::cout << employeeId << ": " << firstName << " " << middleName << " " << lastName << std::endl;
    std::cout << streetName << std::endl;
    std::cout << city << ", " << state << " " << zip << std::endl;
}

int someFunction() {
	std::string streetName;
    std::string city;
    std::string state;
    std::string country;
    int zip;
    int first;
    int second;
    int third;

    if(true) {
        int employeeId;
        std::string firstName;
        std::string lastName;
        std::string middleName;
        
        getName(firstName, lastName, middleName);
        dataClumpOfEight(employeeId, firstName, lastName, middleName, streetName, city, state, zip);
        dataClumpOfEight(employeeId, firstName, lastName, middleName, streetName, city, state, zip);
    }
    std::string secondStreetName;
    std::string secondCity;
    std::string secondState;
    std::string secondZip;

    address(secondStreetName, secondCity, secondState);

    getAddress(secondStreetName, secondCity, secondState);

    getAddress(secondStreetName, secondCity, secondState);

    getStreetNumber(streetName, city, state, first, second);
    getStreetNumber(streetName, city, state, second, third);
    getStreetNumber(streetName, city, state, third, first);

    getSomeInfo(streetName, city, state, country);
    getSomeInfo(streetName, city, state, country);

	return 0;
}