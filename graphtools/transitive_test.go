package graphtools_test

import (
	"reflect"
	"testing"

	"github.com/ori-saporta83/linearizability-testing/graphtools"
)

func TestTransitiveClosure(t *testing.T) {
	type args struct {
		graph graphtools.Graph
	}
	tests := []struct {
		name string
		args args
		want graphtools.Graph
	}{
		{"t3", args{[][]int{{0, 1, 0}, {0, 0, 1}, {0, 0, 0}}}, graphtools.Graph{{0, 1, 1}, {0, 0, 1}, {0, 0, 0}}},
		{"t4",
			args{[][]int{
				{1, 1, 0, 1},
				{0, 1, 1, 0},
				{0, 0, 1, 1},
				{0, 0, 0, 1}}},
			graphtools.Graph{
				{1, 1, 1, 1},
				{0, 1, 1, 1},
				{0, 0, 1, 1},
				{0, 0, 0, 1}}},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			if got := tt.args.graph.TransitiveClosure(); !reflect.DeepEqual(got, tt.want) {
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
		want graphtools.Graph
	}{
		{"t2", args{2}, graphtools.Graph{{0, 0}, {0, 0}}},
		{"t3", args{3}, graphtools.Graph{{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}},
		{"t4", args{4}, graphtools.Graph{{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			if got := graphtools.InitGraph(tt.args.N); !reflect.DeepEqual(got, tt.want) {
				t.Errorf("InitGraph() = %v, want %v", got, tt.want)
			}
		})
	}
}

func TestGraph_Equals(t *testing.T) {
	type args struct {
		other graphtools.Graph
	}
	tests := []struct {
		name  string
		graph graphtools.Graph
		args  args
		want  bool
	}{
		{"t1", graphtools.InitGraph(2), args{other: graphtools.InitGraph(2)}, true},
		{"t2", graphtools.InitGraph(3), args{other: graphtools.InitGraph(3)}, true},
		{"t3", graphtools.Graph{{0, 1}, {0, 0}}, args{other: graphtools.InitGraph(2)}, false},
		{"t4", graphtools.InitGraph(2), args{other: graphtools.Graph{{0, 1}, {0, 0}}}, false},
		{"t5", graphtools.Graph{{0, 1}, {0, 0}}, args{other: graphtools.Graph{{0, 1}, {0, 0}}}, true},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			if got := tt.graph.Equals(tt.args.other); got != tt.want {
				t.Errorf("Graph.Equals() = %v, want %v", got, tt.want)
			}
		})
	}
}

func TestGraph_Clone(t *testing.T) {
	tests := []struct {
		name  string
		graph graphtools.Graph
		want  graphtools.Graph
	}{
		{"t1", graphtools.InitGraph(2), graphtools.InitGraph(2)},
		{"t2", graphtools.InitGraph(3), graphtools.InitGraph(3)},
		{"t2", graphtools.Graph{{0, 1}, {0, 0}}, graphtools.Graph{{0, 1}, {0, 0}}},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			if got := tt.graph.Clone(); !reflect.DeepEqual(got, tt.want) {
				t.Errorf("Graph.Clone() = %v, want %v", got, tt.want)
			}
		})
	}
}

func TestGraph_SwitchLine(t *testing.T) {
	type args struct {
		i int
		j int
	}
	tests := []struct {
		name  string
		graph graphtools.Graph
		args  args
		want  graphtools.Graph
	}{
		{"t1", graphtools.InitGraph(2), args{0, 1}, graphtools.InitGraph(2)},
		{"t2", graphtools.InitGraph(2), args{0, 2}, graphtools.InitGraph(2)},
		{"t3", graphtools.Graph{{1, 0}, {0, 1}}, args{0, 2}, graphtools.Graph{{1, 0}, {0, 1}}},
		{"t4", graphtools.Graph{{1, 0}, {0, 1}}, args{1, 0}, graphtools.Graph{{0, 1}, {1, 0}}},
		{"t5", graphtools.Graph{{1, 0, 0}, {0, 0, 0}, {0, 0, 0}}, args{0, 2}, graphtools.Graph{{0, 0, 0}, {0, 0, 0}, {1, 0, 0}}},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			tt.graph.SwitchLine(tt.args.i, tt.args.j)
			if got := tt.graph; !reflect.DeepEqual(got, tt.want) {
				t.Errorf("Graph.SwitchLine() = %v, want %v", got, tt.want)
			}
		})
	}
}
