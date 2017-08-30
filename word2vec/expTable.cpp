#include "expTable.hpp"
#include <math.h>
#include <iostream>

using namespace std;


double* buildExpTable() {
    cout << "Building expTable......";
    double* table = new double[EXP_TABLE_SIZE];
    for (int i = 0; i < EXP_TABLE_SIZE; i++) {
        double x = ((double)i / EXP_TABLE_SIZE - 0.5) * 2.0 * MAX_EXP;
        double ex = exp(x);
        table[i] = ex / (1 + ex);
    }
    cout << "Finished" << endl;
    return table;
}

double lookupExpTable(double* table, double x) {
    int i = (int)((x / 2.0 / MAX_EXP + 0.5) * EXP_TABLE_SIZE);
    i = i >= EXP_TABLE_SIZE ? (EXP_TABLE_SIZE - 1) : (i < 0 ? 0 : i);
    return table[i];
}
