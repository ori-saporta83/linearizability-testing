package alg_test

import (
	"testing"

	. "github.com/ori-saporta83/linearizability-testing/alg"
	"github.com/stretchr/testify/assert"
)

func TestCheckTrace(t *testing.T) {
	ops1 := []Op{
		Op{OpType: ENQ, Val: 1},
		Op{OpType: DEQ, Val: 1},
		Op{OpType: DEQ, Val: BOTTOM},
	}
	r1Good := InitRelation(len(ops1))
	r1Good.Append(0, 1)
	r1Good.Append(2, 1)
	r1Bad1 := InitRelation(len(ops1))
	r1Bad1.Append(1, 0)
	r1Bad2 := InitRelation(len(ops1))
	r1Bad2.Append(2, 1)
	r1Bad2.Append(0, 2)
	ops2 := []Op{
		Op{OpType: ENQ, Val: 1},
		Op{OpType: DEQ, Val: 2},
		Op{OpType: DEQ, Val: BOTTOM},
	}
	ops3 := []Op{
		Op{OpType: ENQ, Val: 1},
		Op{OpType: DEQ, Val: 1},
		Op{OpType: ENQ, Val: 2},
		Op{OpType: DEQ, Val: 2},
	}
	r3Good := InitRelation(len(ops3))
	r3Good.Append(2, 1)
	r3Good.Append(3, 1)
	r3Bad := InitRelation(len(ops3))
	r3Bad.Append(1, 3)
	r3Bad.Append(2, 0)
	ops4 := []Op{
		Op{OpType: ENQ, Val: 1},
		Op{OpType: DEQ, Val: 1},
		Op{OpType: ENQ, Val: 2},
		Op{OpType: DEQ, Val: 2},
		Op{OpType: DEQ, Val: BOTTOM},
		Op{OpType: DEQ, Val: BOTTOM},
	}
	r4Good := InitRelation(len(ops4))
	r4Good.Append(4, 1)
	r4Good.Append(5, 2)
	r4Good.Append(2, 1)
	r4Bad1 := InitRelation(len(ops4))
	r4Bad1.Append(1, 0)
	r4Bad2 := InitRelation(len(ops4))
	r4Bad2.Append(4, 5)
	r4Bad2.Append(5, 3)
	r4Bad2.Append(2, 4)
	type args struct {
		ops   []Op
		order *Relation
	}
	tests := []struct {
		name string
		args args
		want bool
	}{
		{"t1", args{ops: ops1, order: InitRelation(len(ops1))}, true},
		{"t2", args{ops: ops2, order: InitRelation(len(ops2))}, false},
		{"t3", args{ops: ops3, order: InitRelation(len(ops3))}, true},
		{"t4", args{ops: ops4, order: InitRelation(len(ops4))}, true},
		{"t5", args{ops: ops1, order: r1Good}, true},
		{"t6", args{ops: ops1, order: r1Bad1}, false},
		{"t7", args{ops: ops1, order: r1Bad2}, false},
		{"t8", args{ops: ops3, order: r3Good}, true},
		{"t9", args{ops: ops3, order: r3Bad}, false},
		{"t10", args{ops: ops4, order: r4Good}, true},
		{"t11", args{ops: ops4, order: r4Bad1}, false},
		{"t12", args{ops: ops4, order: r4Bad2}, false},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			err := Init(tt.args.ops)
			if err != nil && !tt.want {
				return
			}
			if got := CheckTrace(tt.args.ops, tt.args.order); got != tt.want {
				t.Errorf("CheckTrace() = %v, want %v", got, tt.want)
			}
		})
	}
}

