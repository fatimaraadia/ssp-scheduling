#include "scheduler.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include "testing_set.h"


//OLD CONSTRUCTOR:
//Scheduler::Scheduler(std::vector<Task>& taskSet)
//    : taskSet(taskSet) {}

//NEW Constructor, with security relations table access
Scheduler::Scheduler(std::vector<Task>& taskSet,
                     const SecurityRelations& secRel): taskSet(taskSet),securityRelations(secRel) {}

void Scheduler::printBlockingParameters() const {
    for (size_t i = 0; i < taskSet.size(); ++i) {
        std::cout << "Task " << i + 1
                  << " | beta = " << taskSet[i].beta
                  << " | cnt = " << taskSet[i].cnt
                  << "\n";
    }
}

/* ============================================================
   STEP 1: Initialize β_i = C_i + q_i^td
   ============================================================ */
void Scheduler::initializeBlockingParameters()
{
    std::cout << "\n=== Initializing Blocking Parameters ===\n";

    for (auto& task : taskSet)
    {
        task.beta = task.C + task.q_td;
        task.cnt  = 1;

        std::cout << "Task S=" << task.S
                  << "  beta=" << task.beta << "\n";
    }
}

/* ============================================================
   DBF for constrained deadlines
   ============================================================ */
double Scheduler::dbf(const Task& task, double L) const
{
    if (L < task.D) return 0.0;

    double jobs = std::floor((L - task.D) / task.T) + 1.0;
    return jobs * task.C;
}

/* ============================================================
   Compute total DBF
   ============================================================ */
double Scheduler::totalDBF(double L) const
{
    double sum = 0.0;
    for (const auto& task : taskSet)
        sum += dbf(task, L);

    return sum;
}

/* ============================================================
   Compute blocking B(L)
   A lower security task can be blocked by higher security ones
   ============================================================ */
double Scheduler::computeBlocking(double L) const
{
    double maxBlocking = 0.0;

    for (const auto& low : taskSet)
    {
        for (const auto& high : taskSet)
        {
            if (securityRelations.dominates(high.S, low.S) &&
                high.S != low.S)
            {
                maxBlocking = std::max(maxBlocking, high.beta);
            }
        }
    }

    return maxBlocking;
}

/* ============================================================
   Adjust β using slack and compute cnt
   ============================================================ */
void Scheduler::adjustBlocking(double L)
{
    std::cout << "\n=== Adjusting Blocking at L=" << L << " ===\n";

    double demand = totalDBF(L);
    double slack  = L - demand;

    std::cout << "Total DBF = " << demand << "\n";
    std::cout << "Slack     = " << slack  << "\n";

    if (slack <= 0)
    {
        std::cout << "No slack available.\n";
        return;
    }

    for (auto& task : taskSet)
    {
        // Only adjust higher security tasks
        bool blocksSomeone = false;
        for (const auto& other : taskSet)
        {
            if (securityRelations.dominates(task.S, other.S) &&
                task.S != other.S)
            {
                blocksSomeone = true;
                break;
            }
        }

        if (!blocksSomeone) continue;

        double oldBeta = task.beta;

        // STEP 1: reduce beta to slack
        task.beta = std::min(task.beta, slack);

        std::cout << "\nTask S=" << task.S << "\n";
        std::cout << "  beta before = " << oldBeta << "\n";
        std::cout << "  beta after  = " << task.beta << "\n";

        // STEP 2: compute cnt
        double numerator   = task.C - task.beta + task.q_td;
        double denominator = task.beta - task.q_td - task.q_su;

        std::cout << "  numerator   = " << numerator   << "\n";
        std::cout << "  denominator = " << denominator << "\n";

        if (denominator <= 0)
        {
            std::cout << "  denominator <= 0, forcing cnt=1\n";
            task.cnt = 1;
        }
        else
        {
            double ratio = numerator / denominator;
            task.cnt = std::ceil(ratio) + 1;
        }

        if (task.cnt < 1) task.cnt = 1;

        std::cout << "  cnt = " << task.cnt << "\n";

        // STEP 3: update beta after splitting
        task.beta =
            (task.C + task.q_td +
             (task.cnt - 1) * task.q_su)
            / task.cnt;

        std::cout << "  beta after splitting = "
                  << task.beta << "\n";
    }
}

/* ============================================================
   Main Schedulability Test
   ============================================================ */
bool Scheduler::isSchedulable()
{
    std::cout << "\n=== Starting Schedulability Test ===\n";

    //std::vector<double> testingSet = generateTestingSet(taskSet);
    std::vector<double> testingSet =
    TestingSet::generateTestingSet(taskSet);

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
