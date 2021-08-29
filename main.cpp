#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <limits>
#include <algorithm>
#include <cstring>

#include "network.h"
#include "routing.h"
#include "connections.h"


int main(int argc, char** argv){


    std::string topfile,connfile, rtfile, ftfile, pathfile;
    int flag_dist=-1, flag_pess=-1;

    for(int i=1; i<argc; i++){
        if(argv[i][0] == '-'){
            //new flag
            if(strcmp("top", argv[i]+1) == 0 && i+1<argc){
                //topology
                topfile = std::string(argv[i+1]);
            }
            else if(strcmp("conn", argv[i]+1) == 0 && i+1<argc){
                connfile = std::string(argv[i+1]);
            }
            else if(strcmp("rt", argv[i]+1) == 0 && i+1<argc){
                rtfile = std::string(argv[i+1]);
            }
            else if(strcmp("ft", argv[i]+1) == 0 && i+1<argc){
                ftfile = std::string(argv[i+1]);

            }
            else if(strcmp("path", argv[i]+1) == 0 && i+1<argc){
                pathfile = std::string(argv[i+1]);

            }
            else if(strcmp("flag", argv[i]+1) == 0 && i+1<argc){
                if(strcmp(argv[i+1], "hop") == 0)
                    flag_dist = 0;
                else flag_dist = 1;
            }
            else if(strcmp("p", argv[i]+1) == 0 && i+1<argc){
                flag_pess = (argv[i+1][0] == '1')?1:0;
            }
        }
    }
    //check arguements are in
    if(connfile == "" || topfile == "" || rtfile == "" || ftfile == "" || pathfile=="" || flag_pess==-1 || flag_dist == -1){
        std::cout << "\n\n";
        logger("INVALID OR MISSING ARGUMENTS", LOG_LEVEL::ERROR);
        logger("USAGE::\n./routing -top  <topology file> -conn  <connectionsfile> -rt  <routingtablefile> -ft  <forwardingtablefile> -path <pathsfile> -flag hop|dist -p 0|1", LOG_LEVEL::INFO);
        return -1;
    }

     Network ntwk;
     ntwk.readNetwork(topfile, flag_dist);
     Routing route(ntwk);
     Connections conn(ntwk, route, connfile, !flag_pess);
     route.writeRoutingTable(rtfile);
     conn.writeConnectionsToFile("output.txt");
     conn.writeForwardingTableToFIle(ftfile);
     conn.writePathsFile(pathfile);
}