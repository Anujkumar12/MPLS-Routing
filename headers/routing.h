#ifndef ROUTE_H
#define ROUTE_H

#include <iostream>
#include<vector>
#include <algorithm>
#include <limits>
#include <sstream>

#include "network.h"
#include "utils.h"


struct Path{
    std::vector<int> nodes;
    int cost;
    void print(){
       for(int i=0; i<(int)nodes.size()-1; i++) std::cout << nodes[i] << "->";
       if(nodes.size()>0)std::cout << nodes.back() << " " << cost << "\n";
    }

    std::string getString(){
        std::stringstream ss;
        for(int i=0; i<(int)nodes.size()-1; i++) ss << nodes[i] << "-";
        if(nodes.size()>0)ss << nodes.back();
        return ss.str();
    }
};

struct Route{
    int from, to;
    Path path1, path2;
};


class Routing{
    
    public:
        Network& network;
        Network network_copy;
        std::vector<std::vector<Route>> routing_table;
        
        Routing(Network& network): network(network), network_copy(network){
            int n_nodes = network.no_nodes();
            routing_table = std::vector<std::vector<Route>>(n_nodes, std::vector<Route>(n_nodes));

            for(int i=0; i<n_nodes; i++){
                std::vector<Path> paths = dijkstra_all(network, i);
                for(int j=0; j<n_nodes; j++){
                    if( i != j){
                        auto dis_paths = link_disjoint_paths(paths, i, j);
                        routing_table[i][j] = {i, j, dis_paths.first, dis_paths.second};
                    }
                }
            }

        }
        
        void printRoutingTable(){
            int n = network.no_nodes();
            for(int i = 0; i<n; i++){
                
                for(int j=0; j<n; j++){
                    if(i == j) continue;
                    Route& route = routing_table[i][j];
                    std::cout <<route.from << " " << route.to << " ";
                    route.path1.print();
                    std::cout << route.from << " "  << route.to << " ";
                    route.path2.print();
                }
            }
        }

        Route& lookup(int from, int to){
            if(from>= network.no_nodes() || to>=network.no_nodes()) 
                logger("LOOKUP INVALID", LOG_LEVEL::ERROR);
            return routing_table[from][to];
        }

        std::vector<Path> dijkstra_all(Network& network, int from){
            

            int nodes = network.no_nodes();
            const int MAX_DIST = std::numeric_limits<int>::max();
            std::vector<int> parents(nodes, -1), distance(nodes, MAX_DIST);
            std::vector<bool> visited(nodes, false);
            MinHeap<int, double> mh;
            mh.insert(from, 0);
            for(int i=0; i<nodes; i++){
                if(i != from) mh.insert(i, MAX_DIST);
            }
            distance[from] = 0;

            while(!mh.empty()){
                // int min_node = -1, min_distance= MAX_DIST;
                // for(int i=0; i<nodes; i++){
                //     if(visited[i] == false && distance[i] < min_distance){
                //         min_distance = distance[i];
                //         min_node = i;
                //     }
                // }
                
                int min_node = mh.getMin().first;
                mh.pop();
                if(distance[min_node] == MAX_DIST) break;
                // if(min_node == -1) break;

                visited[min_node] = true;
                for(auto& p: network.links[min_node]){
                    if( !visited[p.first] && distance[min_node] + p.second.cost < distance[p.first]){
                        distance[p.first] = distance[min_node] + p.second.cost;
                        mh.decrease_key(p.first, distance[min_node] + p.second.cost);
                        parents[p.first] = min_node;
                    }
                }
        }

        std::vector<Path> paths(nodes);
        for(int i=0; i<nodes; i++){
            if(distance[i] != MAX_DIST){
                int n = i;
                paths[n].cost = distance[i];
                while(n != -1){
                    paths[i].nodes.push_back(n);
                    n = parents[n];
                }
                reverse(paths[i].nodes.begin(), paths[i].nodes.end());
            }

            else{
                paths[i].cost = MAX_DIST;
            }
        }
        return paths;

    }

        Path dijkstra_single(Network& network, int from, int to){

            int nodes = network.no_nodes();
            const int MAX_DIST = std::numeric_limits<int>::max();
            std::vector<int> parents(nodes, -1), distance(nodes, MAX_DIST);
            std::vector<bool> visited(nodes, false);
            MinHeap<int, double> mh;
            mh.insert(from, 0);
            for(int i=0; i<nodes; i++){
                if(i != from) mh.insert(i, MAX_DIST);
            }

            distance[from] = 0;
            while(!mh.empty()){
                // int min_node = -1, min_distance= MAX_DIST;
                // for(int i=0; i<nodes; i++){
                //     if(visited[i] == false && distance[i] < min_distance){
                //         min_distance = distance[i];
                //         min_node = i;
                //     }
                // }
                //if(min_node == -1 || min_node == to) break;
                int min_node = mh.getMin().first;
                mh.pop();
                if(min_node == to || distance[min_node] == MAX_DIST) break;

                visited[min_node] = true;
                for(auto& p: network.links[min_node]){
                    if( !visited[p.first] && distance[min_node] + p.second.cost < distance[p.first]){
                        distance[p.first] = distance[min_node] + p.second.cost;
                        mh.decrease_key(p.first, distance[min_node] + p.second.cost);
                        parents[p.first] = min_node;
                    }
                }
            }

            Path path;
            if(distance[to] != MAX_DIST){
                int n = to;
                path.cost = distance[to];
                while(n != -1){
                    path.nodes.push_back(n);
                    n = parents[n];
                }
                std::reverse(path.nodes.begin(), path.nodes.end());
            }
            return path;
    }

        std::pair<Path, Path> link_disjoint_paths(std::vector<Path>& paths, int frm, int to){
            Path& path1 = paths[to];
            for(auto& p1: network.links){
                for(auto& p2: p1.second){
                    int u = p1.first, v = p2.first;
                    network_copy.links[u][v] = network.links[u][v];
                }
            }
            for(int i=0; i<(int)path1.nodes.size()-1; i++){
                int u = path1.nodes[i], v = path1.nodes[i+1];
                network_copy.remove_link(u, v);
                network_copy.remove_link(v, u);
            }
            Path path2 = dijkstra_single(network_copy, frm, to);
            return {path1, path2};
        }

        void writeRoutingTable(const std::string& path){
            std::ofstream file(path);
            if(file.is_open()){
                int node = 0;
                for(auto& routes: routing_table){
                    file << "Node:" << node++ << "\n";
                    for(auto& route: routes){
                        if(route.from == route.to) continue;

                        file << route.to << " " << route.path1.getString() << " " << route.path1.cost << "\n";
                        file  << route.to << " " << route.path1.getString() << " " << route.path2.cost << "\n";                        
                    }
                }
                file.close();
            }
            else{
                logger(path + " COULD NOT BE OPENED", LOG_LEVEL::ERROR);
            }

            
        }
};

#endif