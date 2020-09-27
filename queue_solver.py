# requires pysmt.  To get it, do:
# `pip install pysmt`
# `pysmt-install --z3`
# u can consult `https://github.com/pysmt/pysmt` for more info.
# let me know if u need help with that

# import stuff from pysmt
from pysmt.shortcuts import *
from datetime import datetime
from itertools import product
from queue_testgen import generate_test
import os

####################
# helper functions #
####################
n = None
k = None
q = None

def le(x, y):
    """x<=y = /\{i,j} x_ij => y_ij"""
    return And([Implies(x[(i, j)], y[(i, j)]) for i, j in product(range(n), range(n))])


def lt(x, y):
    """x<y = x<=y /\ \/_{ij} ~x_ij /\ y_ij"""
    return And(le(x, y), Or([And(Not(x[(i, j)]), y[(i, j)]) for i, j in product(range(n), range(n))]))


def irr(y):
    """irr(y) = ~y00 /\ ~y11 /\ .."""
    return And([Not(y[(i, i)]) for i in range(n)])


def trans(y):
    """trans(y) = /\_{i,j,k} y_ij /\ y_jk => y_ik"""
    return And([Implies(And(y[(i, j)], y[(j, k)]), y[(i, k)]) for i, j, k in product(range(n), range(n), range(n)) if (i != j and j != k)])


def po(y):
    """po(y): irr + trans"""
    return And(irr(y), trans(y))


def tot(y):
    """tot(y) = /\_{i != j} y_ij \/ y_ji"""
    return And([Or(y[(i, j)], y[(j, i)]) for i, j in product(range(n), range(n)) if i != j])


def gen_conditions(y, n, k):
    """ 
    for each pair i,j s.t. i := enq(v) and j := deq(v) add y[(i,j)]
    for each pair of pairs <i,j>, <k,l> s.t. i/k := enq(v/w) and j/l := deq(v/w) add:
    Iff(y[(i,k)], y[(j,l)])
    Iff(y[(k,i)], y[(l,j)])
    for each pair i,j as before and k s.t. k := deq(bottom) add:
    Iff(y[(i,k)], y[(j,k)])
    Iff(y[(k,i)], y[(k,j)]) 
    """
    if k >= n:
        raise Exception("k must be less than n")
    if (n-k) % 2 != 0:
        raise Exception("n - k must be even")
    conditions = []
    pairs = int((n-k) / 2)
    for i in range(0, pairs):
        conditions.append(y[(2*i, 2*i+1)])
        for j in range(i+1, pairs):
            conditions.append(Iff(y[(2*i, 2*j)], y[(2*i+1, 2*j+1)]))
            conditions.append(Iff(y[(2*j, 2*i)], y[(2*j+1, 2*i+1)]))
        for j in range(n-k, n):
            conditions.append(Iff(y[(2*i, j)], y[(2*i+1, j)]))
            conditions.append(Iff(y[(j, 2*i)], y[(j, 2*i+1)]))
    return conditions


def queue_O(y, n, k):
    """queue(O,y) = y02 /\ y13 /\ (y01 <=> y23) /\ (y10 <=> y32)"""
    return And(gen_conditions(y, n, k))


def testcase(x):
    """testcase(x): po(x) /\ (forall y, x <=y /\ po(y) /\ tot(y) => ~queue(O,y))"""
    y = {(i, j): Symbol("y_" + label(i) + "_" + label(j))
         for i, j in product(range(n), range(n))}
    return And(po(x), ForAll([y[(i, j)] for i, j in product(range(n), range(n))], Implies(And(le(x, y), po(y), tot(y)), Not(queue_O(y, n, k)))))


def label(i):
    if i < n-k:
        if i % 2 == 0:
            return "enq(" + chr(65 + int(i/2)) + ")"
        return "deq(" + chr(65 + int(i/2)) + ")"
    return "deq(T-" + chr(97 + i - n + k) + ")"


def minimal_one(rel,i0,j0):
    z = {(i, j): rel[(i, j)]
         for i, j in product(range(n), range(n))}
    z[(i0, j0)] = Not(rel[(i0, j0)])
    return Not(testcase(z))

def minimal_all(rel):
    return And([Implies(rel[(i,j)], minimal_one(rel,i,j)) for i, j in product(range(n), range(n))])


def gi(x, e1, rel):
    terms = And(gi_t1(x), gi_t2(x), gi_t3(x, e1, rel), gi_t4(x))
    return terms


