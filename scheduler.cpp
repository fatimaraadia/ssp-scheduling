#include "scheduler.h"
#include <iostream>
#include <algorithm>
#include <cmath>

Scheduler::Scheduler(std::vector<Task>& taskSet)
    : taskSet(taskSet) {
}

void Scheduler::initializeBlockingParameters() {
    // Iterate through each task (tau_i) in the task set (Gamma)
    for (auto& task : taskSet) {
        // beta_i <- C_i + q_i^td (Fully non-preemptive upper bound)
        // C_i: Worst Case Execution Time (wcet)
        // q_i^td: Total Deferral Time (totalDeferral)
        // Note: Ensure your Task struct has 'wcet' and 'totalDeferral' fields,
        // or adjust these names to match your existing Task definition.
        task.beta = task.C + task.q_td;

        // cnt_i <- 1
        task.cnt = 1;
    }
}

void Scheduler::printBlockingParameters() const {
    for (size_t i = 0; i < taskSet.size(); ++i) {
        std::cout << "Task " << i + 1
                  << " | beta = " << taskSet[i].beta
                  << " | cnt = " << taskSet[i].cnt
                  << "\n";
    }
}

double Scheduler::computeDBF(double L) const {
    double dbf = 0.0;

    for (const auto& task : taskSet) {
        if (L >= task.D) {
            double jobs =
                std::floor((L - task.D) / task.T) + 1.0;

            dbf += jobs * task.C;
        }
    }

    return dbf;
}

std::vector<double> Scheduler::generateFeasibilitySet(double feasibilityBound) const {
    std::vector<double> Lset;

    for (const auto& task : taskSet) {
        for (double k = 0; ; ++k) {
            double absDeadline = task.D + k * task.T;

            if (absDeadline > feasibilityBound)
                break;

            Lset.push_back(absDeadline);
        }
    }

    // remove duplicates
    std::sort(Lset.begin(), Lset.end());
    Lset.erase(std::unique(Lset.begin(), Lset.end()), Lset.end());

    return Lset;
}

/* THIS IS PREV VERSION ; before splitting the testing sets

bool Scheduler::isSchedulable(double feasibilityBound) {
    auto Lset = generateFeasibilitySet(feasibilityBound);

    for (double L : Lset) {
        double dbf = computeDBF(L);
        double slack = L - dbf;

        std::cout << "L = " << L
                  << " | DBF = " << dbf
                  << " | Slack = " << slack
                  << "\n";

        if (slack < 0) {
            return false;
        }
    }

    return true;
}
    */

double Scheduler::computeDmax() const {
    double dmax = 0.0;
    for (const auto& task : taskSet)
        dmax = std::max(dmax, task.D);
    return dmax;
}

double Scheduler::computeFeasibilityBound() const {
    double U = 0.0;
    double Csum = 0.0;

    for (const auto& task : taskSet) {
        U += task.C / task.T;
        Csum += task.C;
    }

    if (U >= 1.0)
        return -1;  // immediately unschedulable

    return Csum / (1.0 - U);
}


//The two testing sets:
std::vector<double> Scheduler::generateTestingSet1() const {
    std::vector<double> Lset;
    double Dmax = computeDmax();

    for (const auto& task : taskSet) {
        for (int k = 0;; ++k) {
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

std::vector<double> Scheduler::generateTestingSet2() const {
    std::vector<double> Lset;

    double Dmax = computeDmax();
    double bound = computeFeasibilityBound();

    if (bound < 0)
        return Lset;

    for (const auto& task : taskSet) {
        for (int k = 0;; ++k) {
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

bool Scheduler::isSchedulable() {
    auto T1 = generateTestingSet1();
    auto T2 = generateTestingSet2();

    std::vector<double> testingSet;
    testingSet.reserve(T1.size() + T2.size());
    testingSet.insert(testingSet.end(), T1.begin(), T1.end());
    testingSet.insert(testingSet.end(), T2.begin(), T2.end());

    for (double L : testingSet) {
        double dbf = computeDBF(L);
        double slack = L - dbf;

        if (slack < -1e-9)
            return false;
    }

    return true;
}