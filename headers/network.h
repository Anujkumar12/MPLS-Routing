#ifndef NETWORK_H
#define NETWORK_H

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <limits>
#include <algorithm>

#include "utils.h"

struct Link{
    double cost, bw;
};

class Network{

    public:
        std::unordered_map<int, std::unordered_map<int, Link>> links;
        int n_nodes;

        Network(int n_nodes=0): n_nodes(n_nodes){

        }

        void readNetwork(const std::string& filePath, bool metric_distance = true){
            int nodes, edges;
            std::ifstream file(filePath);
     

            if(file.is_open()){
                int nodes, edges;
               
                file >> nodes >> edges;
                n_nodes = nodes;
                for(int i=0; i<edges; i++){
                    int frm, to;
                    double cost, bw;
                    file >> frm >> to >> cost >> bw;
                    if(!metric_distance) cost = 1.0;
                    links[frm][to] = {cost, bw};
                    links[to][frm] = {cost, bw};
                }
                file.close();
            }
            else{
                logger(filePath + " NOT FOUND", LOG_LEVEL::ERROR);
            }
        }

        inline void add_link(int frm, int to, const Link& link){
            links[frm][to] = link;
        }
        inline void remove_link(int from, int to){
            if(links[from].count(to) != 0){
                links[from].erase(to);
                if(links[from].size() == 0)
                    links.erase(from);
            }
        }
        inline void update_link(int frm, int to, const Link& link){
            links[frm][to] = link;
        }
        inline Link get_link(int frm, int to){
            if(links.count(frm) != 0 && links[frm].count(to) != 0)
                return links[frm][to];
            else{
                logger("LINK NOT FOUND", LOG_LEVEL::ERROR);
                return {0, 0};
            }
        }
        inline bool node_exists(int node){
            return node < n_nodes;
        }
        inline bool link_exists(int frm, int to){
            return links.count(frm) !=0 && links[frm].count(to) != 0;
        }
        inline int no_nodes(){
            return n_nodes;
        }
        //helper
        void printNetwork(){
            std::cout << "Nodes:" << n_nodes << "\n";
            int edges=0;
            for(auto& p1: links){
                for(auto& p2: p1.second){
                    edges++;
                    Link& link = p2.second;
                    std::cout << p1.first << " " << p2.first << " " << link.cost << " " << link.bw << "\n";
                }
            }
            std::cout << "Edges: " << edges << "\n";
        }
};

#endif