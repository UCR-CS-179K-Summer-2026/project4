#include "LongParameterListTest.h"

// Calculates an order total from all required checkout values.
int calculateOrderTotal(int itemCount, int unitPrice, int discount, int tax, int shipping) {
	return itemCount * unitPrice - discount + tax + shipping;
}

// Runs the positive long-parameter-list case.
int main() {
	return calculateOrderTotal(2, 10, 1, 2, 3);
}