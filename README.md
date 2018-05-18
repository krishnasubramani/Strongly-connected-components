# Strongly-connected-components
Kosaraju's two-pass algorithm to find the strongly connected components in a directed graph


Input: SCC.txt file that contains the edges of the directed graph in the following format.

1 1 
1 2 
1 5 
1 6 
1 7 
1 3 
1 8 
1 4 
2 47646 
2 47647 
2 13019 
2 47648 
2 47649 
2 47650 
2 7700 
2 47651 
2 47652 
3 511596 
5 1 
5 9 
5 10 
5 11 
5 12 
5 13 
5 14 
5 15 
and so on..

In the input file, every row indicates an edge. The vertex label in the first column is the tail and the vertex label in the second column is the head. The entry "2 47646" in the above example means that the vertex with label 2 has an outgoing edge to the vertex with label 47646.

Output: The sizes of the first five largest SCCs in the given graph, in decreasing order of sizes, separated by commas. If the algorithm finds less than five SCCs in the given graph, then the number zero is printed for the remaining terms.
