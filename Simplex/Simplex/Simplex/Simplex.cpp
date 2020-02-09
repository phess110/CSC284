// Simplex.cpp : This file contains the 'main' function. Program execution begins and ends there.
#include <iostream>
#include <algorithm>
#include <vector>

using std::vector;

int problemCount;
uint16_t n, m;

/*
    Compute the smallest element t in B such that c[t] > 0
*/
vector<uint16_t>::iterator BlandEnter(vector<uint16_t> &B, vector<double> &c) {
    vector<uint16_t>::iterator it = B.begin();
    vector<uint16_t>::iterator minimum = B.end();
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
uint16_t BlandExit(vector<vector<double>> &A, 
                        vector<double> &b,
                        vector<uint16_t> &B, 
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

    param[out]: B - Contains initial feasible basis. If simplex halts, contains an optimal feasible basis.
    param[out]: x - Contains initial variable assignment. If simplex halts, contains an optimal assignment.

    Note: Initially, we require Ax = b and x >= 0 and A_B is invertible.
**/

double Simplex( double V,
                vector<vector<double>> &A, 
                vector<double> &b,
                vector<double> &c,
                vector<double> &x,
                vector<uint16_t> &B,
                vector<uint16_t> &B_complement) {


    vector<uint16_t>::iterator t = BlandEnter(B_complement, c);
    while (t != B_complement.end()) {
        double lambda;
        uint16_t s = BlandExit(A, b, B, *t, lambda);
        if (lambda < 0) {
            // unbounded
            return 0.0;
        }
        else {
            double a_st = A[s][*t];
            // update assignment
            for (int i = 0; i < m; i++) {
                if (i != s) {
                    for (vector<uint16_t>::iterator j = B_complement.begin(); j != B_complement.end(); j++) {
                        if (*j != *t) {
                            A[i][*j] -= A[s][*j] * A[i][*t] / a_st;
                        }
                    }
                    A[i][B[s]]   = -(A[i][*t] / a_st);
                    b[i]        -= b[s] * (A[i][*t] / a_st);
                }
                else { 
                    // i = s
                    for (vector<uint16_t>::iterator j = B_complement.begin(); j != B_complement.end(); j++) {
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

        t = BlandEnter(B_complement, c);
    }

    return V;
}

/*
    Outputs indices for an initial feasible basis.

    Assumes m x m identity has already been adjoined to A

    Calls simplex on intermediate LP: max -1y subject to A(x|y) = b, x >= 0
*/
void FeasibleBasis(vector<vector<double>>& A, vector<double>& b) {
    vector<uint16_t> B, B_complement;
    vector<double> c;
    vector<double> x;
    double val = 0.0;
    for (uint16_t i = 0; i < n + m; i++) {
        if( i >= n ){ 
            B.push_back(i); 
            c.push_back(0.);
            x.push_back(static_cast<float>(b[i - n]));
            val -= static_cast<float>(b[i - n]);
        }
        else {
            float f = 0.0;
            for (int j = 0; j < m; j++) {
                f -= static_cast<float>(A[j][i]);
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

        // TODO  row reduce A, check for INFEASIBILITY or redundancies
        FeasibleBasis(A, b);
    }
}
