#pragma once

#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
#include "taskgen.h"

/*
 * Testing set generation for DBF-based schedulability tests.
 *
 * T1:  t = D_i + k*T_i  where t <= D_max
 * T2:  t = D_i + k*T_i  where D_max < t <= feasibility_bound
 *
 * Works for implicit and constrained-deadline systems.
 */

namespace TestingSet {

/* ============================
   Utility: compute D_max
   ============================ */
inline double computeDmax(const std::vector<Task>& taskSet)
{
    double dmax = 0.0;
    for (const auto& task : taskSet)
        dmax = std::max(dmax, task.D);
    return dmax;
}

/* ============================
   Utility: compute total utilization
   ============================ */
inline double computeUtilization(const std::vector<Task>& taskSet)
{
    double U = 0.0;
    for (const auto& task : taskSet)
        U += task.C / task.T;
    return U;
}

/* ============================
   Feasibility bound
   (Baruah–Mok–Rosier style bound)
   ============================ */
inline double computeFeasibilityBound(const std::vector<Task>& taskSet)
{
    double U = computeUtilization(taskSet);
    if (U >= 1.0)
        return -1.0;   // immediately infeasible

    double sumC = 0.0;
    for (const auto& task : taskSet)
        sumC += task.C;

    double Dmax = computeDmax(taskSet);

    // Classical processor-demand bound
    // L* = (sumC - Dmax * (1 - U)) / (1 - U)
    double bound = (sumC - Dmax * (1.0 - U)) / (1.0 - U);

    // Bound must at least be Dmax
    return std::max(bound, Dmax);
}

/* ============================
   Generate Testing Set 1
   ============================ */
inline std::vector<double>generateTestingSet1(const std::vector<Task>& taskSet)
{
    std::vector<double> Lset;
    double Dmax = computeDmax(taskSet);

    for (const auto& task : taskSet)
    {
        for (int k = 0;; ++k)
        {
            double t = task.D + k * task.T;
            if (t > Dmax)
                break;

            Lset.push_back(t);
        }
    }

    std::sort(Lset.begin(), Lset.end());
    Lset.erase(std::unique(Lset.begin(), Lset.end()), Lset.end());

    return Lset;
}

/* ============================
   Generate Testing Set 2
   ============================ */
inline std::vector<double>generateTestingSet2(const std::vector<Task>& taskSet)
{
    std::vector<double> Lset;

    double Dmax  = computeDmax(taskSet);
    double bound = computeFeasibilityBound(taskSet);

    if (bound < 0.0)
        return Lset;

    for (const auto& task : taskSet)
    {
        for (int k = 0;; ++k)
        {
            double t = task.D + k * task.T;

            if (t > bound)
                break;

            if (t > Dmax)
                Lset.push_back(t);
        }
    }

    std::sort(Lset.begin(), Lset.end());
    Lset.erase(std::unique(Lset.begin(), Lset.end()), Lset.end());

    return Lset;
}

/* ============================
   Unified Testing Set
   ============================ */
inline std::vector<double>generateTestingSet(const std::vector<Task>& taskSet)
{
    auto T1 = generateTestingSet1(taskSet);
    auto T2 = generateTestingSet2(taskSet);

    std::vector<double> testingSet;
    testingSet.reserve(T1.size() + T2.size());

    testingSet.insert(testingSet.end(), T1.begin(), T1.end());
    testingSet.insert(testingSet.end(), T2.begin(), T2.end());

    std::sort(testingSet.begin(), testingSet.end());
    testingSet.erase(std::unique(testingSet.begin(), testingSet.end()), testingSet.end());

     std::cout<<"---Lets see the testing set---\n";
    for (double L : testingSet) {
        std::cout << L << " ";
    }
    std::cout << "\n";


    return testingSet;
}

}