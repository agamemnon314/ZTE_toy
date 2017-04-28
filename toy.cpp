#include <iostream>
#include <lemon/list_graph.h>
#include "toy.h"
#include "instance.h"
#include "milp.h"
#include "draw.h"
#include "algorithms.h"

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

    string file_name = "ant";
    read_instance(toy, file_name);
    int png_width = 800;
    int png_height = 500;
    int node_size = 10;
    draw_instance(toy, png_width, png_height, node_size, file_name);


    ArcFlag a_flag(g);
    NodeFlag u_flag(g);
    int milp_flag;
    milp_flag = x_y_steiner(toy, a_flag, u_flag);
    if (milp_flag >= 1) {
        cout << "找到最优解！" << endl;
        cout << "最优值: " << milp_flag << endl;
        arcs_to_path(g, toy.s, toy.t, a_flag, u_flag, p);
        write_solution(toy, p, file_name);
        p.clear();
        read_solution(toy, p, file_name);
        calculate_solution_information(toy, sol);
        draw_solution(toy, a_flag, u_flag, png_width, png_height, node_size,
                      file_name);
    }

    if (milp_flag == -1) {
        cout << "当前问题无可行解！" << endl;
        cout << "松弛必经点约束" << endl;
        toy.targetNodePunishment = 10;
        toy.targetEdgePunishment = -1;
        toy.forbiddenEdgePunishment = -1;
        toy.pathNodeLimitPunishment = -1;
        milp_flag = x_y_steiner(toy, a_flag, u_flag);
        if (milp_flag == -1) {
            cout << "当前问题无可行解！" << endl;
        }
        if (milp_flag >= 1) {
            cout << "找到最优解！" << endl;
            cout << "最优值: " << milp_flag << endl;
            arcs_to_path(g, toy.s, toy.t, a_flag, u_flag, p);

            write_solution(toy, p, file_name + "_1");
            p.clear();
            read_solution(toy, p, file_name + "_1");
            read_solution(toy, p, file_name);
            calculate_solution_information(toy, sol);
            cout << g.id(g.target(p.back())) << endl;
            draw_solution(toy, a_flag, u_flag, png_width, png_height, node_size,
                          file_name + "_1");
        }
        cout << "松弛必经边约束" << endl;
        toy.targetNodePunishment = -1;
        toy.targetEdgePunishment = 10;
        toy.forbiddenEdgePunishment = -1;
        toy.pathNodeLimitPunishment = -1;
        milp_flag = x_y_steiner(toy, a_flag, u_flag);
        if (milp_flag == -1) {
            cout << "当前问题无可行解！" << endl;
        }
        if (milp_flag >= 1) {
            cout << "找到最优解！" << endl;
            cout << "最优值: " << milp_flag << endl;
            arcs_to_path(g, toy.s, toy.t, a_flag, u_flag, p);

            write_solution(toy, p, file_name + "_2");
            p.clear();
            read_solution(toy, p, file_name + "_2");
            calculate_solution_information(toy, sol);
            draw_solution(toy, a_flag, u_flag, png_width, png_height, node_size,
                          file_name + "_2");
        }
        cout << "松弛禁止边约束" << endl;
        toy.targetNodePunishment = -1;
        toy.targetEdgePunishment = -1;
        toy.forbiddenEdgePunishment = 10;
        toy.pathNodeLimitPunishment = -1;
        milp_flag = x_y_steiner(toy, a_flag, u_flag);
        if (milp_flag == -1) {
            cout << "当前问题无可行解！" << endl;
        }
        if (milp_flag >= 1) {
            cout << "找到最优解！" << endl;
            cout << "最优值: " << milp_flag << endl;
            arcs_to_path(g, toy.s, toy.t, a_flag, u_flag, p);

            write_solution(toy, p, file_name + "_3");
            p.clear();
            read_solution(toy, p, file_name + "_3");
            read_solution(toy, p, file_name);
            calculate_solution_information(toy, sol);
            draw_solution(toy, a_flag, u_flag, png_width, png_height, node_size,
                          file_name + "_3");
        }
        cout << "松弛路长约束" << endl;
        toy.targetNodePunishment = -1;
        toy.targetEdgePunishment = -1;
        toy.forbiddenEdgePunishment = -1;
        toy.pathNodeLimitPunishment = 10;
        milp_flag = x_y_steiner(toy, a_flag, u_flag);
        if (milp_flag == -1) {
            cout << "当前问题无可行解！" << endl;
        }
        if (milp_flag >= 1) {
            cout << "找到最优解！" << endl;
            cout << "最优值: " << milp_flag << endl;
            arcs_to_path(g, toy.s, toy.t, a_flag, u_flag, p);

            write_solution(toy, p, file_name + "_4");
            p.clear();
            read_solution(toy, p, file_name + "_4");
            read_solution(toy, p, file_name);
            calculate_solution_information(toy, sol);
            draw_solution(toy, a_flag, u_flag, png_width, png_height, node_size,
                          file_name + "_4");
        }
    }


    return 0;
}
