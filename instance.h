//
// Created by agamemnon on 17-4-27.
//

#ifndef ZTE_TOY_INSTANCE_H

#include <lemon/list_graph.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <lemon/dim2.h>
#include "algorithms.h"

using namespace lemon;
using namespace std;
typedef ListGraph::Node Node;
typedef ListGraph::NodeIt NodeIt;
typedef ListGraph::Edge Edge;
typedef ListGraph::EdgeIt EdgeIt;
typedef ListGraph::Arc Arc;
typedef ListGraph::ArcIt ArcIt;
typedef ListGraph::EdgeMap<double> EdgeCost;
typedef ListGraph::EdgeMap<int> EdgeType;
typedef ListGraph::NodeMap<int> NodeType;
typedef dim2::Point<double> Point;
typedef ListGraph::NodeMap<Point> Pos;
typedef ListGraph::EdgeMap<bool> EdgeFlag;
typedef ListGraph::ArcMap<bool> ArcFlag;
typedef ListGraph::NodeMap<bool> NodeFlag;

struct Instance {
    ListGraph &G;
    EdgeCost &edgeCost;
    EdgeType &edgeType;
    NodeType &nodeType;
    Pos &nodePos;
    Node s;
    Node t;
    int nodeNumber;
    int edgeNumber;
    int targetNodeNumber;
    int targetEdgeNumber;
    int forbiddenEdgeNumber;
    vector<Node> &targetNodes;
    vector<Edge> &targetEdges;
    vector<Edge> &forbiddenEdges;
    int pathNodeLimit;
    int targetNodePunishment;
    int targetEdgePunishment;
    int forbiddenEdgePunishment;
    int pathNodeLimitPunishment;
};

struct Solution {
    ListPath<ListGraph> &P;
    int pathLength;
    int violatedTargetNodeNumber;
    int violatedTargetEdgeNumber;
    int violatedForbiddenEdgeNumber;
    int pathNodeNumber;
    int violatedPathNodeNumber;
};

void read_instance(Instance &inst, string file_name) {
    ifstream infile;
    infile.open(file_name);
    unsigned long ind;
    double x, y;
    if (infile) {
        string str, s;
        string data_type = "";
        while (getline(infile, str)) {
            if (str[0] == '@') {
                ind = str.find(',');
                s = str.substr(0, ind);
                data_type = s;
                continue;
            }
            if (data_type == "@node") {
                ind = str.find(',');
                str = str.substr(ind + 1);
                Node u = inst.G.addNode();
                ind = str.find(',');
                s = str.substr(0, ind);
                str = str.substr(ind + 1);
                x = stod(s);
                ind = str.find(',');
                s = str.substr(0, ind);
                str = str.substr(ind + 1, 1);
                y = stod(s);
                inst.nodePos[u] = Point(x, y);
                inst.nodeType[u] = stoi(str);
                continue;
            }
            if (data_type == "@edge") {
                int u, v;
                ind = str.find(',');
                s = str.substr(0, ind);
                str = str.substr(ind + 1);
                u = stoi(s);
                ind = str.find(',');
                s = str.substr(0, ind);
                str = str.substr(ind + 1);
                v = stoi(s);
                Edge e = inst.G.addEdge(inst.G.nodeFromId(u),
                                        inst.G.nodeFromId(v));
                ind = str.find(',');
                s = str.substr(0, ind);
                str = str.substr(ind + 1);
                inst.edgeCost[e] = stoi(s);
                inst.edgeType[e] = stoi(str);

                continue;
            }
        }
        inst.nodeNumber = countNodes(inst.G);
        inst.edgeNumber = countEdges(inst.G);
        inst.targetNodeNumber = 0;
        for (NodeIt u(inst.G); u != INVALID; ++u) {
            if (inst.nodeType[u] == 1) {
                inst.targetNodes.push_back(u);
                inst.targetNodeNumber += 1;
            }
            if (inst.nodeType[u] == 2) {
                inst.s = u;
            }
            if (inst.nodeType[u] == 3) {
                inst.t = u;
            }
        }
        inst.targetEdgeNumber = 0;
        inst.forbiddenEdgeNumber = 0;
        for (EdgeIt e(inst.G); e != INVALID; ++e) {
            if (inst.edgeType[e] == 1) {
                inst.targetEdges.push_back(e);
                inst.targetEdgeNumber += 1;
            }
            if (inst.edgeType[e] == -1) {
                inst.forbiddenEdges.push_back(e);
                inst.forbiddenEdgeNumber += 1;
            }
        }

    }
    infile.close();

}

