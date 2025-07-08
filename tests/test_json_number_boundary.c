#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char errbuf[1024];

    // Test boundary number values
    struct json *zero = json_number(0.0);
    assert(json_is_number(zero));
    assert(json_double_value(zero) == 0.0);
    assert(json_int_value(zero) == 0);
    json_free(zero);

    struct json *negative_zero = json_number(-0.0);
    assert(json_is_number(negative_zero));
    assert(json_double_value(negative_zero) == -0.0);
    json_free(negative_zero);

    // Test very large numbers
    struct json *large = json_number(1e100);
    assert(json_is_number(large));
    assert(json_double_value(large) == 1e100);
    json_free(large);

    struct json *small = json_number(1e-100);
    assert(json_is_number(small));
    assert(json_double_value(small) == 1e-100);
    json_free(small);

    // Test integer boundary values
    struct json *max_int = json_number(2147483647); // INT_MAX
    assert(json_is_number(max_int));
    assert(json_int_value(max_int) == 2147483647);
    json_free(max_int);

    struct json *min_int = json_number(-2147483648); // INT_MIN
    assert(json_is_number(min_int));
    assert(json_int_value(min_int) == -2147483648);
    json_free(min_int);

    // Test parsing boundary cases
    struct json *parsed_large = json_read_string("1.7976931348623157e+308", errbuf);
    assert(parsed_large != NULL);
    assert(json_is_number(parsed_large));
    json_free(parsed_large);

    // Test very small positive number
    struct json *parsed_small = json_read_string("2.2250738585072014e-308", errbuf);
    assert(parsed_small != NULL);
    assert(json_is_number(parsed_small));
    json_free(parsed_small);

    // Test scientific notation
    struct json *scientific = json_read_string("1.23e10", errbuf);
    assert(scientific != NULL);
    assert(json_is_number(scientific));
    assert(json_double_value(scientific) == 1.23e10);
    json_free(scientific);

    struct json *scientific_neg = json_read_string("-4.56E-5", errbuf);
    assert(scientific_neg != NULL);
    assert(json_is_number(scientific_neg));
    assert(json_double_value(scientific_neg) == -4.56E-5);
    json_free(scientific_neg);

    // Test precision limits
    struct json *precise = json_read_string("0.123456789012345", errbuf);
    assert(precise != NULL);
    assert(json_is_number(precise));
    json_free(precise);

    // Test integer vs double distinction
    struct json *whole = json_number(42.0);
    assert(json_is_number(whole));
    assert(json_double_value(whole) == 42.0);
    assert(json_int_value(whole) == 42);
    json_free(whole);

    struct json *fraction = json_number(42.5);
    assert(json_is_number(fraction));
    assert(json_double_value(fraction) == 42.5);
    assert(json_int_value(fraction) == 42); // Truncation expected
    json_free(fraction);

    // Test round-trip through JSON string
    struct json *original = json_number(3.141592653589793);
    FILE *temp = tmpfile();
    assert(temp != NULL);
    json_write(original, temp);
    rewind(temp);
    
    struct json *roundtrip = json_read(temp, errbuf);
    fclose(temp);
    
    assert(roundtrip != NULL);
    assert(json_is_number(roundtrip));
    // May have some precision loss due to string conversion
    double diff = json_double_value(original) - json_double_value(roundtrip);
    assert(diff < 1e-10 && diff > -1e-10); // Allow small precision error
    
    json_free(original);
    json_free(roundtrip);

    return 0;
}
