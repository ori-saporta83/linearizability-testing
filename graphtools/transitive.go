package graphtools

import (
	"github.com/yourbasic/graph"
)

//Graph is a matrix representation of a directional graph
type Graph [][]int

//TransitiveClosure returns the reachability matrix of the given graph
func (g Graph) TransitiveClosure() Graph {
	V := len(g)
	/* reach[][] will be the output matrix that will finally
	   have the shortest  distances between every pair of
	   vertices */
	reach := InitGraph(V)
	var i, j, k int

	/* Initialize the solution matrix same as input graph
	   matrix. Or  we can say the initial values of shortest
	   distances are based  on shortest paths considering
	   no intermediate vertex. */
	for i = 0; i < V; i++ {
		for j = 0; j < V; j++ {
			reach[i][j] = g[i][j]
		}
	}

	/* Add all vertices one by one to the set of intermediate
	   vertices.
	  ---> Before start of a iteration, we have reachability
		   values for all  pairs of vertices such that the
		   reachability values consider only the vertices in
		   set {0, 1, 2, .. k-1} as intermediate vertices.
	  ----> After the end of a iteration, vertex no. k is
			added to the set of intermediate vertices and the
			set becomes {0, 1, 2, .. k} */
	for k = 0; k < V; k++ {
		// Pick all vertices as source one by one
		for i = 0; i < V; i++ {
			// Pick all vertices as destination for the
			// above picked source
			for j = 0; j < V; j++ {
				// If vertex k is on a path from i to j,
				// then make sure that the value of reach[i][j] is 1
				val := 0
				if (reach[i][j] != 0) || ((reach[i][k] != 0) && (reach[k][j] != 0)) {
					val = 1
				}
				reach[i][j] = val
			}
		}
	}

	return reach
}

//InitGraph Inits an NxN 2d slice
func InitGraph(N int) Graph {
	grid := make([][]int, N)
	for i := 0; i < N; i++ {
		grid[i] = make([]int, N)
	}
	return grid
}

//Equals returns true iff graph and other are identical
func (g Graph) Equals(other Graph) bool {
	if len(g) != len(other) {
		return false
	}
	for i := 0; i < len(g); i++ {
		if len(g[i]) != len(other[i]) {
			return false
		}
		for j := 0; j < len(g[i]); j++ {
			if g[i][j] != other[i][j] {
				return false
			}
		}
	}
	return true
}

//SwitchLine switches between two lines in the graph
func (g Graph) SwitchLine(i, j int) {
	if i > len(g)-1 || j > len(g)-1 || i == j {
		return
	}
	sub := g[i]
	g[i] = g[j]
	g[j] = sub
}

//Contains returns true if all 1 values in other are contained in graph
func (g Graph) Contains(other Graph) bool {
	if len(g) != len(other) {
		return false
	}
	for i := 0; i < len(g); i++ {
		if len(g[i]) != len(other[i]) {
			return false
		}
		for j := 0; j < len(g[i]); j++ {
			if other[i][j] == 1 && g[i][j] != other[i][j] {
				return false
			}
		}
	}
	return true
}

//Clone returns a copy of the graph
func (g Graph) Clone() Graph {
	duplicate := InitGraph(len(g))
	for i := range g {
		duplicate[i] = make([]int, len(g[i]))
		copy(duplicate[i], g[i])
	}
	return duplicate
}

//Reverse returns the reversed copy of the graph
func (g Graph) Reverse() Graph {
	reversed := InitGraph(len(g))
	for i := 0; i < len(g); i++ {
		for j := 0; j < len(g); j++ {
			if g[i][j] == 1 {
				reversed[j][i] = 1
			}
		}
	}
	return reversed
}

//Order returns the # vertices
func (g Graph) Order() int {
	return len(g)
}

//Visit calls do for all neighbors of v and aborts with true if do returns skip == true
func (g Graph) Visit(v int, do func(w int, c int64) (skip bool)) (aborted bool) {
	for i := 0; i < len(g); i++ {
		if g[v][i] == 1 {
			if do(i, 0) {
				return true
			}
		}
	}
	return false
}

//Ordered returns true if the graph is ordered
func (g Graph) Ordered() bool {
	if g.Cyclic() {
		return false
	}
	transitive := g.TransitiveClosure()
	for i := 0; i < g.Order(); i++ {
		for j := i + 1; j < g.Order(); j++ {
			if transitive[i][j] == 0 && transitive[j][i] == 0 {
				return false
			}
		}
	}
	return true
}

//Cyclic returns true if the graph is cyclic
func (g Graph) Cyclic() bool {
	return !graph.Acyclic(g)
}
