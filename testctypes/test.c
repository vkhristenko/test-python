#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct Point {
    int x;
    int y;
};

struct Line {
    struct Point start;
    struct Point end;
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

void show_line(struct Line line) {
    show_point(line.start);
    show_point_byref(&line.start);
    show_point(line.end);
    show_point_byref(&line.end);
}

void show_line_byref(struct Line *line) {
    show_point(line->start);
    show_point_byref(&(line->start));
    show_point(line->end);
    show_point_byref(&(line->end));
}

struct FileContext {
    FILE *file;
};

struct FileContext open_context_toread() {
    printf("open a file to read\n");
    struct FileContext ctx;
    ctx.file = fopen("test1.bin", "rb");
    return ctx;
}

struct FileContext open_context_towrite() {
    printf("open a file to write\n");
    struct FileContext ctx;
    ctx.file = fopen("test1.bin", "wb");
    return ctx;
}

void write_buffer(struct FileContext ctx, char *buffer, int size) {
    fwrite(buffer, size, 1, ctx.file);
}

void read_buffer(struct FileContext ctx, char *buffer, int size) {
    fread(buffer, size, 1, ctx.file);
}

void close_context(struct FileContext ctx) {
    if (ctx.file == NULL)
        printf("file pointer is null\n");
    else 
        printf("closing an opened file\n");
    fclose(ctx.file);
}
