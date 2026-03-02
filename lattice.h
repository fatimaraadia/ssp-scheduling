#ifndef LATTICE_H
#define LATTICE_H

#include <set>
#include <map>
#include <vector>
#include <iostream>


class Lattice {
public:
    Lattice();

    // Add a new security class
    void addElement(int id);

    // Add partial order relation: a <= b
    void addOrder(int a, int b);

    // Compute full transitive closure of <=
    void buildTransitiveClosure();

    // Check if a <= b
    bool leq(int a, int b) const;

    // Compute meet (greatest lower bound)
    int meet(int a, int b) const;

    // Compute join (least upper bound)
    int join(int a, int b) const;

    // Verify lattice property
    bool isLattice() const;

    // Print lattice structure
    //void printFullLattice() const;
    void printLattice() const;


private:
    std::set<int> elements;

    // adjacency[a] contains all b such that a <= b
    std::map<int, std::set<int>> adjacency;

    // helper functions
    std::set<int> upperBounds(int a, int b) const;
    std::set<int> lowerBounds(int a, int b) const;
};

#endif //#pragma once
