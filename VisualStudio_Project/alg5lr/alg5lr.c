#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <windows.h>


struct Edge {
    int u;
    int v;
};

struct Graph {
    int n;
    int m;
    int is_directed;
    int** adj;
    int** inc;
    struct Edge* edges;
};

struct Graph* create_graph(int n, int is_directed) {
    struct Graph* g = calloc(1, sizeof(*g));
    g->n = n;
    g->is_directed = is_directed;
    g->adj = malloc(n * sizeof(int*));
    for (int i = 0; i < n; i++) {
        g->adj[i] = calloc(n, sizeof(int));
    }
    return g;
}

void free_graph(struct Graph* g) {
    if (!g) return;
    if (g->adj) {
        for (int i = 0; i < g->n; i++) free(g->adj[i]);
        free(g->adj);
    }
    if (g->inc) {
        for (int i = 0; i < g->n; i++) free(g->inc[i]);
        free(g->inc);
    }
    if (g->edges) free(g->edges);
    free(g);
}

void build_edge_list(struct Graph* g) {
    if (g->edges) {
        free(g->edges);
        g->edges = NULL;
    }
    g->m = 0;
    if (g->is_directed) {
        for (int i = 0; i < g->n; i++) {
            for (int j = 0; j < g->n; j++) {
                if (g->adj[i][j]) g->m++;
            }
        }
    }
    else {
        for (int i = 0; i < g->n; i++) {
            for (int j = i + 1; j < g->n; j++) {
                if (g->adj[i][j]) g->m++;
            }
        }
    }

    if (g->m == 0) return;

    g->edges = malloc(g->m * sizeof(struct Edge));
    int edge_idx = 0;

    if (g->is_directed) {
        for (int i = 0; i < g->n; i++) {
            for (int j = 0; j < g->n; j++) {
                if (g->adj[i][j]) {
                    g->edges[edge_idx].u = i + 1;
                    g->edges[edge_idx].v = j + 1;
                    edge_idx++;
                }
            }
        }
    }
    else {
        for (int i = 0; i < g->n; i++) {
            for (int j = i + 1; j < g->n; j++) {
                if (g->adj[i][j]) {
                    g->edges[edge_idx].u = i + 1;
                    g->edges[edge_idx].v = j + 1;
                    edge_idx++;
                }
            }
        }
    }
}

void print_edges(struct Graph* g) {
    printf("\nСписок рёбер (m = %d):\n", g->m);
    if (g->m == 0) {
        printf("  Рёбер нет\n");
        return;
    }
    for (int i = 0; i < g->m; i++) {
        if (g->is_directed) {
            printf("  e%-2d: v%d -> v%d\n", i + 1, g->edges[i].u, g->edges[i].v);
        }
        else {
            printf("  e%-2d: (v%d, v%d)\n", i + 1, g->edges[i].u, g->edges[i].v);
        }
    }
}

void print_adj(struct Graph* g) {
    printf("Матрица смежности:\n    ");
    for (int j = 0; j < g->n; j++) printf("v%d ", j + 1);
    printf("\n");
    for (int i = 0; i < g->n; i++) {
        printf("v%d  ", i + 1);
        for (int j = 0; j < g->n; j++) {
            printf("%-3d", g->adj[i][j]);
        }
        printf("\n");
    }
}

void print_inc(struct Graph* g) {
    if (g->m == 0) {
        printf("Рёбер нет\n");
        return;
    }
    printf("Матрица инцидентности:\n    ");
    for (int j = 0; j < g->m; j++) printf("e%-2d ", j + 1);
    printf("\n");
    for (int i = 0; i < g->n; i++) {
        printf("v%d  ", i + 1);
        for (int j = 0; j < g->m; j++) {
            printf("%-4d", g->inc[i][j]);
        }
        printf("\n");
    }
}

void task1(struct Graph* g) {
    printf("\n[Задание 1: Анализ матрицы смежности]\n");
    print_adj(g);

    int* deg_out = calloc(g->n, sizeof(int));
    int* deg_in = calloc(g->n, sizeof(int));
    int* deg_total = calloc(g->n, sizeof(int));

    for (int i = 0; i < g->n; i++) {
        for (int j = 0; j < g->n; j++) {
            if (g->adj[i][j]) {
                deg_out[i]++;
                deg_in[j]++;
            }
        }
    }

    for (int i = 0; i < g->n; i++) {
        deg_total[i] = g->is_directed ? (deg_out[i] + deg_in[i]) : deg_out[i];
    }

    printf("Размер графа (число рёбер): %d\n", g->m);

    printf("Степени вершин:\n");
    for (int i = 0; i < g->n; i++) {
        if (g->is_directed) {
            printf("  v%d: deg+ = %d (исход), deg- = %d (заход), сумма = %d\n",
                i + 1, deg_out[i], deg_in[i], deg_total[i]);
        }
        else {
            printf("  v%d: deg = %d\n", i + 1, deg_total[i]);
        }
    }

    printf("Изолированные вершины: ");
    int count = 0;
    for (int i = 0; i < g->n; i++) {
        if (deg_total[i] == 0) {
            printf("v%d ", i + 1);
            count++;
        }
    }
    if (count == 0) printf("нет");
    printf("\n");

    printf("Концевые (висячие) вершины: ");
    count = 0;
    for (int i = 0; i < g->n; i++) {
        if (deg_total[i] == 1) {
            printf("v%d ", i + 1);
            count++;
        }
    }
    if (count == 0) printf("нет");
    printf("\n");

    printf("Доминирующие вершины: ");
    count = 0;
    for (int i = 0; i < g->n; i++) {
        int target = g->is_directed ? deg_out[i] : deg_total[i];
        if (target == g->n - 1 && g->n > 1) {
            printf("v%d ", i + 1);
            count++;
        }
    }
    if (count == 0) printf("нет");
    printf("\n");

    free(deg_out);
    free(deg_in);
    free(deg_total);
}

