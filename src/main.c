#include "student.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int read_int(const char *prompt, int *out)
{
    char buf[64];
    printf("%s", prompt);
    fflush(stdout);
    if (fgets(buf, sizeof(buf), stdin) == NULL) return 0;
    char *end = NULL;
    long v = strtol(buf, &end, 10);
    if (end == buf) return 0;
    while (*end == ' ' || *end == '\t') end++;
    if (*end != '\n' && *end != '\0') return 0;
    *out = (int)v;
    return 1;
}

static int read_float(const char *prompt, float *out)
{
    char buf[64];
    printf("%s", prompt);
    fflush(stdout);
    if (fgets(buf, sizeof(buf), stdin) == NULL) return 0;
    char *end = NULL;
    float v = strtof(buf, &end);
    if (end == buf) return 0;
    while (*end == ' ' || *end == '\t') end++;
    if (*end != '\n' && *end != '\0') return 0;
    *out = v;
    return 1;
}

static int read_line(const char *prompt, char *out, size_t size)
{
    printf("%s", prompt);
    fflush(stdout);
    if (fgets(out, (int)size, stdin) == NULL) return 0;
    size_t len = strlen(out);
    if (len > 0 && out[len - 1] == '\n') out[len - 1] = '\0';
    else { int c; while ((c = getchar()) != '\n' && c != EOF) {} }
    return out[0] != '\0';
}

static void show_menu(void)
{
    printf("\n===== 学生管理系统 =====\n"
           " 1. 添加学生\n 2. 删除学生\n 3. 修改学生\n 4. 查找学生\n"
           " 5. 显示全部\n 6. 按学号排序\n 7. 按成绩排序\n 8. 统计信息\n"
           " 9. 保存到文件\n10. 从文件读取\n 0. 退出\n"
           "=========================\n");
}

static void handle_add(StudentList *list)
{
    Student s; memset(&s, 0, sizeof(s));
    if (!read_int("学号: ", &s.id) || s.id <= 0) { printf("学号无效。\n"); return; }
    if (list_find(list, s.id) != NULL) { printf("学号 %d 已存在。\n", s.id); return; }
    if (!read_line("姓名: ", s.name, NAME_LEN)) { printf("姓名不能为空。\n"); return; }
    if (strchr(s.name, ' ') != NULL) { printf("姓名中请不要包含空格。\n"); return; }
    if (!read_int("年龄: ", &s.age) || s.age <= 0 || s.age > 150) { printf("年龄无效。\n"); return; }
    if (!read_float("成绩: ", &s.score) || s.score < 0.0f || s.score > 100.0f) { printf("成绩无效（0 - 100）。\n"); return; }
    if (list_add(list, s)) printf("添加成功。\n");
    else printf("添加失败：内存不足。\n");
}

static void handle_update(StudentList *list)
{
    int id;
    if (!read_int("请输入要修改的学号: ", &id)) { printf("输入无效。\n"); return; }
    Student *p = list_find(list, id);
    if (p == NULL) { printf("未找到学号 %d。\n", id); return; }
    char name[NAME_LEN]; int age; float score;
    if (!read_line("新姓名: ", name, NAME_LEN) || strchr(name, ' ') != NULL) { printf("姓名无效。\n"); return; }
    if (!read_int("新年龄: ", &age) || age <= 0 || age > 150) { printf("年龄无效。\n"); return; }
    if (!read_float("新成绩: ", &score) || score < 0.0f || score > 100.0f) { printf("成绩无效。\n"); return; }
    strncpy(p->name, name, NAME_LEN - 1); p->name[NAME_LEN - 1] = '\0';
    p->age = age; p->score = score;
    printf("修改成功。\n");
}

static void handle_find(StudentList *list)
{
    int id;
    if (!read_int("请输入学号: ", &id)) { printf("输入无效。\n"); return; }
    Student *p = list_find(list, id);
    if (p == NULL) { printf("未找到学号 %d。\n", id); return; }
    printf("学号\t姓名\t年龄\t成绩\n%d\t%s\t%d\t%.2f\n", p->id, p->name, p->age, p->score);
}

int main(void)
{
    StudentList list;
    if (!list_init(&list, 8)) { fprintf(stderr, "错误：内存分配失败。\n"); return 1; }
    if (list_load(&list, DATA_FILE)) printf("已从 %s 加载 %d 条记录。\n", DATA_FILE, list.count);
    else printf("未找到数据文件 %s，将以空列表启动。\n", DATA_FILE);

    int running = 1;
    while (running) {
        show_menu();
        int choice = -1;
        if (!read_int("请选择: ", &choice)) { printf("输入无效，请重新输入。\n"); continue; }
        switch (choice) {
        case 1: handle_add(&list); break;
        case 2: { int id; if (!read_int("请输入要删除的学号: ", &id)) { printf("输入无效。\n"); break; } printf(list_remove(&list, id) ? "删除成功。\n" : "未找到该学号。\n"); break; }
        case 3: handle_update(&list); break;
        case 4: handle_find(&list); break;
        case 5: list_print(&list); break;
        case 6: list_sort_by_id(&list); printf("已按学号升序排序。\n"); list_print(&list); break;
        case 7: list_sort_by_score(&list); printf("已按成绩降序排序。\n"); list_print(&list); break;
        case 8: list_stats(&list); break;
        case 9: if (list_save(&list, DATA_FILE)) printf("已保存 %d 条记录到 %s。\n", list.count, DATA_FILE); else printf("保存失败，请检查 data 目录是否存在。\n"); break;
        case 10: if (list_load(&list, DATA_FILE)) printf("读取成功，共 %d 条记录。\n", list.count); else printf("读取失败：未找到 %s。\n", DATA_FILE); break;
        case 0: printf("再见！\n"); running = 0; break;
        default: printf("无效选项，请重新输入。\n"); break;
        }
    }
    list_free(&list);
    return 0;
}