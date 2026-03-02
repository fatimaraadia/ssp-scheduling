#include "lattice.h"
#include <algorithm>
#include <iomanip>
#include <stdexcept>

Lattice::Lattice() {}

void Lattice::addElement(int id) {
    elements.insert(id);
    adjacency[id]; // ensure key exists
}

void Lattice::addOrder(int a, int b) {
    adjacency[a].insert(b);
}

void Lattice::buildTransitiveClosure() {
    // Floyd-Warshall closure
    for (int k : elements) {
        for (int i : elements) {
            if (adjacency[i].count(k)) {
                for (int j : adjacency[k]) {
                    adjacency[i].insert(j);
                }
            }
        }
    }

    // reflexivity
    for (int e : elements) {
        adjacency[e].insert(e);
    }
}

bool Lattice::leq(int a, int b) const {
    auto it = adjacency.find(a);
    if (it == adjacency.end()) return false;
    return it->second.count(b) > 0;
}

std::set<int> Lattice::upperBounds(int a, int b) const {
    std::set<int> result;
    for (int e : elements) {
        if (leq(a, e) && leq(b, e))
            result.insert(e);
    }
    return result;
}

std::set<int> Lattice::lowerBounds(int a, int b) const {
    std::set<int> result;
    for (int e : elements) {
        if (leq(e, a) && leq(e, b))
            result.insert(e);
    }
    return result;
}

int Lattice::join(int a, int b) const {
    auto ups = upperBounds(a, b);

    int candidate = -1;
    for (int u : ups) {
        bool minimal = true;
        for (int v : ups) {
            if (u != v && leq(v, u)) {
                minimal = false;
                break;
            }
        }
        if (minimal) {
            if (candidate != -1)
                throw std::runtime_error("Join not unique.");
            candidate = u;
        }
    }

    if (candidate == -1)
        throw std::runtime_error("No join exists.");

    return candidate;
}

int Lattice::meet(int a, int b) const {
    auto lows = lowerBounds(a, b);

    int candidate = -1;
    for (int l : lows) {
        bool maximal = true;
        for (int v : lows) {
            if (l != v && leq(l, v)) {
                maximal = false;
                break;
            }
        }
        if (maximal) {
            if (candidate != -1)
                throw std::runtime_error("Meet not unique.");
            candidate = l;
        }
    }


    if (candidate == -1)
        throw std::runtime_error("No meet exists.");

    return candidate;
}

bool Lattice::isLattice() const {
    for (int a : elements) {
        for (int b : elements) {
            try {
                join(a, b);
                meet(a, b);
            }
            catch (...) {
                return false;
            }
        }
    }
    return true;
}

void Lattice::printLattice() const {
    // Use a sorted vector for predictable output order
    std::vector<int> sortedElements(elements.begin(), elements.end());

//    std::cout << "Lattice Partial Order:\n";
//    for (int a : sortedElements) {
//        for (int b : sortedElements) {
//           // Exclude reflexive relationships 
//            if (a != b && leq(a, b)) {
//                std::cout << "  " << a << " <= " << b << "\n";
//            }
//        }
//    }
    std::cout << std::endl;

    std::cout << "Lattice Connections:\n";
    for (int a : sortedElements) {
        for (int b : sortedElements) {
            if (a == b) continue;

            // Check if b covers a (a -> b)
            if (leq(a, b)) {
                bool isCover = true;
                for (int k : sortedElements) {
                    if (k != a && k != b && leq(a, k) && leq(k, b)) {
                        isCover = false;
                        break;
                    }
                }
                if (isCover) {
                    std::cout << "  " << a << " -> " << b << "\n";
                }
            }
        }
    }
}