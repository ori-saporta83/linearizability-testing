# requires pysmt.  To get it, do:
# `pip install pysmt`
# `pysmt-install --z3`
# u can consult `https://github.com/pysmt/pysmt` for more info.
# let me know if u need help with that

# import stuff from pysmt
from pysmt.shortcuts import *
from datetime import datetime
from itertools import product

####################
# helper functions #
####################


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
    print("conditions:", conditions)
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
    return "deq(T_" + chr(97 + i - n + k) + ")"


def minimal(rel):
    """minimal(x): forall z, z < x => ~testcase(z)"""
    z = {(i, j): Symbol("z_" + label(i) + label(j))
         for i, j in product(range(n), range(n))}
    return ForAll([z[(i, j)] for i, j in product(range(n), range(n))], Implies(lt(z, rel), Not(testcase(z))))


def gi(x, e1, rel):
    return And(gi_t1(x), gi_t2(x), gi_t3(x, e1, rel), gi_t4(x))


def gi_t1(x):
    return And([Or([x[i, j] for j in range(n)]) for i in range(n)])


def gi_t2(x):
    return And([Or(Not(x[i, k]), Not(x[j, k])) for i, j, k in product(range(n), range(n), range(n)) if i != j])


def gi_t3(x, e1, rel):
    return And([Implies(Iff(e1[i, j], Not(rel[k, l])), Or(Not(x[i, k]), Not(x[j, l]))) for i, j, k, l in product(range(n), range(n), range(n), range(n)) if l != k and i < j])


def gi_t4(x):
    """
    "Color coding": either i, j are both enq/deq(val) so they should be <= n-k and have the same mod2 value, 
    or they are both deq(bot) so they are both >= n-k
    x_ij --> ((i >= n-k) --> (j >= n-k)) || ((i < n-k) && (i%2 == j%2))
    """
    # return And([Implies(x[i, j], Or(Implies(GE(i, n-k), GE(j, n-k)), And(LT(i, n-k), Equals(i % 2, j % 2)))) for i, j in product(range(n), range(n))])
    return And([Implies(x[i, j], Equals(opType(i), opType(j))) for i, j in product(range(n), range(n))])


def opType(i):
  if i >= n-k:
    return Int(0) # deq(bot)
  return Int(i % 2) # 0: enq(val), 1: deq(val)


def main():
    # number of operations
    global n
    n = 4

    # number of deq(bottom) operations, must maintain:
    # k < n
    # (n - k) % 2 = 0
    global k
    k = 0

    print("start", datetime.now())

    # create a solver
    solver = Solver("z3")

    # create Boolean variables
    rel = {(i, j): Symbol(label(i)+"_"+label(j))
           for i, j in product(range(n), range(n))}

    x = {(i, j): Symbol("x_"+str(i)+str(j))
         for i, j in product(range(n), range(n))}

    # assert
    solver.add_assertion(testcase(rel))
    solver.add_assertion(minimal(rel))

    # check satisfiability
    res = solver.check_sat()

    # print result
    print(res)

    # get all possible models
    while (res):
        # variables that are true in the model
        model_pos = [rel[(i, j)] for i, j in product(
            range(n), range(n)) if solver.get_value(rel[(i, j)]).is_true()]
        # variables that are false in the model
        model_neg = [rel[(i, j)] for i, j in product(
            range(n), range(n)) if solver.get_value(rel[(i, j)]).is_false()]

        model = {(i, j): solver.get_value(rel[(i, j)])
                 for i, j in product(range(n), range(n))}
        print(model_pos)
        # negate the variables that are true in the model
        # assert positively the variables that are false in the model
        # put everything in one disjunction
        solver.add_assertion(Not(Exists(x.values(), And(
            gi(x, model, rel), Not(And([x[i, i] for i in range(0, n)]))))))
        # solver.add_assertion(Or([Not(m) for m in model_pos] + model_neg))
        res = solver.check_sat()

    print("end", datetime.now())


if __name__ == "__main__":
    main()
