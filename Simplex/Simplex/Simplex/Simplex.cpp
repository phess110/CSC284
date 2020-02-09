// Simplex.cpp
#include <iostream>
#include <algorithm>
#include <vector>

using std::vector;

int problemCount;
uint16_t n, m;

typedef vector<uint16_t> Basis;
typedef vector<vector<double>> DoubleMatrix;

int RowReduce(DoubleMatrix, vector<double>, vector<int> &);
vector<uint16_t>::iterator BlandEnter(Basis &, vector<double> &);
uint16_t BlandExit(DoubleMatrix &, vector<double> &, Basis &, uint16_t, double &);

/*
    Compute the smallest element t in B such that c[t] > 0
*/
Basis::iterator BlandEnter(Basis &B, vector<double> &c) {
    Basis::iterator it = B.begin();
    Basis::iterator minimum = B.end();
    bool found = false;

    for (; it != B.end(); it++) {
        if (c[*it] > 0) {
            if (found) {
                if (*it < *minimum) {
                    minimum = it;
                }
            }
            else {
                minimum = it;
                found = true;
            }
        }
    }

    return minimum;
}

/*

    param[out]: minSlack - minimum adjustment to remain feasible. If A_{i,t} <= 0 for all i in [m], minSlack has value -1 on return to indicate UNBOUNDED.

    Returns pointer to B[k] where k is choose to minimize B[k] subject to A_{k,t} > 0 and b_k/A_{k,t} = lambda^*
*/
uint16_t BlandExit(DoubleMatrix &A,
                        vector<double> &b,
                        Basis &B, 
                        uint16_t t, 
                        double & minSlack) {

    uint16_t minimum = m + 1;
    double tempSlack;
    bool found = false;
    
    for (int i = 0; i < m; i++) {
        if (A[i][t] > 0) {
            tempSlack = b[i] / A[i][t];
            if (found) {
                // TODO need to use approx equals
                if ((tempSlack == minSlack && B[i] < B[minimum]) || tempSlack < minSlack) {
                    minimum = i;
                    minSlack = tempSlack;
                }
            }
            else {
                minSlack = tempSlack;
                minimum = i;
                found = true;
            }
        }
    }

    // if not found return negative value to indicate failure
    minSlack = found ? minSlack : -1.;
    return minimum;
}

/**
    Returns optimal value of LP given by max c^Tx subject to Ax = b, x >= 0

    param[in]: A - matrix of constraint coefficients
    param[in]: b - vector of constaint values
    param[in]: c - vector of objective constants
    param[in]: B_comp - the complement of B in {0,...,m+n-1}

    param[out]: B - Contains initial feasible basis. If simplex halts, contains an optimal feasible basis.
    param[out]: x - Contains initial variable assignment. If simplex halts, contains an optimal assignment.

    Note: Initially, we require Ax = b and x >= 0 and A_B is invertible.
**/

double Simplex( double V,
                DoubleMatrix &A,
                vector<double> &b,
                vector<double> &c,
                vector<double> &x,
                Basis &B,
                vector<uint16_t> &B_comp) {

    double lambda;
    Basis::iterator t = BlandEnter(B_comp, c);

    while (t != B_comp.end()) {
        uint16_t s = BlandExit(A, b, B, *t, lambda);
        if (lambda < 0) {
            // unbounded
            // TODO 
            return 0.0;
        }
        else {
            double a_st = A[s][*t];
            // update assignment
            for (int i = 0; i < m; i++) {
                if (i != s) {
                    for (Basis::iterator j = B_comp.begin(); j != B_comp.end(); j++) {
                        if (*j != *t) {
                            A[i][*j] -= A[s][*j] * A[i][*t] / a_st;
                        }
                    }
                    A[i][B[s]]   = -(A[i][*t] / a_st);
                    b[i]        -= b[s] * (A[i][*t] / a_st);
                }
                else { 
                    // i = s
                    for (Basis::iterator j = B_comp.begin(); j != B_comp.end(); j++) {
                        if (*j != *t) {
                            A[s][*j]   /= a_st;
                            c[*j]      -= c[*t] * A[s][*j] / a_st;
                            x[*j]       = 0.;
                        }
                        else {
                            x[*t] = lambda;
                        }
                    }
                    A[s][B[s]] = 1. / a_st;
                    b[s] /= a_st;
                    c[B[s]] = -c[*t] / a_st;
                }
                x[B[i]] = b[i] - lambda * A[i][*t];
            }
        }

        V += lambda * c[*t];

        // update B and B^C
        uint16_t temp = *t;
        *t = B[s];
        B[s] = temp;

        t = BlandEnter(B_comp, c);
    }

    return V;
}

