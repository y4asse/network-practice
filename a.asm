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
store r0, 0
store r0, 5
store r0, 1
store r0, 2
store r0, 3
L1:
loadi r1, 0
addr r1, r0
cmpr r1, r0
jge L0
store r0, 1
store r0, 1
store r0, 6
store r0, 1
store r0, 1
store r0, 7
loadi r1, 0
addr r1, r0
cmpr r1, r0
jge L2
store r0, 5
jmp L3
L2:
L3:
store r0, 0
jmp L1
L0:
store r0, 4
load r0, 4
writed r0
loadi r0,'\n'
writec r0
