#include "inputUtil.h"

// Checks if a given num is between lower and upper values inclusive
char checkRange(char num, char lower, char upper){
	if(num < lower || num > upper)
		return 0;
	
	return 1;

}

// Verifies that the quantity given is between 0 and 59
// Used for input seconds and minutes
// Expects the buffer pointer to point to the beginning of the quantity
// Returns -1 if it fails, otherwise returns the quantity
char getSecMin(char * buffer){
	char quantity = (buffer[0] - '0')* 10 + (buffer[1] - '0');
	
	if (checkRange(quantity, 0, 59) == 0)
		return (char)-1;
	
	return quantity;
}


// Verifies that the hours given is between 0 and 23
// Expects the buffer pointer to point to the beginning of the hours
// Returns -1 if it fails, otherwise returns the hours
char getHour(char * buffer){
	char hours = (buffer[0] - '0')* 10 + (buffer[1] - '0');
	
	if (checkRange(hours, 0, 23) == 0)
		return (char)-1;
	
	return hours;
}