void task2(struct Graph* g) {
    g->inc = malloc(g->n * sizeof(int*));
    for (int i = 0; i < g->n; i++) {
        g->inc[i] = calloc(g->m > 0 ? g->m : 1, sizeof(int));
    }

    for (int k = 0; k < g->m; k++) {
        int u = g->edges[k].u - 1;
        int v = g->edges[k].v - 1;

        if (g->is_directed) {
            g->inc[u][k] = 1;
            g->inc[v][k] = -1;
        }
        else {
            g->inc[u][k] = 1;
            g->inc[v][k] = 1;
        }
    }

    printf("\n[Задание 2*: Анализ матрицы инцидентности]\n");
    print_inc(g);

    int* deg = calloc(g->n, sizeof(int));
    for (int i = 0; i < g->n; i++) {
        for (int k = 0; k < g->m; k++) {
            if (g->inc[i][k] != 0) deg[i]++;
        }
    }

    printf("Размер графа: %d\n", g->m);

    printf("Степени вершин: ");
    for (int i = 0; i < g->n; i++) {
        printf("v%d: %d%s", i + 1, deg[i], (i < g->n - 1) ? ", " : "\n");
    }

    printf("Изолированные: ");
    int count = 0;
    for (int i = 0; i < g->n; i++) {
        if (deg[i] == 0) {
            printf("v%d ", i + 1);
            count++;
        }
    }
    if (count == 0) printf("нет");
    printf("\n");

    printf("Концевые: ");
    count = 0;
    for (int i = 0; i < g->n; i++) {
        if (deg[i] == 1) {
            printf("v%d ", i + 1);
            count++;
        }
    }
    if (count == 0) printf("нет");
    printf("\n");

    free(deg);
    for (int i = 0; i < g->n; i++) free(g->inc[i]);
    free(g->inc);
    g->inc = NULL;
}

void run_manual() {
    int n = 6;
    struct Graph* g = create_graph(n, 0);
    printf("\n--- Граф из методички (Рисунок 1) ---\n");
    g->adj[0][1] = g->adj[1][0] = 1;
    g->adj[0][2] = g->adj[2][0] = 1;
    g->adj[0][5] = g->adj[5][0] = 1;
    g->adj[1][3] = g->adj[3][1] = 1;
    g->adj[2][4] = g->adj[4][2] = 1;

    build_edge_list(g);
    print_edges(g);
    task1(g);
    task2(g);

    free_graph(g);
}

void run_random(int n, int is_directed, int prob) {
    struct Graph* g = create_graph(n, is_directed);
    printf("\n--- Случайный %s граф (n = %d, p = %d%%) ---\n",
        is_directed ? "ориентированный" : "неориентированный", n, prob);

    if (is_directed) {
        for (int i = 0; i < g->n; i++) {
            for (int j = 0; j < g->n; j++) {
                if (i != j) {
                    g->adj[i][j] = (rand() % 100 < prob) ? 1 : 0;
                }
            }
        }
    }
    else {
        for (int i = 0; i < g->n; i++) {
            for (int j = i + 1; j < g->n; j++) {
                g->adj[i][j] = (rand() % 100 < prob) ? 1 : 0;
                g->adj[j][i] = g->adj[i][j];
            }
        }
    }

    build_edge_list(g);
    print_edges(g);
    task1(g);
    task2(g);

    free_graph(g);
}

int main(int argc, char* argv[]) {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "Russian");
    srand((unsigned int)time(NULL));

    int choice;
    while (1) {
        printf("\n=== МЕНЮ ===\n");
        printf("1. Граф из методички (Рисунок 1)\n");
        printf("2. Случайный граф\n");
        printf("0. Выход\n");
        printf("Выбор: ");

        if (scanf("%d", &choice) != 1 || choice == 0) break;

        if (choice == 1) {
            run_manual();
        }
        else if (choice == 2) {
            int n = 6;
            int type = 0;
            int prob = 30;

            printf("Количество вершин n: ");
            if (scanf("%d", &n) != 1 || n < 2) n = 6;

            printf("Тип графа (0 - неориентированный, 1 - ориентированный): ");
            if (scanf("%d", &type) != 1 || (type != 0 && type != 1)) type = 0;

            printf("Вероятность наличия ребра (0..100%%): ");
            if (scanf("%d", &prob) != 1 || prob < 0 || prob > 100) prob = 30;

            run_random(n, type, prob);
        }
    }

    return 0;
}