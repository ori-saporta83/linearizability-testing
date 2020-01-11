package graphtools

//Graph is a matrix representation of a directional graph
type Graph [][]int

//TransitiveClosure returns the reachability matrix of the given graph
func (graph Graph) TransitiveClosure() Graph {
	V := len(graph)
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
			reach[i][j] = graph[i][j]
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
func (graph Graph) Equals(other Graph) bool {
	if len(graph) != len(other) {
		return false
	}
	for i := 0; i < len(graph); i++ {
		if len(graph[i]) != len(other[i]) {
			return false
		}
		for j := 0; j < len(graph[i]); j++ {
			if graph[i][j] != other[i][j] {
				return false
			}
		}
	}
	return true
}

//Clone returns a copy of the graph
func (graph Graph) Clone() Graph {
	duplicate := InitGraph(len(graph))
	for i := range graph {
		duplicate[i] = make([]int, len(graph[i]))
		copy(duplicate[i], graph[i])
	}
	return duplicate
}
