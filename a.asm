loadi r0, 0
store r0, 0
loadi r0, 0
store r0, 1
loadi r0, 0
store r0, 2
loadi r0, 0
store r0, 3
loadi r0, 0
store r0, 4
loadi r0, 0
store r0, 5
loadi r0, 0
store r0, 6
loadi r0, 0
store r0, 7
loadi r0, 0
store r0, 0
loadi r0, 0
store r0, 5
loadi r0, 3827
store r0, 1
loadi r0, 8358
store r0, 2
loadi r0, 3797
store r0, 3
L1:
load r0, 0
loadi r1, 0
addr r1, r0
loadi r1, 1000
loadi r2, 2
addr r1, r2
cmpr r0, r1
jge L0
load r0, 1
load r1, 2
addr r0, r1
load r1, 3
mulr r0, r1
load r1, 1
load r2, 2
addr r1, r2
load r2, 3
mulr r1, r2
load r2, Data0
divr r1, r2
load r2, Data1
mulr r1, r2
subr r0, r1
store r0, 1
load r0, 1
loadi r1, 10
divr r0, r1
store r0, 1
load r0, 1
loadi r1, 10
divr r0, r1
store r0, 6
load r0, 1
load r1, 2
addr r0, r1
load r1, 3
mulr r0, r1
load r1, 1
load r2, 2
addr r1, r2
load r2, 3
mulr r1, r2
load r2, Data2
divr r1, r2
load r2, Data3
mulr r1, r2
subr r0, r1
store r0, 1
load r0, 1
loadi r1, 10
divr r0, r1
store r0, 1
load r0, 1
loadi r1, 10
divr r0, r1
store r0, 7
load r0, 6
load r1, 6
mulr r0, r1
load r1, 7
load r2, 7
mulr r1, r2
addr r0, r1
loadi r1, 0
addr r1, r0
load r1, Data4
cmpr r0, r1
jge L2
load r0, 5
loadi r1, 1
addr r0, r1
store r0, 5
jmp L3
L2:
L3:
load r0, 0
loadi r1, 1
addr r0, r1
store r0, 0
jmp L1
L0:
load r0, 5
loadi r1, 4
mulr r0, r1
store r0, 4
load r0, 0
writed r0
loadi r0,'\n'
writec r0
halt
Data0: data 100000
Data1: data 100000
Data2: data 100000
Data3: data 100000
Data4: data 1000000
