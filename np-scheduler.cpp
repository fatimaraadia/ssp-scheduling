#include "np-scheduler.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include "testing_set.h"

// Constructor
NPScheduler::NPScheduler(std::vector<Task>& taskSet,
                         const SecurityRelations& secRel)
    : taskSet(taskSet), securityRelations(secRel) {}

// Print beta (no cnt used)
void NPScheduler::printBlockingParameters() const {
    std::cout << "=== NP-Scheduler Blocking Parameters ===\n";
    for (size_t i = 0; i < taskSet.size(); ++i) {
        std::cout << "Task " << i + 1
                  << " | S=" << taskSet[i].S
                  << " | beta=" << taskSet[i].beta
                  << "\n";
    }
}

// Initialize beta = C + q_td
void NPScheduler::initializeBlockingParameters() {
    std::cout << "\n=== Initializing NP-Scheduler Blocking Parameters ===\n";
    for (auto& task : taskSet) {
        task.beta = task.C + task.q_td;
        std::cout << "Task S=" << task.S
                  << "  beta=" << task.beta << "\n";
    }
}

// DBF for one task (constrained-deadline model)
double NPScheduler::dbf(const Task& task, double L) const {
    if (L < task.D) return 0.0;
    double jobs = std::floor((L - task.D) / task.T) + 1.0;
    return jobs * task.C;
}

// Total DBF
double NPScheduler::totalDBF(double L) const {
    double sum = 0.0;
    for (const auto& task : taskSet)
        sum += dbf(task, L);
    return sum;
}

// Maximum blocking by higher-security tasks
double NPScheduler::computeBlocking(double L) const
{
    double maxBlocking = 0.0;

    for (const auto& low : taskSet) // tau_k
    {
        if (L < low.D) continue; // only consider tasks whose deadline <= L

        for (const auto& high : taskSet) // tau_j
        {
            if (high.S <= low.S) continue; // only higher security can block

            double beta_j = high.C + high.q_td;
            maxBlocking = std::max(maxBlocking, beta_j);

            std::cout<<"Candidate blocker: S=" << high.S
                     << ", C+q_td=" << beta_j
                     << " blocks task S=" << low.S
                     << "\n";
        }
    }

    return maxBlocking;
}

/*double NPScheduler::computeBlocking(double L) const
{
    double maxBlocking = 0.0;

    for (const auto& low : taskSet)  // tau_k
    {
        if (low.D > L) continue; // only consider tasks whose deadline <= L

        for (const auto& high : taskSet) // tau_j
        {
            if (&high == &low) continue;        // skip self
            if (high.D <= L) continue;          // candidate blockers must have D_j > L
            if (high.S < low.S) continue;      // only higher security can block

            //print for debugging
            std::cout<<"high.C + high.q_td : "<<high.C <<"& "<< high.q_td<<"\n";

            double beta_j = high.C + high.q_td;
            std::cout<<"beta calc: "<<beta_j<<"\n";
            maxBlocking = std::max(maxBlocking, beta_j);
        }
    }
    return maxBlocking;
}
*/


// Schedulability test
bool NPScheduler::isSchedulable() {
    std::vector<double> testingSet = TestingSet::generateTestingSet(taskSet);

    for (double L : testingSet) {
        std::cout << "\nChecking L=" << L << "\n";

        double demand = totalDBF(L);
        double block  = computeBlocking(L);

        std::cout << "Total DBF = " << demand
                  << " | Blocking = " << block
                  << " | Total = " << demand + block
                  << " | L = " << L << "\n";

        if (demand + block > L) {
            std::cout << "Violation at L=" << L << "\n";
            return false;
        }
    }
    return true;
}