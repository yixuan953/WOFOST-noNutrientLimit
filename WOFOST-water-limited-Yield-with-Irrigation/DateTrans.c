#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

char* DekadDate(float sowing_float){
    
    int sowing_dekad = (int)sowing_float;

    // Validate input
    if (sowing_dekad < 1 || sowing_dekad > 36) {
        fprintf(stderr, "Error: Dekad must be between 1 and 36\n");
        return NULL;
    }

    const int days_in_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Calculate month (1-12)
    int month = ((sowing_dekad - 1) / 3) + 1;
    
    // Calculate dekad within the month (1-3)
    int dekad_in_month = ((sowing_dekad - 1) % 3) + 1;
    
    // Calculate day (middle of the dekad)
    int day = (dekad_in_month - 1) * 10 + 5;
    
    // Adjust for month-end
    if (day > days_in_month[month - 1]) {
        day = days_in_month[month - 1];
    }

    // Allocate memory for date string
    char *date_str = (char*)malloc(20 * sizeof(char));
    memset(date_str, 0, 20);

    // Formate date_str
    snprintf(date_str, 20, "%02d-%02d", month, day);
    return date_str;
}