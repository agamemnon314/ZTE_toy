//
// Created by agamemnon on 17-4-27.
//

#ifndef ZTE_TOY_ALGORITHMS_H
#define ZTE_TOY_ALGORITHMS_H

#include <lemon/adaptors.h>
#include <lemon/list_graph.h>
#include <lemon/euler.h>
#include <lemon/path.h>
#include <string>
#include <vector>
#include <iostream>

using namespace lemon;
using namespace std;
typedef ListGraph::Node Node;
typedef ListGraph::NodeIt NodeIt;
typedef ListGraph::Edge Edge;
typedef ListGraph::EdgeIt EdgeIt;
typedef ListGraph::Arc Arc;
typedef ListGraph::ArcIt ArcIt;
typedef ListGraph::ArcMap<bool> ArcFlag;
typedef ListGraph::NodeMap<bool> NodeFlag;


bool arcs_to_path(ListGraph &g, Node s, Node t,
                  ArcFlag &a_flag, NodeFlag &u_flag, ListPath<ListGraph> &p) {
    SubDigraph<ListGraph, NodeFlag, ArcFlag> sub_graph(g, u_flag, a_flag);
    bool has_st = true;
    Edge e_ad;
    if (findEdge(g, t, s) == INVALID) {
        e_ad = g.addEdge(t, s);
        has_st = false;
    }
    sub_graph.enable(findArc(g, t, s));


    bool out_flag = eulerian(sub_graph);
    if (out_flag) {
        p.clear();
        for (DiEulerIt<SubDigraph<ListGraph, NodeFlag, ArcFlag>> a(sub_graph,
                                                                   s);
             a != INVALID; ++a) {
            p.addBack(a);
        }
        p.eraseBack();

    }
    sub_graph.disable(findArc(g, t, s));
    return out_flag;
}


#endif //ZTE_TOY_ALGORITHMS_H
