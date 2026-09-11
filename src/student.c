#include "student.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int list_init(StudentList *list, int capacity)
{
    if (capacity <= 0) capacity = 8;
    list->items = (Student *)malloc(sizeof(Student) * (size_t)capacity);
    if (list->items == NULL) {
        list->count = 0;
        list->capacity = 0;
        return 0;
    }
    list->count = 0;
    list->capacity = capacity;
    return 1;
}

void list_free(StudentList *list)
{
    free(list->items);
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}

void list_clear(StudentList *list)
{
    list->count = 0;
}

static int list_grow(StudentList *list)
{
    int new_cap = list->capacity * 2;
    Student *p = (Student *)realloc(list->items, sizeof(Student) * (size_t)new_cap);
    if (p == NULL) return 0;
    list->items = p;
    list->capacity = new_cap;
    return 1;
}

int list_add(StudentList *list, Student s)
{
    if (list->count >= list->capacity) {
        if (!list_grow(list)) return 0;
    }
    list->items[list->count++] = s;
    return 1;
}

int list_remove(StudentList *list, int id)
{
    for (int i = 0; i < list->count; i++) {
        if (list->items[i].id == id) {
            for (int j = i; j < list->count - 1; j++) {
                list->items[j] = list->items[j + 1];
            }
            list->count--;
            return 1;
        }
    }
    return 0;
}

Student *list_find(StudentList *list, int id)
{
    for (int i = 0; i < list->count; i++) {
        if (list->items[i].id == id) {
            return &list->items[i];
        }
    }
    return NULL;
}

void list_print(const StudentList *list)
{
    if (list->count == 0) {
        printf("暂无学生记录。\n");
        return;
    }
    printf("学号\t姓名\t年龄\t成绩\n");
    printf("--------------------------------\n");
    for (int i = 0; i < list->count; i++) {
        const Student *s = &list->items[i];
        printf("%d\t%s\t%d\t%.2f\n", s->id, s->name, s->age, s->score);
    }
}

void list_stats(const StudentList *list)
{
    if (list->count == 0) {
        printf("暂无学生记录。\n");
        return;
    }
    double sum = 0.0;
    int max_i = 0, min_i = 0;
    for (int i = 0; i < list->count; i++) {
        sum += list->items[i].score;
        if (list->items[i].score > list->items[max_i].score) max_i = i;
        if (list->items[i].score < list->items[min_i].score) min_i = i;
    }
    printf("总人数: %d\n", list->count);
    printf("平均分: %.2f\n", sum / list->count);
    printf("最高分: %.2f (%s, 学号 %d)\n", list->items[max_i].score, list->items[max_i].name, list->items[max_i].id);
    printf("最低分: %.2f (%s, 学号 %d)\n", list->items[min_i].score, list->items[min_i].name, list->items[min_i].id);
}

static int cmp_id(const void *a, const void *b)
{
    return ((const Student *)a)->id - ((const Student *)b)->id;
}

static int cmp_score(const void *a, const void *b)
{
    const Student *x = (const Student *)a;
    const Student *y = (const Student *)b;
    if (x->score < y->score) return 1;
    if (x->score > y->score) return -1;
    return 0;
}

void list_sort_by_id(StudentList *list)
{
    if (list->count > 1) qsort(list->items, (size_t)list->count, sizeof(Student), cmp_id);
}

void list_sort_by_score(StudentList *list)
{
    if (list->count > 1) qsort(list->items, (size_t)list->count, sizeof(Student), cmp_score);
}

int list_save(const StudentList *list, const char *filename)
{
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) return 0;
    for (int i = 0; i < list->count; i++) {
        const Student *s = &list->items[i];
        fprintf(fp, "%d %s %d %.2f\n", s->id, s->name, s->age, s->score);
    }
    fclose(fp);
    return 1;
}

int list_load(StudentList *list, const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) return 0;
    list_clear(list);
    Student s;
    while (fscanf(fp, "%d %31s %d %f", &s.id, s.name, &s.age, &s.score) == 4) {
        if (!list_add(list, s)) break;
    }
    fclose(fp);
    return 1;
}