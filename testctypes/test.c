#include <string.h>
#include <stdio.h>

struct Point {
    int x;
    int y;
};

void show_point(struct Point point) {
    printf("x: %d y: %d\n", point.x, point.y);
}

void show_point_byref(struct Point *point) {
    printf("x: %d y: %d\n", point->x, point->y);
}

int add_int(int a, int b) {
    return a+b;
}

double add_double(double a, double b) {
    return a+b;
}

float add_float(float a, float b) {
    return a+b;
}

char* my_strconcat(char *dest, char* src) {
    return strcat(dest, src);
}
