package graphtools

import (
	"reflect"
	"testing"
)

func TestTransitiveClosure(t *testing.T) {
	type args struct {
		graph [][]int
	}
	tests := []struct {
		name string
		args args
		want [][]int
	}{
		{"t3", args{[][]int{{0, 1, 0}, {0, 0, 1}, {0, 0, 0}}}, [][]int{{0, 1, 1}, {0, 0, 1}, {0, 0, 0}}},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			if got := TransitiveClosure(tt.args.graph); !reflect.DeepEqual(got, tt.want) {
				t.Errorf("TransitiveClosure() = %v, want %v", got, tt.want)
			}
		})
	}
}

func TestInitGraph(t *testing.T) {
	type args struct {
		N int
	}
	tests := []struct {
		name string
		args args
		want [][]int
	}{
		{"t2", args{2}, [][]int{{0, 0}, {0, 0}}},
		{"t3", args{3}, [][]int{{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}},
		{"t4", args{4}, [][]int{{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			if got := InitGraph(tt.args.N); !reflect.DeepEqual(got, tt.want) {
				t.Errorf("InitGraph() = %v, want %v", got, tt.want)
			}
		})
	}
}
