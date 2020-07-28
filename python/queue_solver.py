# requires pysmt.  To get it, do:
# `pip install pysmt`
# `pysmt-install --z3`
# u can consult `https://github.com/pysmt/pysmt` for more info.
# let me know if u need help with that

#import stuff from pysmt
from pysmt.shortcuts import *

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

#queue(O,y) = y02 /\ y13 /\ y01 <=> y23 /\ y10 <=> y32
def queue_O(y):
  return And(y[(0,2)], y[(1,3)], Iff(y[(0,1)], y[(2,3)]), Iff(y[(1,0)], y[(3,2)]))

#testcase(x): po(x) /\ (forall y, x <=y /\ po(y) /\ tot(y) => ~queue(O,y))
def testcase(x):
  y = {(i,j) : Symbol("y_" + str(i) + str(j)) for i in range(0,n) for j in range(0,n)}
  return And(po(x), ForAll([y[(i,j)] for i in range(0,n) for j in range(0,n)], Implies(And(le(x,y), po(y), tot(y)), Not(queue_O(y)))))

#minimal(x): forall z, z < x => ~testcase(z)
def minimal(x):
  z = {(i,j) : Symbol("z_" + str(i) + str(j)) for i in range(0,n) for j in range(0,n)}
  return ForAll([z[(i,j)] for i in range(0,n) for j in range(0,n)], Implies(lt(z,x), Not(testcase(z))))

###############
# main code   #
###############

#number of operations
n = 4
  
#create a solver
solver = Solver("z3")

#create Boolean variables
x = {(i,j) : Symbol("x_" + str(i) + str(j)) for i in range(0,n) for j in range(0,n)}

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