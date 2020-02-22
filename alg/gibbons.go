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

//Remove the given pair from the relation
func (r *Relation) Remove(a, b int) {
	r.graph[a][b] = 0
}

//Contains return if the given pair is in the relation
func (r *Relation) Contains(a, b int) bool {
	return r.graph[a][b] == 1
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

func (r Relation) String() string {
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

func (r *Relation) enlargePairs(pairs map[int]pair) {
	for k1 := range pairs {
		for k2 := range pairs {
			if k1 == k2 {
				continue
			}
			p1 := pairs[k1]
			p2 := pairs[k2]
			if r.graph[p1.enqIdx][p2.enqIdx] != 0 {
				r.graph[p1.deqIdx][p2.deqIdx] = 1
			}
			if r.graph[p1.deqIdx][p2.deqIdx] == 1 {
				r.graph[p1.enqIdx][p2.enqIdx] = 1
			}
		}
	}
}

func (r *Relation) enlargeBottoms(bottoms []int, pairs map[int]pair) {
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

//GetCopy of relation
func (r *Relation) GetCopy() Relation {
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

func findMatches(ops []Op) (*Relation, map[int]pair, []int, error) {
	result := InitRelation(len(ops))

	pairs := map[int]pair{}
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
		pairs[val] = pair{enqIdx: enq, deqIdx: deq}
	}
	return result, pairs, bottoms, nil
}

var matches *Relation
var pairs map[int]pair
var bottoms []int
var ops []Op

//CheckTrace return true if the given trace represents a valid queue trace according to Gibbons D algorithm
func CheckTrace(ops []Op, o *Relation) bool {
	order := o.GetCopy()

	order.merge(matches)

	before := order.GetCopy()

	for {
		order.enlargePairs(pairs)
		order.enlargeBottoms(bottoms, pairs)
		order.addTransitiveRelation()

		if before.equals(&order) {
			break
		} else {
			before = order.GetCopy()
		}
	}

	return !order.isCyclic()
}

//IsNormal returns true if r is in normal form
func (r *Relation) IsNormal() bool {
	vmap := map[int]int{}
	bottomMap := map[int]int{}
	valCount := 0
	bottomCount := 0

	for i := range r.graph {
		for j := range r.graph[i] {
			if r.graph[i][j] == 1 {
				a := getNormalIndex(i, vmap, bottomMap, &valCount, &bottomCount)
				b := getNormalIndex(j, vmap, bottomMap, &valCount, &bottomCount)
				if a != i || b != j {
					return false
				}
			}
		}
	}
	return true
}

//Normalize set r to normal form
func (r *Relation) Normalize() *Relation {
	nr := InitRelation(len(ops))
	vmap := map[int]int{}
	bottomMap := map[int]int{}
	valCount := 0
	bottomCount := 0

	for i := range r.graph {
		for j := range r.graph[i] {
			if r.graph[i][j] == 1 {
				nr.Append(getNormalIndex(i, vmap, bottomMap, &valCount, &bottomCount),
					getNormalIndex(j, vmap, bottomMap, &valCount, &bottomCount))
			}
		}
	}
	return nr
}

func getNormalIndex(baseIndex int, vmap, bottomMap map[int]int, valCount, bottomCount *int) (index int) {
	v := ops[baseIndex].Val
	if v == BOTTOM {
		if _, ok := bottomMap[baseIndex]; !ok {
			*bottomCount++
			bottomMap[baseIndex] = *bottomCount
		}
		index = bottoms[bottomMap[baseIndex]-1]
	} else {
		if _, ok := vmap[v]; !ok {
			*valCount++
			vmap[v] = *valCount
		}
		if ops[baseIndex].OpType == ENQ {
			index = pairs[vmap[v]].enqIdx
		} else {
			index = pairs[vmap[v]].deqIdx
		}
	}
	return
}
