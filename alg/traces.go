package alg

import (
	"fmt"

	gl "github.com/ahmetb/go-linq"
	"github.com/ori-saporta83/linearizability-testing/graphtools"
)

const maxLevels = 10

//CheckAllTraces return all possible minimal failed traces
func CheckAllTraces(opList []Op) []*Relation {
	err := Init(opList)
	if err != nil {
		return nil
	}

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
			rc := r.GetCopy()
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
					r := sft[i].GetCopy()
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

//Init data sets
func Init(opList []Op) error {
	var err error
	ops = opList
	matches, pairs, bottoms, err = findMatches(ops)
	return err
}

//CheckAllTraces2 oops
func CheckAllTraces2(opList []Op) []Relation {
	err := Init(opList)
	if err != nil {
		fmt.Println(err)
		return nil
	}
	badTraces = []Relation{}
	l := len(ops)
	CheckTracesWithBase(*InitRelation(l), 0)
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
		if trace.IsNormal() {
			filteredBadTraces = append(filteredBadTraces, trace)
		}
	}
	return filteredBadTraces
}

//CheckTracesWithBase return all possible minimal failed traces
func CheckTracesWithBase(base Relation, level int) {
	l := len(ops)
	level++
	//TODO: what is the maximum # of edges in a disconnected DAG???
	if level == maxLevels {
		return
	}
	result := graphtools.InitGraph(l)
	for i := 0; i < l; i++ {
		for j := 0; j < l; j++ {
			currTrace := base.GetCopy()
			if i == j || currTrace.Contains(i, j) || currTrace.Contains(j, i) {
				result[i][j] = 0
				continue
			}
			contains := false
			currTrace.Append(i, j)
			for _, ft := range badTraces {
				copy := currTrace.GetCopy()
				copy.addTransitiveRelation()
				if copy.contains(ft) {
					contains = true
					break
				}
			}
			if contains || currTrace.Connected() {
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
				baseCopy := base.GetCopy()
				baseCopy.Append(i, j)
				CheckTracesWithBase(baseCopy, level)
			}
		}
	}
}
