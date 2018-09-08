#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void my_print(int x) {
    printf("value = %d\n", x);
}

void print_ints(int *ints, int n) {
    for (int i=0; i<n; i++)
        my_print(ints[i]);
}

void modify_and_print_ints(int *ints, int n) {
    for (int i=0; i<n; i++) {
        ints[i] = i+10;
        my_print(ints[i]);
    }
}

struct point_t {
    int x;
    int y;
};

void show_point_by_value(struct point_t p) 
{
    printf("x: %d y: %d\n", p.x, p.y);
}

void show_point_by_ref(struct point_t *p) 
{
    printf("x: %d y: %d\n", p->x, p->y);
}

void print_points(struct point_t *ps, int n) {
    for (int i=0; i<n; i++)
        show_point_by_ref(&(ps[i]));
}

struct point_t move_point(struct point_t p, int x, int y) {
    struct point_t res;
    res.x = p.x + x;
    res.y = p.y + y;

    return res;
}

void move_point_inplace(struct point_t *p, int x, int y) {
    p->x += x;
    p->y += y;
}

struct line_t {
    struct point_t start;
    struct point_t end;
};

void move_line_along_x_by_value(struct line_t l, int delta) {
    l.start.x += delta;
    l.end.x += delta;
}

void move_line_along_x_by_ref(struct line_t *l, int delta) {
    l->start.x += delta;
    l->end.x += delta;
}

struct line_t move_line_along_x(struct line_t l, int delta) {
    struct line_t res;
    res.start.x = l.start.x + delta;
    res.end.x = l.end.x + delta;

    res.start.y = l.start.y;
    res.end.y = l.end.y;

    return res;
}

void show_line_by_value(struct line_t l) 
{
    show_point_by_value(l.start);
    show_point_by_value(l.end);
    show_point_by_ref(&l.start);
    show_point_by_ref(&l.end);
}

void show_line_by_ref(struct line_t *l) 
{
    show_point_by_value(l->start);
    show_point_by_value(l->end);
    show_point_by_ref(&l->start);
    show_point_by_ref(&l->end);
}

struct rectangle_t {
    struct point_t left_upper_corner;
    struct point_t right_bottom_corner;
};

struct string_t {
    char *str;
    int length;
};
