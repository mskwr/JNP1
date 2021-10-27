/**
 * @authors Olaf Placha, Michał Skwarek
 */

#include <cmath>
#include <iostream>
#include <regex>
#include <unordered_map>
#include <unordered_set>

using namespace std;

/** Holds all possible gate types. */
enum Gate { NOT, XOR, AND, NAND, OR, NOR };

/** Child to gate type and set of parents mapping. */
using graph = unordered_map<int32_t, pair<Gate, unordered_set<int32_t>>>;

/**
 * Creates a gate depending on its name.
 * @param gateName : name of the gate
 * @return gate
 */
static Gate createGate(string &gateName) {
    if (gateName == "NOT")
        return NOT;
    else if (gateName == "XOR")
        return XOR;
    else if (gateName == "AND")
        return AND;
    else if (gateName == "NAND")
        return NAND;
    else if (gateName == "OR")
        return OR;
    else
        return NOR;
}

/**
 * Checks if number of input signals is suitable for given gate.
 * @param gateName : name of the gate
 * @param n : number of input signals
 * @return true if number of signals is suitable, otherwise false
 */
static bool correctNumberOfSignals(Gate &gateName, size_t n) {
    switch (gateName) {
        case NOT:
            return n == 1;
        case XOR:
            return n == 2;
        default:
            return n >= 2;
    }
}

/**
 * Checks if name of the gate is correct.
 * @param gateName : name of the gate
 * @return true if name is correct, otherwise false
 */
static bool correctGateName(string &gateName) {
    return regex_match(gateName, regex("NOT|XOR|AND|NAND|OR|NOR"));
}

/**
 * Checks if signal is correct. If it is, set its value on given variable.
 * @param sig : signal
 * @param curSig : signal value
 * @return true if signal is valid, otherwise false
 */
static bool correctSignal(string &sig, int32_t &curSig) {
    if (regex_match(sig, regex("[0-9]+"))) {
        stringstream(sig) >> curSig;
        return curSig >= 1 && curSig <= 999999999;
    }
    return false;
}

/**
 * Parses input.
 * If any error appears during parsing input, then false is returned and the
 * program is terminated (the second element of the pair is discarded).
 * Otherwise, true and graph storing connections is returned.
 * @return pair: (input correctness, graph)
 */
static pair<bool, graph> parseInput() {
    graph g;
    string line;
    size_t lineCount = 0;
    bool correctInput = true;

    while (getline(cin, line)) {
        ++lineCount;
        bool correctLine = true;
        stringstream str(line);
        string gateName;
        str >> gateName;

        // check gate name
        if (!correctGateName(gateName)) {
            correctInput = correctLine = false;
            cerr << "Error in line " << lineCount << ": " << line << "\n";
            continue;
        }

        Gate curGate = createGate(gateName);
        string outSig;
        int32_t curOutSig;
        str >> outSig;

        // check if signal is a number in correct range
        if (!correctSignal(outSig, curOutSig)) {
            correctInput = correctLine = false;
            cerr << "Error in line " << lineCount << ": " << line << "\n";
            continue;
        }

        string inSig;
        unordered_set<int32_t> parents;
        size_t count = 0;

        while (str >> inSig) {
            int32_t curInSig;

            if (!correctSignal(inSig, curInSig)) {
                correctInput = correctLine = false;
                cerr << "Error in line " << lineCount << ": " << line << "\n";
                break;
            }

            parents.insert(curInSig);
            ++count;

            // check if the number of input signals is correct
            if ((curGate == NOT && count > 1) || (curGate == XOR && count > 2))
                break;
        }

        if (correctLine && !correctNumberOfSignals(curGate, count)) {
            correctInput = correctLine = false;
            cerr << "Error in line " << lineCount << ": " << line << "\n";
            continue;
        } else if (correctLine && g.contains(curOutSig)) {
            correctInput = correctLine = false;
            cerr << "Error in line " << lineCount << ": "
                 << "signal ";
            cerr << curOutSig << " is assigned to multiple outputs.\n";
            continue;
        }

        if (correctLine)
            g.insert(make_pair(curOutSig, make_pair(curGate, parents)));
    }
    return make_pair(correctInput, g);
}

