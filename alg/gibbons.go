package alg

import (
	"fmt"
	"strings"

	"github.com/ori-saporta83/linearizability-testing/graphtools"
	"github.com/pkg/errors"
)

//BOTTOM is the result of deq when the queue is empty
const BOTTOM = -1

//OpType possible queue operations
type OpType int

//queue operations
const (
	ENQ OpType = iota
	DEQ
)

//Op Single Queue Op on Val
type Op struct {
	OpType OpType
	Val    int
}

//Relation a set of pairs of indexes
type Relation struct {
	graph graphtools.Graph
}

//InitRelation returns new relation with given capacity
func InitRelation(opsLen int) *Relation {
	return &Relation{graph: graphtools.InitGraph(opsLen)}
}

//Append the given pair into the relation
func (r *Relation) Append(a, b int) {
	r.graph[a][b] = 1
}

//merge append all other pairs into the relation
func (r *Relation) merge(other *Relation) {
	for i := 0; i < len(other.graph); i++ {
		for j := 0; j < len(other.graph); j++ {
			if other.graph[i][j] == 1 {
				r.Append(i, j)
			}
		}
	}
}

type pair struct {
	enqIdx int
	deqIdx int
}

func (r *Relation) String() string {
	sb := strings.Builder{}
	sb.WriteString("[")
	for i := 0; i < len(r.graph); i++ {
		for j := 0; j < len(r.graph); j++ {
			if r.graph[i][j] > 0 {
				sb.WriteString(fmt.Sprintf("<%d,%d>", i, j))
			}
		}
	}
	sb.WriteString("]")
	return sb.String()
}

func (r *Relation) enlargePairs(pairs []pair) {
	for i := 0; i < len(pairs); i++ {
		for j := 0; j < len(pairs); j++ {
			if i == j {
				continue
			}
			p1 := pairs[i]
			p2 := pairs[j]
			if r.graph[p1.enqIdx][p2.enqIdx] != 0 {
				r.graph[p1.deqIdx][p2.deqIdx] = 1
			}
			if r.graph[p1.deqIdx][p2.deqIdx] == 1 {
				r.graph[p1.enqIdx][p2.enqIdx] = 1
			}
		}
	}
}

func (r *Relation) enlargeBottoms(bottoms []int, pairs []pair) {
	for _, bottomIdx := range bottoms {
		for _, pair := range pairs {
			if r.graph[bottomIdx][pair.deqIdx] == 1 {
				r.graph[bottomIdx][pair.deqIdx] = 1
			}
			if r.graph[pair.enqIdx][bottomIdx] == 1 {
				r.graph[pair.deqIdx][bottomIdx] = 1
			}
		}
	}
}

func (r *Relation) addTransitiveRelation() {
	r.graph = r.graph.TransitiveClosure()
}

func (r *Relation) getCopy() Relation {
	duplicate := Relation{}
	duplicate.graph = r.graph.Clone()
	return duplicate
}

func (r *Relation) equals(other *Relation) bool {
	return r.graph.Equals(other.graph)
}

func (r Relation) isCyclic() bool {
	for i := 0; i < len(r.graph); i++ {
		if r.graph[i][i] != 0 {
			return true
		}
	}
	return false
}

func (r Relation) contains(other Relation) bool {
	return r.graph.Contains(other.graph)
}

func findMatches(ops []Op) (*Relation, []pair, []int, error) {
	result := InitRelation(len(ops))

	pairs := []pair{}
	bottoms := []int{}
	pushes := map[int]int{}
	pops := map[int]int{}
	for i := range ops {
		if ops[i].Val == BOTTOM {
			bottoms = append(bottoms, i)
		} else if ops[i].OpType == ENQ {
			pushes[ops[i].Val] = i
		} else {
			pops[ops[i].Val] = i
		}
	}
	if len(pushes) != len(pops) {
		return nil, nil, nil, errors.Errorf("Not all pairs match")
	}
	for val, enq := range pushes {
		deq, ok := pops[val]
		if !ok {
			return nil, nil, nil, errors.Errorf("Can't find pair for %d", val)
		}
		result.Append(enq, deq)
		pairs = append(pairs, pair{enqIdx: enq, deqIdx: deq})
	}
	return result, pairs, bottoms, nil
}

//CheckTrace return true if the given trace represents a valid queue trace according to Gibbons D algorithm
func CheckTrace(ops []Op, o *Relation) bool {
	//if there's pop(nil) it should be first or last
	order := o.getCopy()
	matches, pairs, bottoms, err := findMatches(ops)
	if err != nil {
		return false
	}

	order.merge(matches)

	before := order.getCopy()

	for {
		order.enlargePairs(pairs)
		order.enlargeBottoms(bottoms, pairs)
		order.addTransitiveRelation()

		if before.equals(&order) {
			break
		} else {
			before = order.getCopy()
		}
	}

	return !order.isCyclic()
}
