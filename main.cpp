#include "taskgen.h"
#include "dag_transformer.h"
#include <iostream>

int main() {
    int t_n;      // tasks per task set
    int n;        // number of task sets
    double U;     // total utilization

    std::cout << "Enter tasks per task set (t_n): ";
    std::cin >> t_n;

    std::cout << "Enter number of task sets (n): ";
    std::cin >> n;

    std::cout << "Enter total utilization (U): ";
    std::cin >> U;

    TaskGenerator generator;

    auto taskSets = generator.generateTaskSets(n, t_n, U);

    // Print and save the tasksets
    generator.printTaskSets(taskSets);
    generator.saveTaskSetsToFile(taskSets, "generated_tasksets.txt");

    // Loop thru each taskset
    for (size_t i = 0; i < taskSets.size(); ++i) {
        std::cout << "Processing Task Set " << i + 1 << ":\n";
        DAGTransformer transformer(taskSets[i]);
        transformer.buildLattice();   // First, build the security lattice
        transformer.buildRelations(); // Then, build the lookup table based on the lattice
        transformer.transformAndPrintBySecurityLevel();
    }



    return 0;
}