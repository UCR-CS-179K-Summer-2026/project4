#include <string>
#include <vector>
#include <iostream>

void address(std::string streetName, std::string city, std::string state, std::string zip) {
	std::cout << streetName << std::endl;
}

void getAddress(std::string streetName, std::string city, std::string state, std::string zip) {
	std::cout << streetName << std::endl;
}

int someFunction() {
	std::string streetName;
    std::string city;
    std::string state;
    std::string zip;
    
    address(streetName, city, state, zip);
    getAddress(streetName, city, state, zip);

	return 0;
}