//
// Created by agamemnon on 17-4-28.
//

#include "small_instance.h"

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
    int path_node_limit = 1500;
    int target_node_punishment = -1;
    int target_edge_punishment = -1;
    int forbidden_edge_punishment = -1;
    int path_node_limit_punishment = -1;
    Instance inst{g, edge_cost, edge_type, node_type, node_pos, s, t,
                  node_number, edge_number, target_node_number,
                  target_edge_number, forbidden_edge_number, target_nodes,
                  target_edges, forbidden_edges, path_node_limit,
                  target_node_punishment, target_edge_punishment,
                  forbidden_edge_punishment, path_node_limit_punishment};
    ListPath<ListGraph> p;
    Solution sol{p, 0, 0, 0, 0, 0, 0};

    string file_name = "./instances/100/1000_225_145_15_5";
    read_instance(inst, file_name);
    int png_width = 1000;
    int png_height = 800;
    int node_size = 6;
    draw_instance(inst, png_width, png_height, node_size, file_name);
    ArcFlag a_flag(g, false);
    NodeFlag u_flag(g, false);
    int milp_flag=1;
//    milp_flag = x_y_steiner(inst, a_flag, u_flag);
    if (milp_flag >= 1) {
//        arcs_to_path(g, inst.s, inst.t, a_flag, u_flag, p);
//        cout<<g.id(inst.s)<<endl;
//        cout<<g.id(inst.t)<<endl;
//        cout << "路径序列: " << endl;
//
//        write_solution(inst, p, file_name);
        p.clear();
        read_solution(inst, p, file_name);
        calculate_solution_information(inst, sol);
        display_solution_information(inst, sol);
        for (ListPath<ListGraph>::ArcIt a(p); a != INVALID; ++a) {
            a_flag[findArc(g, g.source(a), g.target(a))] = true;
            u_flag[g.source(a)] = true;
            u_flag[g.target(a)] = true;
        }
        draw_solution(inst, a_flag, u_flag, png_width, png_height, node_size,
                      file_name);
    }


}