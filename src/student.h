#ifndef STUDENT_H
#define STUDENT_H

#define NAME_LEN 32
#define DATA_FILE "data/students.dat"

typedef struct {
    int   id;
    char  name[NAME_LEN];
    int   age;
    float score;
} Student;

typedef struct {
    Student *items;
    int      count;
    int      capacity;
} StudentList;

/* 生命周期 */
int  list_init(StudentList *list, int capacity);
void list_free(StudentList *list);
void list_clear(StudentList *list);

/* 增删查改 */
int      list_add(StudentList *list, Student s);
int      list_remove(StudentList *list, int id);
Student *list_find(StudentList *list, int id);

/* 展示与统计 */
void list_print(const StudentList *list);
void list_stats(const StudentList *list);

/* 排序 */
void list_sort_by_id(StudentList *list);
void list_sort_by_score(StudentList *list);

/* 文件读写 */
int list_save(const StudentList *list, const char *filename);
int list_load(StudentList *list, const char *filename);

#endif