#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "libjson/json.h"

void test_json_string_escape()
{
    struct json *str = json_string("test\none\ttwo");
    char buf[1024];
    FILE *out = fmemopen(buf, sizeof(buf), "w");
    int ret = json_fwrite(str, out);
    fclose(out);

    assert(ret > 0);
    assert(strcmp(buf, "\"test\\none\\ttwo\"") == 0);

    const char *escaped_str = json_tostring(str);

    assert(escaped_str != NULL);
    assert(strcmp(escaped_str, "test\none\ttwo") == 0);

    json_free(str);
}

int main()
{
    test_json_string_escape();
    printf("All object tests passed!\n");
    return 0;
}
