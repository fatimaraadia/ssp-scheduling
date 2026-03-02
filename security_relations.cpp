#include "security_relations.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>

SecurityRelations::SecurityRelations(const Lattice& lattice, const std::set<int>& elements) {
    // For each element Si in the lattice...
    for (int Si : elements) {
        // ...find all elements Sj...
        for (int Sj : elements) {
            // ...where Si is not less than or equal to Sj.
            if (!lattice.leq(Si, Sj)) {
                nonDominatingLookup[Si].insert(Sj);
            }
        }
    }
}

const std::set<int>& SecurityRelations::getNonDominatingSet(int Si) const {
    auto it = nonDominatingLookup.find(Si);
    if (it == nonDominatingLookup.end()) {
        throw std::runtime_error("Security level not found in lookup table.");
    }
    return it->second;
}

void SecurityRelations::printTable() const {
    std::cout << "Security Relations Lookup Table (S_i not <= S_j):\n";
    std::vector<int> sortedKeys;
    for (const auto& pair : nonDominatingLookup) {
        sortedKeys.push_back(pair.first);
    }
    std::sort(sortedKeys.begin(), sortedKeys.end());

    for (int key : sortedKeys) {
        std::cout << "  For S_i = " << key << ", the set of S_j is { ";
        for (int val : nonDominatingLookup.at(key)) {
            std::cout << val << " ";
        }
        std::cout << "}\n";
    }
    std::cout << std::endl;
}