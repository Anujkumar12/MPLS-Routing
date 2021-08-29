---------- MPLS Link Disjoint Routing implementation------------

----------USAGE---------
Using the Makefile

$ make

OR

$ g++ -std=c++11 -I ./headers main.cpp -o routing

This creates the *routing* executable, which can be run as:

$ ./routing −top <topology_file> −conn <connections_file> −rt <routing_table_file> −ft <forwarding_tablefile> −path <paths_file> −flag <hop|dist> −p <0|1>


There are test cases given already. Run as:

$ ./routing -top top14.doc -conn NSFNET_100.doc -rt routingfile.txt -ft forwarding.txt -path pathfile.txt -flag dist -p 0

$ ./routing -top top14.doc -conn NSFNET_100.doc -rt routingfile.txt -ft forwarding.txt -path pathfile.txt -flag dist -p 1

$ ./routing -top top14.doc -conn NSFNET_100.doc -rt routingfile.txt -ft forwarding.txt -path pathfile.txt -flag hop -p 0

$ ./routing -top top14.doc -conn NSFNET_100.doc -rt routingfile.txt -ft forwarding.txt -path pathfile.txt -flag hop -p 1



$ ./routing -top top24.doc -conn ARPANET_100.doc -rt routingfile.txt -ft forwarding.txt -path pathfile.txt -flag dist -p 0

$ ./routing -top top24.doc -conn ARPANET_100.doc -rt routingfile.txt -ft forwarding.txt -path pathfile.txt -flag dist -p 1

$ ./routing -top top24.doc -conn ARPANET_100.doc -rt routingfile.txt -ft forwarding.txt -path pathfile.txt -flag hop -p 0

$ ./routing -top top24.doc -conn ARPANET_100.doc -rt routingfile.txt -ft forwarding.txt -path pathfile.txt -flag hop -p 1

---OUTPUTS----

1. Routing Table: 
	- [ Destination_node || Path || Path Cost ]
	- Path is stored as : 1-2-3-4-5-6
	- Routing table for each node starts as **Node:x** where x is the node's number. Following the line, its routing table is prenset. To each node, 2 disjoint paths are printed on separate consecutive lines.

2. Forwarding table ( for each admitted connection):
	- [Interface in || Label in || Inteface out || Label out]
	- Forwarding table for a node begins with **Node:x** where x is the node's number.
	- For Label Edge Routers, correpsonding entries are defined -1.
	
3. Output file (for each admitted connection):
	- [Connection Id || Source || Destination || Label List || Path Cost]
	- Label list is represented as 1-2-3-4
	
4. Pathfile:
	- [Total_connections  Admitted Connections]

--------APPROACH USED-----------
------ Routing -------

The network is read into a adjacency list graph reperesentation. The Network is then represnted by an object of class Network.

For finding the disjoint paths, the following approach is used:
1.  Find the shortest path from a node to other node using Dijkstra's Algo. Let the path be P1.
2.  Create another graph by removing the edges of P1 from the network's graph. Now run Dijkstra's algo on this modified graph to find the second disjoint path.
3. This is done for every node with optimizations

-Depending on the metric, cost given in the inputs or hops are used for determining the shortest paths.

Code structure:
    -Network class: Represents the network as a graph. Resposible for reading the topology.
    -Routing class: Takes the network object and creates the routing table for every node by the method discussed above.
    
 ------ Forwarding-----
 Each router maintains a list of already used labels. Label counters start from 0. For each new connection established and the path chosen, labels are assigned and updated along the routers of the path.
 
 Both the optimistic and pessimistic approachs are available. For all the links along a path, the corresponding condition is checked and if satisified by all the links, the connection is accepted.
 
 Both shortest paths are checked. If none satisfy the needs, connection req. is rejected.
 
 Code structure:
    Connections Class: Takes the Network and Routing objects, and accepts/rejects the connection request given. Also forms and updates the forwarding table for an accepted connection.
 


