//
// Created by agamemnon on 17-4-28.
//

#include<iostream>
#include <list>
#include <lemon/list_graph.h>
#include "ItemPath.h"
#include "instance.h"
#include "heuristic.h"
#include "milp.h"
#include "draw.h"

using namespace std;
using namespace lemon;

int main() {
    ListGraph g;
    EdgeCost edge_cost(g);
    EdgeType edge_type(g);
    NodeType node_type(g);
    Pos node_pos(g);
    Node s;
    Node t;
    int node_number = 0;
    int edge_number = 0;
    int target_node_number = 0;
    int target_edge_number = 0;
    int forbidden_edge_number = 0;
    vector<Node> target_nodes(0);
    vector<Edge> target_edges(0);
    vector<Edge> forbidden_edges(0);
    int path_node_limit = 9;
    int target_node_punishment = -1;
    int target_edge_punishment = -1;
    int forbidden_edge_punishment = -1;
    int path_node_limit_punishment = -1;
    Instance toy{g, edge_cost, edge_type, node_type, node_pos, s, t,
                 node_number, edge_number, target_node_number,
                 target_edge_number, forbidden_edge_number, target_nodes,
                 target_edges, forbidden_edges, path_node_limit,
                 target_node_punishment, target_edge_punishment,
                 forbidden_edge_punishment, path_node_limit_punishment};
    ListPath<ListGraph> p;
    Solution sol{p, 0, 0, 0, 0, 0, 0};

    string file_name = "100_5_5_5_5";
    read_instance(toy, file_name);
    int png_width = 1000;
    int png_height = 800;
    int node_size = 10;
    draw_instance(toy, png_width, png_height, node_size, file_name);


    EdgeCost edge_length(g);
    for (EdgeIt e(g); e != INVALID; ++e) {
        if (edge_type[e] == -1) {
            edge_length[e] += 1e10;
        } else {
//            edge_length[e] = 1;
            edge_length[e] = edge_cost[e];
        }
    }
    Dijkstra<ListGraph, EdgeCost> dijk(g, edge_length);

    vector<vector<double>> dist;
    for (NodeIt u(g); u != INVALID; ++u) {
        vector<double> x(countNodes(g), 0);
        dist.push_back(x);
    }
    for (NodeIt u(g); u != INVALID; ++u) {
        dijk.run(u);
        for (NodeIt v(g); v != INVALID; ++v) {
            dist[g.id(u)][g.id(v)] = dijk.dist(v);
        }
    }


    set<Target> target_set;
    for (EdgeIt e(g); e != INVALID; ++e) {
        if (edge_type[e] == 1) {
            Target x;
            x.isEdge = true;
            x.id = g.id(e);
            target_set.insert(x);
        }
    }
    for (NodeIt u(g); u != INVALID; ++u) {
        if (node_type[u] == 1) {
            Target x;
            x.isNode = true;
            x.id = g.id(u);
            target_set.insert(x);
        }
    }

    list <Target> path_list;
    Target xs, xt;
    xs.isNode = true;
    xs.id = g.id(toy.s);
    xt.isNode = true;
    xt.id = g.id(toy.t);
    path_list.push_back(xs);
    path_list.push_back(xt);
//    Target x;
//    x.isNode = true;
//    x.id = 7;
//    cout << xs.id << endl;

    print_list(path_list, g);
    farthestInsertion(path_list, g, edge_length, target_set, dist);
    print_list(path_list, g);

    cout << pathLength(path_list, g, edge_length, dist);

    return 0;
}