func TestRelation_Normalize(t *testing.T) {
	ops1 := []Op{
		Op{OpType: ENQ, Val: 1},
		Op{OpType: DEQ, Val: 1},
		Op{OpType: DEQ, Val: BOTTOM},
	}
	r1 := InitRelation(len(ops1))
	r1.Append(0, 1)
	r1.Append(2, 0)

	ops2 := []Op{
		Op{OpType: ENQ, Val: 1},
		Op{OpType: DEQ, Val: 1},
		Op{OpType: ENQ, Val: 2},
		Op{OpType: DEQ, Val: 2},
		Op{OpType: DEQ, Val: BOTTOM},
	}
	r2 := InitRelation(len(ops2))
	r2.Append(2, 3)
	r2.Append(3, 4)

	r2Want := InitRelation(len(ops2))
	r2Want.Append(0, 1)
	r2Want.Append(1, 4)

	ops3 := append(ops2, Op{OpType: DEQ, Val: BOTTOM})
	r3 := InitRelation(len(ops3))
	r3.Append(1, 2)
	r3.Append(0, 5)

	r3Want := InitRelation(len(ops3))
	r3Want.Append(1, 2)
	r3Want.Append(0, 4)

	ops4 := []Op{
		Op{OpType: ENQ, Val: 1},
		Op{OpType: DEQ, Val: 1},
		Op{OpType: ENQ, Val: 2},
		Op{OpType: DEQ, Val: 2},
		Op{OpType: ENQ, Val: 3},
		Op{OpType: DEQ, Val: 3},
		Op{OpType: DEQ, Val: BOTTOM},
		Op{OpType: DEQ, Val: BOTTOM},
		Op{OpType: DEQ, Val: BOTTOM},
	}

	r4 := InitRelation(len(ops4))
	r4.Append(0, 1)
	r4.Append(4, 5)
	r4.Append(4, 6)
	r4.Append(4, 8)

	r4Want := InitRelation(len(ops4))
	r4Want.Append(0, 1)
	r4Want.Append(2, 3)
	r4Want.Append(2, 6)
	r4Want.Append(2, 7)

	ops5 := []Op{
		Op{OpType: ENQ, Val: 1},
		Op{OpType: DEQ, Val: 1},
		Op{OpType: ENQ, Val: 2},
		Op{OpType: DEQ, Val: 2},
	}
	r5 := InitRelation(len(ops5))
	r5.Append(1, 2)

	r5Want := InitRelation(len(ops5))
	r5Want.Append(3, 0)

	tests := []struct {
		name   string
		opList []Op
		r      *Relation
		want   *Relation
	}{
		{"t1", ops1, r1, r1},
		{"t2", ops2, r2, r2Want},
		{"t3", ops3, r3, r3Want},
		{"t4", ops4, r4, r4Want},
		{"t5", ops5, r5, r5Want},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			Init(tt.opList)
			r := tt.r.Normalize()
			assert.Equal(t, tt.want, r)
		})
	}
}

func TestRelation_IsNormal(t *testing.T) {
	ops1 := []Op{
		Op{OpType: ENQ, Val: 1},
		Op{OpType: DEQ, Val: 1},
		Op{OpType: DEQ, Val: BOTTOM},
	}
	r1 := InitRelation(len(ops1))
	r1.Append(0, 1)
	r1.Append(2, 0)

	ops2 := []Op{
		Op{OpType: ENQ, Val: 1},
		Op{OpType: DEQ, Val: 1},
		Op{OpType: ENQ, Val: 2},
		Op{OpType: DEQ, Val: 2},
		Op{OpType: DEQ, Val: BOTTOM},
	}
	r2 := InitRelation(len(ops2))
	r2.Append(2, 3)
	r2.Append(3, 4)

	ops3 := append(ops2, Op{OpType: DEQ, Val: BOTTOM})
	r3 := InitRelation(len(ops3))
	r3.Append(1, 2)
	r3.Append(0, 5)

	ops4 := []Op{
		Op{OpType: ENQ, Val: 1},
		Op{OpType: DEQ, Val: 1},
		Op{OpType: ENQ, Val: 2},
		Op{OpType: DEQ, Val: 2},
		Op{OpType: ENQ, Val: 3},
		Op{OpType: DEQ, Val: 3},
		Op{OpType: DEQ, Val: BOTTOM},
		Op{OpType: DEQ, Val: BOTTOM},
		Op{OpType: DEQ, Val: BOTTOM},
	}

	r4 := InitRelation(len(ops4))
	r4.Append(0, 1)
	r4.Append(4, 5)
	r4.Append(4, 6)
	r4.Append(4, 8)

	ops5 := ops4
	r5 := InitRelation(len(ops5))
	r5.Append(0, 1)
	r5.Append(2, 3)
	r5.Append(2, 6)
	r5.Append(2, 7)

	tests := []struct {
		name   string
		opList []Op
		r      *Relation
		want   bool
	}{
		{"t1", ops1, r1, true},
		{"t2", ops2, r2, false},
		{"t3", ops3, r3, false},
		{"t4", ops4, r4, false},
		{"t5", ops5, r5, true},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			Init(tt.opList)
			assert.Equal(t, tt.r.IsNormal(), tt.want)
		})
	}
}
