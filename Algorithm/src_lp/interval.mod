/* COLOR, Graph Coloring Problem */

/* Written in GNU MathProg by Andrew Makhorin <mao@gnu.org> */

/* Given an undirected loopless graph G = (V, E), where V is a set of
   nodes, E <= V x V is a set of arcs, the Graph Coloring Problem is to
   find a mapping (coloring) F: V -> C, where C = {1, 2, ... } is a set
   of colors whose cardinality is as small as possible, such that
   F(i) != F(j) for every arc (i,j) in E, that is adjacent nodes must
   be assigned different colors. */

param n, integer, >= 2;
/* number of nodes */

set V := {1..n};
/* set of nodes */

param nc := 3;

set NC := {1..nc};

set E, within V cross V;
/* set of arcs */

set ALLE, within V cross NC;

check{(i,j) in E}: i != j;
/* there must be no loops */

/* We need to estimate an upper bound of the number of colors |C|.
   The number of nodes |V| can be used, however, for sparse graphs such
   bound is not very good. To obtain a more suitable estimation we use
   an easy "greedy" heuristic. Let nodes 1, ..., i-1 are already
   assigned some colors. To assign a color to node i we see if there is
   an existing color not used for coloring nodes adjacent to node i. If
   so, we use this color, otherwise we introduce a new color. */

set EE := setof{(i,j) in E} (i,j) union setof{(i,j) in E} (j,i);
/* symmetrisized set of arcs */

/* number of colors used by the heuristic; obviously, it is an upper
   bound of the optimal solution */

display nc;

var x{i in V, c in 1..nc}, binary;
/* x[i,c] = 1 means that node i is assigned color c */

s.t. map{i in V}: sum{c in 1..nc} x[i,c] <= 1;
/* each node must be assigned exactly one color */

s.t. arc{(i,j) in E, c in 1..nc}: x[i,c] + x[j,c] <= 1;
/* adjacent nodes cannot be assigned the same color */

maximize obj: sum{(i, j) in ALLE} x[i,j];
/* objective is to minimize the number of colors used */

data;

/* These data correspond to the instance myciel3.col from:
   http://mat.gsia.cmu.edu/COLOR/instances.html */

/* The optimal solution is 4 */

param n := 4;

set E :=
 1 2
 1 3
 1 4
 2 3
;

set ALLE :=
 1 1
 1 2
 1 3
 2 1
 2 2
 2 3
 3 1
 3 2
 4 1
 4 2
 4 3
 ;

end;
