#include "clean_control_2.h"

// Applies a discount using four or fewer values.
int applyDiscount(int subtotal, int discount, int tax, int shipping) {
	return subtotal - discount + tax + shipping;
}