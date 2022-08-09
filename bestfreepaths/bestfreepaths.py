# -*- coding: utf-8 -*-
"""
Created on Thu Jul  7 04:26:13 2022

@author: x213212
"""

from z3 import *

def Getlistlogic(sindex,stmtindex,list_array ):
    list_array[0] = (a) if stmtindex>sindex else  Not(a)
    list_array[1] = b if list_array[1]==1 else Not(b)
    list_array[2] = c if list_array[2]==1 else Not(c)
    list_array[3] = d if list_array[3]==1 else Not(d)
    return list_array

start_bb_index= 2
a, b, c, d= Bools('a b c d')
q1 = Or(a, b, c, d)
q2 = Implies(d, c)
q3 = a
q4 = Or(And(c,d),Not(c))
q5 = And((a),(b),(c),(d))
# q6 = And(Not(a),(b),(c),(d))

x = [
[7,1,1,1],
[6,1,0,0],
[5,1,1,0],
[3,1,0,0],
[4,0,0,1]
]
x_back = [
[7,1,1,1],
[6,1,0,0],
[5,1,1,0],
[3,1,0,0],
[4,0,0,1]
]
# print(x[0])
new_x =[]
for getarray in x:
    new_x.append((Getlistlogic(start_bb_index,getarray[0],getarray)))

s = Solver()
test = And(q1,q2 ,q3,q4,q5)
s.add(test)
# print(new_x)
options=[]

for x in new_x:
    options.append(x)
print(options)
# for answer, option in zip("ABCDEF", options):
result = []
index=0
for  option in  options:
    s.push()
    s.add(option)
    # print(answer, s.check(), s.assertions())
    if s.check() != sat:
        print(x_back[index])    
        result.append(x_back[index])
        
    index = index+1
    s.pop()
print("Answer：", (result))