/**
 * Preforms DFS on the given graph and marks nodes that are in the current
 * recursion stack
 * @param g : graph
 * @param currentNode : node that is currently being visited
 * @param activeNodes : nodes that are being visited in the dfs stack
 * @param visitedNodes : nodes that were visited before
 * @return true iff there exists a cycle that contains currentNode
 */
static bool dfsWithActiveNodes(graph &g, int32_t currentNode,
                               unordered_set<int32_t> &activeNodes,
                               unordered_set<int32_t> &visitedNodes) {
    if (!g.contains(currentNode)) {
        // currentNode has no parents, it cannot be a part of a cycle
        return false;
    }
    if (activeNodes.contains(currentNode)) {
        // currentNode is in the current recursion stack, there is a cycle!
        return true;
    }
    // mark currentNode as active
    activeNodes.insert(currentNode);
    // visit all unvisited parents of the current node
    for (auto const &parent : g.find(currentNode)->second.second) {
        if (!visitedNodes.contains(parent)) {
            if (dfsWithActiveNodes(g, parent, activeNodes, visitedNodes)) {
                // if any of parents is a part of a cycle, return true
                return true;
            }
        }
    }
    // currentNode is no longer active
    activeNodes.erase(currentNode);
    // mark currentNode as visited
    visitedNodes.insert(currentNode);
    return false;
}

/**
 * Detects cycle in a graph
 * @param g : graph
 * @return true iff given graph contains cycle
 */
static bool hasCycle(graph &g) {
    unordered_set<int32_t> activeNodes;
    unordered_set<int32_t> visitedNodes;

    // start looking for cycle from nodes that have parents
    for (auto const &node : g) {
        // if any node is a part of a cycle, return true
        if (dfsWithActiveNodes(g, node.first, activeNodes, visitedNodes)) {
            return true;
        }
    }
    return false;
}

/**
 * Finds ids of signals that are input signals
 * @param g : graph
 * @return vector with input signals' ids
 */
static unordered_set<int32_t> findInputSignals(graph &g) {
    // set with signals that have at least one parent
    unordered_set<int32_t> children;

    for (auto const &pair : g) {
        children.insert(pair.first);
    }

    unordered_set<int32_t> inputSignals;

    // traverse all signals and add to set iff they do not have any parent
    for (auto const &pair : g) {
        for (auto const &parent : pair.second.second) {
            if (!children.contains(parent)) {
                inputSignals.insert(parent);
            }
        }
    }
    return inputSignals;
}

/**
 * Fills the bitset of given size with binary representation of given number.
 * @param n : number
 * @param size : size of bitset
 * @param bitset : bitset
 */
static void toBinary(size_t n, size_t size, vector<char> &bitset) {
    size_t i = size - 1;

    while (n != 0) {
        bitset[i] = n % 2 != 0;
        n /= 2;
        --i;
    }
}

/**
 * Returns ids of signals that are not input signals
 * @param g : graph
 * @return unordered set with all signals' ids
 */
static unordered_set<int32_t> getNonInputSignals(const graph &g) {
    unordered_set<int32_t> signals;

    for (auto const &pair : g) {
        signals.insert(pair.first);
    }
    return signals;
}

/**
 * Returns XOR of values passed in vector
 * @param signals : vector of signal values
 * @return boolean, result of XOR
 */
static bool gateXOR(const vector<char> &signals) {
    if (signals.size() == 1) {
        // gate must have 2 identical input streams
        return false;
    }
    if ((signals.at(0) && !signals.at(1))
        || (!signals.at(0) && signals.at(1))) {
        return true;
    }
    return false;
}

/**
 * Returns AND of values passed in vector
 * @param signals : vector of signal values
 * @return boolean, result of AND
 */
static bool gateAND(const vector<char> &signals) {
    for (auto signal : signals) {
        if (!signal) {
            return false;
        }
    }
    return true;
}

/**
 * Returns OR of values passed in vector
 * @param signals : vector of signal values
 * @return boolean, result of OR
 */