/*
    Outputs indices for an initial feasible basis.

    Assumes m x m identity has already been adjoined to A

    Calls simplex on intermediate LP: max -1y subject to A(x|y) = b, x >= 0
*/
void FeasibleBasis(DoubleMatrix &A, vector<double> &b) {
    Basis B, B_complement;
    vector<double> c;
    vector<double> x;
    double val = 0.0;
    for (uint16_t i = 0; i < n + m; i++) {
        if( i >= n ){ 
            B.push_back(i); 
            c.push_back(0.);
            x.push_back(b[i - n]);
            val -= b[i - n];
        }
        else {
            double f = 0.0;
            for (int j = 0; j < m; j++) {
                f -= A[j][i];
            }
            c.push_back(f);
            x.push_back(0.);
            B_complement.push_back(i);
        }
    }

    /*
    Equivalent program:
        c_B^TA_B^{-1}b + max c'^Tx
        A_B^{-1}Ax = A_B^{-1}b
        x >= 0
    where 
    c'^T = c^T - c_B^TA_B^{-1}A

    In this case A_B^{-1} = I_{m x m}, c_B^T = (-1,...,-1), so
    c'^T = (-C_0, -C_0,..., -C_{n-1}, 1-C_n, ... 1-C_{n+m-1}) and
    val = c_B^T b = - column sum(b)
    where C_i is the sum of column i of A. Note 1-C_i = 0 for i >= n
    */

    // TODO don't pass A to simplex by reference
    // TODO remove remaining y's
    // TODO output basis to cout
}

int main(int argc, char *argv[])
{
    std::cin >> problemCount;
    double r;
    // read in problem
    for (int i = 0; i < problemCount; i++) {
        std::cin >> n;
        std::cin >> m;
        vector<vector<double>> A;
        vector<double> b;
        for (int j = 0; j < m; j++) {
            vector<double> v;
            for (int k = 0; k < n; k++) {
                std::cin >> r;
                v.push_back(r);
            }
            for (int k = 0; k < m; k++) {
                // Adjoin m x m identity to A
                v.push_back((k == j) ? 1. : 0.);
            }
            A.push_back(v);
            std::cin >> r;
            b.push_back(r);
        }

        vector<int> redundant;
        int R = RowReduce(A, b, redundant);

        if (R == -1) {
            std::cout << "INFEASIBLE\n";
        }
        else {
            // remove redundant constraints
            for (vector<int>::iterator it = redundant.begin(); it != redundant.end(); it++) {
                A.erase(A.begin() + *it);
                b.erase(b.begin() + *it);
                m--;
            }
            //FeasibleBasis(A, b);
        }
    }
}

/*
    Computes the rank of matrix A by putting A in reduced row echelon form.
    If the linear system Ax = b is infeasible, returns -1.

    param[out]: redundancies - list of redundant constraints that can be removed
*/
int RowReduce(DoubleMatrix A, vector<double> b, vector<int> &redundancies) {
    int pivotRow = 0, pivotCol, rank;
    double scale;

    rank = m;

    while (pivotRow < m) {
        pivotCol = -1;
        // select a pivot column with nonzero entry
        for (int i = 0; i < n; i++) {
            if (A[pivotRow][i] != 0.0) {
                scale = A[pivotRow][i];
                for (int j = 0; j < n; j++) {
                    A[pivotRow][j] /= scale;
                }
                b[pivotRow] /= scale;
                pivotCol = i;
                break;
            }
        }

        if (pivotCol == -1) {
            // singularity
            if (b[pivotRow] != 0.0) {
                // infeasible
                return -1;
            }
            else {
                // redundant constraint, delete row
                rank--;
                redundancies.push_back(pivotRow);
            }
        }
        else {
            // Adjust A and B
            for (int i = 0; i < m; i++) {
                if (i != pivotRow) {
                    scale = A[i][pivotCol];
                    for (int j = 0; j < n; j++) {
                        A[i][j] -= scale * A[pivotRow][j];
                    }
                    b[i] -= scale * b[pivotRow];
                }
            }
        }

        pivotRow++;
    }
    return rank;
}