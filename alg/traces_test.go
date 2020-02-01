package alg_test

import (
	"fmt"
	"testing"
	"time"

	. "github.com/ori-saporta83/linearizability-testing/alg"
)

func TestCheckAllTraces(t *testing.T) {
	ops1 := []Op{
		Op{OpType: ENQ, Val: 1},
		Op{OpType: DEQ, Val: 1},
		Op{OpType: DEQ, Val: BOTTOM},
	}
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
	ops4 := []Op{
		Op{OpType: ENQ, Val: 1},
		Op{OpType: DEQ, Val: 1},
		Op{OpType: ENQ, Val: 2},
		Op{OpType: DEQ, Val: 2},
		Op{OpType: DEQ, Val: BOTTOM},
		// Op{OpType: DEQ, Val: 3},
		// Op{OpType: DEQ, Val: BOTTOM},
		// Op{OpType: DEQ, Val: BOTTOM},
	}

	type args struct {
		ops []Op
	}
	tests := []struct {
		name string
		args args
	}{
		{"t1", args{ops: ops1}},
		{"t2", args{ops: ops2}},
		{"t3", args{ops: ops3}},
		{"t4", args{ops: ops4}},
	}
	for i, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			fmt.Printf("======= %d started at %s\n", i, time.Now().String())
			got := CheckAllTraces(tt.args.ops)
			for _, r := range got {
				fmt.Println(r)
			}
			fmt.Printf("======= %d finished at %s\n", i, time.Now().String())
		})
	}
}

func TestCheckAllTraces2(t *testing.T) {
	ops1 := []Op{
		Op{OpType: ENQ, Val: 1},
		Op{OpType: DEQ, Val: 1},
		Op{OpType: DEQ, Val: BOTTOM},
	}
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
	ops4 := []Op{
		Op{OpType: ENQ, Val: 1},
		Op{OpType: DEQ, Val: 1},
		Op{OpType: ENQ, Val: 2},
		Op{OpType: DEQ, Val: 2},
		Op{OpType: DEQ, Val: BOTTOM},
	}
	ops5 := []Op{
		Op{OpType: ENQ, Val: 1},
		Op{OpType: DEQ, Val: 1},
		Op{OpType: ENQ, Val: 2},
		Op{OpType: DEQ, Val: 2},
		Op{OpType: ENQ, Val: 3},
		Op{OpType: DEQ, Val: 3},
	}
	ops6 := []Op{
		Op{OpType: ENQ, Val: 1},
		Op{OpType: DEQ, Val: 1},
		Op{OpType: ENQ, Val: 2},
		Op{OpType: DEQ, Val: 2},
		Op{OpType: ENQ, Val: 3},
		Op{OpType: DEQ, Val: 3},
		Op{OpType: DEQ, Val: BOTTOM},
	}
	ops7 := []Op{
		Op{OpType: ENQ, Val: 1},
		Op{OpType: DEQ, Val: 1},
		Op{OpType: ENQ, Val: 2},
		Op{OpType: DEQ, Val: 2},
		Op{OpType: ENQ, Val: 3},
		Op{OpType: DEQ, Val: 3},
		Op{OpType: ENQ, Val: 4},
		Op{OpType: DEQ, Val: 4},
	}

	type args struct {
		ops []Op
	}
	tests := []struct {
		name string
		args args
	}{
		{"t1", args{ops: ops1}},
		{"t2", args{ops: ops2}},
		{"t3", args{ops: ops3}},
		{"t4", args{ops: ops4}},
		{"t5", args{ops: ops5}},
		{"t6", args{ops: ops6}},
		{"t7", args{ops: ops7}},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			fmt.Printf("======= %s started at %s\n", tt.name, time.Now().String())
			got := CheckAllTraces2(tt.args.ops)
			for _, r := range got {
				fmt.Println(r)
			}
			fmt.Printf("======= %s finished at %s\n", tt.name, time.Now().String())
		})
	}
}
