#ifndef CONN_H
#define CONN_H


#include "network.h"
#include "routing.h"

//Connection management
class Connections{

    private:
        Network network;
        Routing& routing;
        
        struct ConnectionReq{
            int id, from, to;
            double bmin, bavg, bmax;
            //helper
            void print(){
                std::cout << id << " " << from << " " << to << " " << bmin << " " << bavg << " "
                 << bmax << "\n";
            }
        };
        struct ConnectionTableEntry{
            int id, src, dest;
            std::vector<int> label_list;
            double cost;
        };

        struct ForwdTableEntry{
            int inter_in, label_in, inter_out, label_out;
        };
        std::vector<ConnectionReq> conn_reqs;
        std::unordered_map<int, std::unordered_map<int, Link>> links;
        //label generation
        std::vector< std::vector<ForwdTableEntry>> forwardingTables;
        std::vector<int> labels;
        //Final connnection table
        std::vector<ConnectionTableEntry> connection_table;

    public:
        Connections(Network& net, Routing& routing, const std::string& path, bool optimistic):
        network(net), routing(routing), links(net.links), forwardingTables(net.no_nodes()),
        labels(net.no_nodes(), 0)
        {
            //read connections requests
            std::ifstream file(path);
            if(file.is_open()){
               
                int n_conn;
                file >> n_conn;
                for(int i=0; i<n_conn; i++){
                    int frm, to;
                    double bmin, bavg, bmax;
                    file >> frm >> to >> bmin >> bavg >> bmax;
                    conn_reqs.push_back({i, frm, to, bmin, bavg, bmax});
                }
                file.close();
                for(auto& conn: conn_reqs)
                    check_req(conn,optimistic);
            }
            else{
                logger(path + " NOT FOUND", LOG_LEVEL::ERROR);
            }
        }

        std::vector<int> assign_labels(Path& path){
            //assigne MPLS labels along a path
            //iterate over path
            std::vector<int> res;
            //first node
            int node = path.nodes[0];
            forwardingTables[node].push_back({-1, -1, path.nodes[1], labels[node]++});
            res.push_back(labels[node]-1);

            for(int i=1; i<(int)path.nodes.size()-1; i++){
                
                int u = path.nodes[i-1], v = path.nodes[i], w = path.nodes[i+1];
                forwardingTables[v].push_back({u, labels[node]-1, w, labels[v]++});
                res.push_back(labels[v]-1);
            }

            //ending node
            int nodes = path.nodes.size();
            int prev = path.nodes[nodes-2];
            forwardingTables[path.nodes.back()].push_back({prev, labels[prev]-1, -1, -1});
            return res;
        }
        
        bool check_req(ConnectionReq& req, bool optimistic){
            
            //return false if nodes not in network
            if(req.from > network.no_nodes() || req.to > network.no_nodes())
                return false;
            //get the disjoint routes
            Route& route = routing.lookup(req.from, req.to);
            //calc req.
            double req_val=0;
            if(optimistic){
                 req_val = std::min(req.bmax,req.bavg + 0.25*(req.bmax-req.bmin));
            }
            else{
                req_val = req.bmax;
            }
            double cost=0;
            std::vector<int> labels;
            //check path1
            if(check_path(route.path1, req_val)){
                //assign to path1
                assign_path(route.path1, req_val);
                labels = assign_labels(route.path1);
                cost = route.path1.cost;
            }
            else if(check_path(route.path2, req_val)){
                //assign to path2
                assign_path(route.path2, req_val);
                labels = assign_labels(route.path2);
                cost = route.path2.cost;
            }
            else{
                //deny req
                return false;
            }
            //assigned successfully
            connection_table.push_back({req.id,req.from, req.to,labels, cost});
            return true;
        }

        bool check_path(Path& path, double req){
            if(path.nodes.size() == 0) return false;
            int fine = true;
            for(int i=0; i<(int)path.nodes.size()-1; i++){
                int u = path.nodes[i], v = path.nodes[i+1];
                if(links[u][v].bw < req){
                    fine = false;
                    break;
                }
            }
            return fine;
        }
        void assign_path(Path& path, double req){
            for(int i=0; i<(int)path.nodes.size()-1; i++){
                int u = path.nodes[i], v = path.nodes[i+1];
                links[u][v].bw -= req;
                links[v][u].bw -= req;
            }

        }

        //helper
        void printConnections(){
            for(auto& conn: connection_table){
                std::cout << conn.id << " " <<
                conn.src << " " <<
                conn.dest << " " <<
                conn.cost << " ";
                for(int x: conn.label_list){
                    std::cout << x << ":";
                }
                std::cout << "\n";
            }
        }

        void writeConnectionsToFile(const std::string& path){
            std::ofstream file(path);
            if(file.is_open()){
                for(auto& conn: connection_table){
                    file << conn.id << " ";;
                    file << conn.src << " ";;
                    file << conn.dest << " ";;
                    for(int i=0; i<(int)conn.label_list.size()-1; i++)
                        file << conn.label_list[i] << "-";
                    file << conn.label_list.back() << " ";
                    file << conn.cost << "\n";;
        
                }
                file.close();
            }
            else{
                logger(path + " COULD NOT BE OPENED", LOG_LEVEL::ERROR);
            }
        }

        void writeForwardingTableToFIle(const std::string& path){
            std::ofstream file(path);
            int node =0;
            if(file.is_open()){
                for(auto& fw: forwardingTables){
                    file <<"Node:" << node++ << "\n";

                    for(auto& fwe: fw){
                        file << fwe.inter_in << " ";;
                        file << fwe.label_in<< " ";;
                        file << fwe.inter_out << " ";;
                        file << fwe.label_out << "\n";;
                    }
        
                }
                file.close();
            }
            else{
                logger(path + " COULD NOT BE OPENED", LOG_LEVEL::ERROR);
            }
        }

        void writePathsFile(const std::string& path){
            std::ofstream file(path);

            if(file.is_open()){
                file << conn_reqs.size() << " "  << connection_table.size();
                file.close();
            }
            else{
                logger(path + " FILE COULD NOT BE OPENED", LOG_LEVEL::ERROR);
            }
        }
};

#endif