static bool gateOR(const vector<char> &signals) {
    for (auto signal : signals) {
        if (signal) {
            return true;
        }
    }
    return false;
}

/**
 * Returns output signal from a set of input signal and gate type
 * @param parentSignals : vector of signals' values
 * @param gateType : enum denoting gate type
 * @return output signal's value
 */
static bool
valuateSignalBasedOnParentsAndGate(const vector<char> &parentSignals,
                                   Gate gateType) {
    switch (gateType) {
        case NOT:
            return !parentSignals.at(0);
        case XOR:
            return gateXOR(parentSignals);
        case AND:
            return gateAND(parentSignals);
        case NAND:
            return !gateAND(parentSignals);
        case OR:
            return gateOR(parentSignals);
        case NOR:
            return !gateOR(parentSignals);
        default:
            throw invalid_argument("invalid gate type!");
    }
}

/**
 * Determines currentSignal valuation based on its parents
 * @param currentSignal : id of current signal
 * @param valuation : map with valuations
 * @param g : graph
 */
static bool dfsWithValuation(int32_t currentSignal,
                             unordered_map<int32_t, bool> &valuation,
                             const graph &g) {
    // if currentSignal has been valuated before, return it value
    if (valuation.contains(currentSignal)) {
        return valuation.at(currentSignal);
    }
    // valuate all parent signals
    vector<char> parentSignals;
    for (auto parentSignal : g.at(currentSignal).second) {
        parentSignals.push_back(dfsWithValuation(parentSignal, valuation, g));
    }

    bool currentSignalValuation = valuateSignalBasedOnParentsAndGate(
            parentSignals, g.at(currentSignal).first);
    // put valuation in the map
    valuation.insert(make_pair(currentSignal, currentSignalValuation));
    return currentSignalValuation;
}

/**
 * Valuates all signals from the graph and inserts their value to valuation map
 * @param valuation
 * @param g
 * @param signals
 */
static void populateValuation(unordered_map<int32_t, bool> &valuation,
                              const graph &g,
                              const unordered_set<int32_t> &signals) {
    for (auto signal : signals) {
        dfsWithValuation(signal, valuation, g);
    }
}

/**
 * Prints given valuation.
 * @param valuation : valuation
 */
static void printValuation(unordered_map<int32_t, bool> &valuation) {
    vector<int32_t> keys;
    keys.reserve(valuation.size());

    for (auto i : valuation)
        keys.push_back(i.first);

    sort(keys.begin(), keys.end());

    for (size_t i = 0; i < valuation.size(); ++i)
        cout << valuation.at(keys[i]);

    cout << "\n";
}

/**
 * Produces valuations in correct order, populate and print them.
 * @param g : child to gate type and set of parents mapping
 */
static void printTruthTable(graph &g) {
    unordered_set<int32_t> inputSignals = findInputSignals(g);
    vector<int32_t> keys;
    size_t n = inputSignals.size();
    keys.reserve(n);
    vector<char> bitset;
    bitset.reserve(n);

    for (size_t i = 0; i < n; ++i)
        bitset[i] = false;

    for (auto i = inputSignals.begin(); i != inputSignals.end();)
        keys.push_back(inputSignals.extract(i++).value());

    // sort valuation by input signals
    sort(keys.begin(), keys.end());

    for (size_t i = 0; i < (size_t)pow(2, n); ++i) {
        unordered_map<int32_t, bool> valuation;
        toBinary(i, n, bitset);

        // giving following binary numbers to sorted input signals
        for (size_t j = 0; j < n; ++j)
            valuation.insert(make_pair(keys[j], bitset[j]));

        unordered_set<int32_t> signals = getNonInputSignals(g);
        populateValuation(valuation, g, signals);
        printValuation(valuation);
    }
}

int main() {
    pair<bool, graph> p = parseInput();

    if (!p.first) {
        // there was some error when parsing input
        return 1;
    }

    graph g = p.second;

    if (hasCycle(g)) {
        cerr << "Error: ";
        cerr << "sequential logic analysis has not yet been implemented.\n";
        return 1;
    }

    printTruthTable(g);
    return 0;
}