void read_solution(Instance &inst, ListPath<ListGraph> &p,
                   string file_name) {
    p.clear();
    ListGraph &g = inst.G;
    ifstream infile;
    infile.open(file_name + "_solution");
    unsigned long ind;
    if (infile) {
        string str, s;
        string data_type = "";
        while (getline(infile, str)) {
            if (str[0] == '@') {
                continue;
            }
            ind = str.find('-');
            s = str.substr(0, ind);
            str = str.substr(ind + 2);
            p.addBack(findArc(g, g.nodeFromId(stoi(s)),
                              g.nodeFromId(stoi(str))));

        }
        infile.close();
    }
}

void write_solution(Instance &inst, ListPath<ListGraph> &p,
                    string file_name) {
    ListGraph &g = inst.G;
    ofstream outfile;
    outfile.open(file_name + "_solution");
    outfile << "@Path" << endl;
    for (ListPath<ListGraph>::ArcIt a(p); a != INVALID; ++a) {
        outfile << g.id(g.source(a)) << "--" << g.id(g.target(a)) << endl;
    }
    outfile.close();
}

void calculate_solution_information(Instance &inst, Solution &sol) {
    ListGraph &g = inst.G;
    ListGraph::EdgeMap<double> &edge_cost = inst.edgeCost;
    ListGraph::EdgeMap<int> &edge_type = inst.edgeType;
    ListGraph::NodeMap<int> &node_type = inst.nodeType;
    ListPath<ListGraph> &p = sol.P;
    sol.pathLength = 0;
    sol.violatedTargetNodeNumber = 0;
    sol.violatedTargetEdgeNumber = 0;
    sol.violatedForbiddenEdgeNumber = 0;
    sol.violatedPathNodeNumber = 0;
    sol.pathNodeNumber = 1;
    EdgeFlag e_flag(g, false);
    NodeFlag u_flag(g, false);
    for (ListPath<ListGraph>::ArcIt a(p); a != INVALID; ++a) {
        sol.pathLength += edge_cost[findEdge(g, g.source(a), g.target(a))];
        sol.pathNodeNumber += 1;
        e_flag[findEdge(g, g.source(a), g.target(a))] = true;
        u_flag[g.source(a)] = true;
        u_flag[g.target(a)] = true;
    }
    for (EdgeIt e(g); e != INVALID; ++e) {
        if (edge_type[e] == 1 && e_flag[e] == false) {
            sol.violatedTargetEdgeNumber += 1;
        }
        if (edge_type[e] == -1 && e_flag[e] == true) {
            sol.violatedForbiddenEdgeNumber += 1;
        }
    }
    for (NodeIt u(g); u != INVALID; ++u) {
        if (node_type[u] == 1 && u_flag[u] == false) {
            sol.violatedTargetNodeNumber += 1;
        }
    }
    if (sol.pathNodeNumber > inst.pathNodeLimit) {
        sol.violatedPathNodeNumber = sol.pathNodeNumber - inst.pathNodeLimit;
    }
}

void display_solution_information(Instance &inst, Solution &sol) {
    ListGraph &g = inst.G;
    ListPath<ListGraph> &p = sol.P;
    cout << "路径序列: " << endl;
    for (ListPath<ListGraph>::ArcIt a(p); a != INVALID; ++a) {
        cout << g.id(g.source(a)) << "--";
    }
    cout << g.id(g.target(p.back())) << endl;
    cout << "路长: ";
    cout << sol.pathLength << endl;
    cout << "经过点数: ";
    cout << sol.pathNodeNumber << endl;
    cout << "超出点数（未超出为0）: ";
    cout << sol.violatedPathNodeNumber << endl;
    cout << "未经过目标点个数: ";
    cout << sol.violatedTargetNodeNumber << endl;
    cout << "未经过目标边条数: ";
    cout << sol.violatedTargetEdgeNumber << endl;
    cout << "经过禁止边条数: ";
    cout << sol.violatedForbiddenEdgeNumber << endl;
    cout << "///////////////////////////////"
         << "///////////////////////////////" << endl;
}

#define ZTE_TOY_INSTANCE_H

#endif //ZTE_TOY_INSTANCE_H
