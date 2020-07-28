# requires pysmt.  To get it, do:
# `pip install pysmt`
# `pysmt-install --z3`
# u can consult `https://github.com/pysmt/pysmt` for more info.
# let me know if u need help with that

#import stuff from pysmt
from pysmt.shortcuts import *
from datetime import datetime

####################
# helper functions #
####################

#x<=y = /\{i,j} x_ij => y_ij
def le(x,y):
  return And([Implies(x[(i,j)],y[(i,j)]) for i in range(0,n) for j in range(0,n)])

#x<y = x<=y /\ \/_{ij} ~x_ij /\ y_ij
def lt(x,y):
  return And(le(x,y), Or([And(Not(x[(i,j)]),y[(i,j)]) for i in range(0,n) for j in range(0,n)]))

#irr(y) = ~y00 /\ ~y11 /\ ..
def irr(y): 
  return And([Not(y[(i,i)]) for i in range(0,n)])

#trans(y) = /\_{i,j,k} y_ij /\ y_jk => y_ik
def trans(y):
  return And([Implies(And(y[(i,j)], y[(j,k)]), y[(i,k)]) for i in range(0,n) for j in range(0,n) for k in range(0,n) if (i != j and j != k)])

#po(y): irr + trans
def po(y):
  return And(irr(y), trans(y))

#tot(y) = /\_{i != j} y_ij \/ y_ji
def tot(y):
  return And([Or(y[(i,j)], y[(j,i)]) for i in range(0,n) for j in range(0,n) if i != j])

def gen_conditions(y, n, k):
  # for each pair i,j s.t. i := enq(v) and j := deq(v) add y[(i,j)]
  # for each pair of pairs <i,j>, <k,l> s.t. i/k := enq(v/w) and j/l := deq(v/w) add:
  # Iff(y[(i,k)], y[(j,l)])
  # Iff(y[(k,i)], y[(l,j)])
  # for each pair i,j as before and k s.t. k := deq(bottom) add:
  # Iff(y[(i,k)], y[(j,k)])
  # Iff(y[(k,i)], y[(k,j)])
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

#queue(O,y) = y02 /\ y13 /\ (y01 <=> y23) /\ (y10 <=> y32)
def queue_O(y, n, k):
  return And(gen_conditions(y, n, k))

#testcase(x): po(x) /\ (forall y, x <=y /\ po(y) /\ tot(y) => ~queue(O,y))
def testcase(x):
  y = {(i,j) : Symbol("y_" + label(i) + "_" + label(j)) for i in range(0,n) for j in range(0,n)}
  return And(po(x), ForAll([y[(i,j)] for i in range(0,n) for j in range(0,n)], Implies(And(le(x,y), po(y), tot(y)), Not(queue_O(y, n, k)))))

def label(i):
  if i < n-k:
    if i % 2 == 0:
      return "enq(" + chr(65 + int(i/2)) + ")"
    return "deq(" + chr(65 + int(i/2)) + ")"
  return "deq(T_" + chr(97 + i - n + k) + ")"


#minimal(x): forall z, z < x => ~testcase(z)
def minimal(x):
  z = {(i,j) : Symbol("z_" + label(i) + label(j)) for i in range(0,n) for j in range(0,n)}
  return ForAll([z[(i,j)] for i in range(0,n) for j in range(0,n)], Implies(lt(z,x), Not(testcase(z))))

###############
# main code   #
###############

print("start", datetime.now())


#number of operations
n = 8

#number of deq(bottom) operations, must maintain:
#k < n
#(n - k) % 2 = 0
k = 0

#create a solver
solver = Solver("z3")

#create Boolean variables
x = {(i,j) : Symbol(label(i)+"_"+label(j)) for i in range(0,n) for j in range(0,n)}

#assert
solver.add_assertion(testcase(x))
solver.add_assertion(minimal(x))

#check satisfiability
res = solver.check_sat()

#print result
print(res)

#get all possible models
while (res):
  #variables that are true in the model
  model_pos = [x[(i,j)] for i in range(0,n) for j in range(0,n) if solver.get_value(x[(i,j)]).is_true()]
  #variables that are false in the model
  model_neg = [x[(i,j)] for i in range(0,n) for j in range(0,n) if solver.get_value(x[(i,j)]).is_false()]
  print(model_pos)
  #negate the variables that are true in the model
  #assert positively the variables that are false in the model
  #put everything in one disjunction
  solver.add_assertion(Or([Not(m) for m in model_pos] + model_neg))
  res = solver.check_sat()

print("end", datetime.now())
