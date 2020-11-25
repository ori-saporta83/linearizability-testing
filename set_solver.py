# requires pysmt.  To get it, do:
# `pip install pysmt`
# `pysmt-install --z3`
# u can consult `https://github.com/pysmt/pysmt` for more info.
# let me know if u need help with that

# import stuff from pysmt
from pysmt.shortcuts import *
from datetime import datetime
from itertools import product
import os

####################
# helper functions #
####################
n = None
k = None
l = None
ops = None

def le(x, y):
    """x<=y = /\{i,j} x_ij => y_ij"""
    return And([Implies(x[(i, j)], y[(i, j)]) for i, j in product(range(len(ops)), range(len(ops)))])


def lt(x, y):
    """x<y = x<=y /\ \/_{ij} ~x_ij /\ y_ij"""
    return And(le(x, y), Or([And(Not(x[(i, j)]), y[(i, j)]) for i, j in product(range(len(ops)), range(len(ops)))]))


def irr(y):
    """irr(y) = ~y00 /\ ~y11 /\ .."""
    return And([Not(y[(i, i)]) for i in range(len(ops))])


def trans(y):
    """trans(y) = /\_{i,j,k} y_ij /\ y_jk => y_ik"""
    return And([Implies(And(y[(i, j)], y[(j, k)]), y[(i, k)]) for i, j, k in product(range(len(ops)), range(len(ops)), range(len(ops))) if (i != j and j != k)])


def po(y):
    """po(y): irr + trans"""
    return And(irr(y), trans(y))


def tot(y):
    """tot(y) = /\_{i != j} y_ij \/ y_ji"""
    return And([Or(y[(i, j)], y[(j, i)]) for i, j in product(range(len(ops)), range(len(ops))) if i != j])

def gen_conditions(y, n, k, j):
    """ 
    for each pair i,j s.t. i := add(v) and j := remove(v) add y[(i,j)]
    """
    if n < 1:
        raise Exception("n must be at least 1")
    group_size = 2 + k + l
    conditions = []
    for i in range(n):
        add_index = i*(group_size)
        remove_index = add_index + 1
        # add before remove
        conditions.append(y[(add_index, remove_index)])
        for j in range(k):
            curr_op_index = add_index+2+j
            # in_true after add
            conditions.append(y[(add_index, curr_op_index)])
            # in_true before remove
            conditions.append(y[(curr_op_index, remove_index)])
        for j in range(l):
            curr_op_index = add_index+2+k+j
            # Either before add or after remove
            conditions.append(
                Or(y[(remove_index, curr_op_index)], y[(curr_op_index, add_index)]))
    return conditions


def set_O(y, n, k, l):
    return And(gen_conditions(y, n, k, l))


def testcase(x):
    """testcase(x): po(x) /\ (forall y, x <=y /\ po(y) /\ tot(y) => ~set(O,y))"""
    global ops, n, k, l
    y = {(i, j): Symbol("y_" + label(i) + "_" + label(j))
         for i, j in product(range(len(ops)), range(len(ops)))}
    return And(po(x), ForAll([y[(i, j)] for i, j in product(range(len(ops)), range(len(ops)))], Implies(And(le(x, y), po(y), tot(y)), Not(set_O(y, n, k, l)))))


def gen_ops(n, k, l):
    result = []
    for i in range(n):
        result.append("add(" + chr(65 + int(i)) + ")")
        result.append("remove(" + chr(65 + int(i)) + ")")
        for j in range(k):
            result.append("in(" + chr(65 + int(i)) + ")-t-" + str(j))
        for j in range(l):
            result.append("in(" + chr(65 + int(i)) + ")-f-" + str(j))
    return result


def label(i):
    global ops
    return ops[i]


def minimal_one(rel, i0, j0):
    global ops
    z = {(i, j): rel[(i, j)]
         for i, j in product(range(len(ops)), range(len(ops)))}
    z[(i0, j0)] = Not(rel[(i0, j0)])
    return Not(testcase(z))


def minimal_all(rel):
    global ops
    return And([Implies(rel[(i, j)], minimal_one(rel, i, j)) for i, j in product(range(len(ops)), range(len(ops)))])


# def gi(x, e1, rel):
#     terms = And(gi_t1(x), gi_t2(x), gi_t3(x, e1, rel), gi_t4(x))
#     return terms


# def gi_strong(x, e1, rel):
#     return And(gi(x, e1, rel), gi_t5(x))


# def gi_t1(x):
#     global ops
#     return And([Or([x[i, j] for j in range(len(ops))]) for i in range(len(ops))])


# def gi_t2(x):
#     global ops
#     return And([Or(Not(x[i, k]), Not(x[j, k])) for i, j, k in product(range(len(ops)), range(len(ops)), range(len(ops))) if i != j])


# def gi_t3(x, e1, rel):
#     """ x[i,k] && x[j,l] --> (e1[i,j] <--> rel[k,l]) """
#     global ops
#     return And([Implies(Iff(e1[i, j], Not(rel[k, l])), Or(Not(x[i, k]), Not(x[j, l]))) for i, j, k, l in product(range(len(ops)), range(len(ops)), range(len(ops)), range(len(ops))) if l != k and i != j])


# def gi_t4(x):
#     """
#     "Color coding": mapped ops should be of the same type (just different value)
#     """
#     global ops
#     return And([Not(x[i, j]) for i, j in product(range(len(ops)), range(len(ops))) if opType(i) != opType(j)])


# def gi_t5(x):
#     """remap by values"""
#     global ops
#     return And([Not(And(x[i, j], x[i+m, t])) for i, j, t in product(range(len(ops)), range(len(ops)), range(len(ops))) for m in range(1, 2+k+l) if base_val(j) != base_val(t) and opType(i) == 0])

def base_val(i):
    global k, l
    return int(i / (2+k+l))

def opType(i):
    s = ops[i]
    desc = s[:s.index("(")]+s[s.index(")")+1:]
    if desc == "add":
        return 0
    elif desc == "remove":
        return 1
    elif desc == "in-t":
        return 2
    elif desc == "in-f":
        return 3

def main():
    # number of values
    global n
    n = 3

    # number of in per value that return true:
    global k
    k = 1

    # number of in per value that return false:
    global l
    l = 1

    global ops
    ops = gen_ops(n, k, l)

    print("start", datetime.now())
    print("n:", n, "k:", k, "l:", l)

    try:
        # create a solver
        solver = Solver("z3")

        # create Boolean variables
        rel = {(i, j): Symbol(label(i)+"_"+label(j))
               for i, j in product(range(len(ops)), range(len(ops)))}

        x = {(i, j): Symbol("x_"+label(i)+"_"+label(j))
             for i, j in product(range(len(ops)), range(len(ops)))}

        # assert
        solver.add_assertion(testcase(rel))
        solver.add_assertion(minimal_all(rel))

        # check satisfiability
        res = solver.check_sat()

        # get all possible models
        i = 1
        while (res):
            # variables that are true in the model
            model_pos = [rel[(i, j)] for i, j in product(
                range(len(ops)), range(len(ops))) if solver.get_value(rel[(i, j)]).is_true()]

            print(i, ":", model_pos)
            i += 1

            # variables that are false in the model
            model_neg = [rel[(i, j)] for i, j in product(
                range(n), range(n)) if solver.get_value(rel[(i, j)]).is_false()]

            # solver.add_assertion(Not(Exists(x.values(), gi(x, model, rel))))
            solver.add_assertion(Or([Not(m) for m in model_pos] + model_neg))

            res = solver.check_sat()

    finally:
        print("end", datetime.now())


if __name__ == "__main__":
    main()
