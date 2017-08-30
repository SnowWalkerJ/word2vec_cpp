#include "expTable.hpp"
#include <math.h>


double* buildExpTable() {
    double* table = new double[EXP_TABLE_SIZE];
    for (int i = 0; i < EXP_TABLE_SIZE; i++) {
        double x = ((double)i / EXP_TABLE_SIZE - 0.5) * 2.0 * MAX_EXP;
        table[i] = exp(x);
    }
    return table;
}

double lookupExpTable(double* table, double x) {
    int i = (int)((x / 2.0 / MAX_EXP + 0.5) * EXP_TABLE_SIZE);
    i = min(max(i, 0), EXP_TABLE_SIZE-1);
    return table[i];
}