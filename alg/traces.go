package alg

import (
	gl "github.com/ahmetb/go-linq"
	"github.com/ori-saporta83/linearizability-testing/graphtools"
)

//CheckAllTraces return all possible minimal failed traces
func CheckAllTraces(ops []Op) []*Relation {
	l := len(ops)
	queue := []*Relation{}
	for i := 0; i < l; i++ {
		for j := 0; j < l; j++ {
			if i == j {
				continue
			}
			r := InitRelation(l)
			r.Append(i, j)
			queue = append(queue, r)
		}
	}
	failedTraces := []*Relation{}
	successfulTraces := map[string]*Relation{}
	for {
		for _, r := range queue {
			rc := r.getCopy()
			rc.addTransitiveRelation()
			if rc.isCyclic() {
				continue
			}
			contains := false
			for _, ft := range failedTraces {
				if rc.contains(*ft) {
					contains = true
					break
				}
			}
			if contains {
				continue
			}
			if CheckTrace(ops, r) {
				successfulTraces[r.String()] = r
			} else {
				failedTraces = append(failedTraces, r)
			}
		}
		queue = nil
		if len(successfulTraces) > 0 {
			sft := []*Relation{}
			gl.From(successfulTraces).SelectT(func(x gl.KeyValue) *Relation { return x.Value.(*Relation) }).ToSlice(&sft)
			for i := 0; i < len(sft); i++ {
				for j := i + 1; j < len(sft); j++ {
					r := sft[i].getCopy()
					r.merge(sft[j])
					queue = append(queue, &r)
				}
			}
			successfulTraces = map[string]*Relation{}
		} else {
			break
		}
	}
	return failedTraces
}

var badTraces = []Relation{}

//CheckAllTraces2 oops
func CheckAllTraces2(ops []Op) []Relation {
	badTraces = []Relation{}
	l := len(ops)
	CheckTracesWithBase(ops, *InitRelation(l), 1)
	filteredBadTraces := []Relation{}
	for i, trace := range badTraces {
		contains := false
		for j, otherTrace := range badTraces {
			if i != j && trace.contains(otherTrace) {
				contains = true
				break
			}
		}
		if contains {
			continue
		}
		filteredBadTraces = append(filteredBadTraces, trace)
	}
	return filteredBadTraces
}

//CheckTracesWithBase return all possible minimal failed traces
func CheckTracesWithBase(ops []Op, base Relation, level int) {
	l := len(ops)
	level++
	if level == l {
		return
	}
	result := graphtools.InitGraph(l)
	for i := 0; i < l; i++ {
		for j := 0; j < l; j++ {
			currTrace := base.getCopy()
			if i == j || currTrace.Contains(i, j) || currTrace.Contains(j, i) {
				result[i][j] = 0
				continue
			}
			contains := false
			currTrace.Append(i, j)
			for _, ft := range badTraces {
				copy := currTrace.getCopy()
				copy.addTransitiveRelation()
				if copy.contains(ft) {
					contains = true
					break
				}
			}
			if contains {
				result[i][j] = 0
				continue
			}
			if CheckTrace(ops, &currTrace) {
				result[i][j] = 1
			} else {
				result[i][j] = 0
				badTraces = append(badTraces, currTrace)
			}
		}
	}
	for i := 0; i < l; i++ {
		for j := 0; j < l; j++ {
			if result[i][j] == 1 {
				baseCopy := base.getCopy()
				baseCopy.Append(i, j)
				CheckTracesWithBase(ops, baseCopy, level)
			}
		}
	}
}