def gi_strong(x, e1, rel):
    return And(gi(x, e1, rel), gi_t5(x))


def gi_t1(x):
    return And([Or([x[i, j] for j in range(n)]) for i in range(n)])


def gi_t2(x):
    return And([Or(Not(x[i, k]), Not(x[j, k])) for i, j, k in product(range(n), range(n), range(n)) if i != j])


def gi_t3(x, e1, rel):
    # x[i,k] && x[j,l] --> (e1[i,j] <--> rel[k,l])
    return And([Implies(Iff(e1[i, j], Not(rel[k, l])), Or(Not(x[i, k]), Not(x[j, l]))) for i, j, k, l in product(range(n), range(n), range(n), range(n)) if l != k and i != j])


def gi_t4(x):
    """
    "Color coding": either i, j are both enq/deq(val) so they should be <= n-k and have the same mod2 value, 
    or they are both deq(bot) so they are both >= n-k
    x_ij --> ((i >= n-k) --> (j >= n-k)) || ((i < n-k) && (i%2 == j%2))
    """
    return And([Not(x[i, j]) for i, j in product(range(n), range(n)) if opType(i) != opType(j)])


def gi_t5(x):
    """remap by pairs"""
    return And([Implies(x[i, j], x[i+1, j+1]) for i, j in product(range(n), range(n)) if opType(i) == 0 and opType(j) == 0])


def opType(i):
    if i >= n-k:
        return 2  # 2: deq(bot)
    return i % 2  # 0: enq(val), 1: deq(val)


def main():
    global q
    q = "qu"

    # number of operations
    global n
    n = 7

    # number of deq(bottom) operations, must maintain:
    # k < n
    # (n - k) % 2 = 0
    global k
    k = 1

    print("start", datetime.now())
    print("n:", n, "k:", k, "q:", q)

    path = os.path.join("tests","generated", q+"_"+str(n)+"_"+str(k))
    if not os.path.exists(path):
        os.mkdir(path)

    try:
        # create a solver
        solver = Solver("z3")

        # create Boolean variables
        rel = {(i, j): Symbol(label(i)+"_"+label(j))
               for i, j in product(range(n), range(n))}

        # rel2 = {(i, j): Symbol("A_" + label(i)+"_"+label(j))
        #        for i, j in product(range(n), range(n))}

        x = {(i, j): Symbol("x_"+label(i)+"_"+label(j))
             for i, j in product(range(n), range(n))}

        # assert
        solver.add_assertion(testcase(rel))
        solver.add_assertion(minimal_all(rel))

        # solver.add_assertion(testcase(rel2))
        # solver.add_assertion(minimal(rel2))

        # solver.add_assertion(Exists(x.values(), gi(x, rel2, rel)))
        # solver.add_assertion(Not(Exists(x.values(), gi_strong(x, rel2, rel))))

        # check satisfiability
        res = solver.check_sat()

        # get all possible models
        i = 1
        while (res):
            # variables that are true in the model
            model_pos = [rel[(i, j)] for i, j in product(
                range(n), range(n)) if solver.get_value(rel[(i, j)]).is_true()]
            # variables that are false in the model
            model_neg = [rel[(i, j)] for i, j in product(
                range(n), range(n)) if solver.get_value(rel[(i, j)]).is_false()]

            model = {(i, j): solver.get_value(rel[(i, j)])
                     for i, j in product(range(n), range(n))}

            # variables that are true in the model
            # model_pos2 = [rel2[(i, j)] for i, j in product(
            #     range(n), range(n)) if solver.get_value(rel2[(i, j)]).is_true()]

            model_x = [x[(i, j)] for i, j in product(
                range(n), range(n)) if solver.get_value(x[(i, j)]).is_true()]

            print(i, ":", model_pos)
            test_data = generate_test(model_pos, n, k, q)
            f = open(os.path.join(path, "t"+str(i)+".c"), "w")
            f.write(test_data)
            f.close()
            i += 1
            # print("rel2:", model_pos2)
            # print("X:", model_x)
            # break
            # negate the variables that are true in the model
            # assert positively the variables that are false in the model
            # put everything in one disjunction

            # solver.add_assertion(Or([Not(m) for m in model_pos] + model_neg))

            solver.add_assertion(Not(Exists(x.values(), gi(x, model, rel))))

            res = solver.check_sat()

    finally:
        print("end", datetime.now())


if __name__ == "__main__":
    main()
