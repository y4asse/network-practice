loadi r0, 0
store r0, 0
loadi r0, 0
store r0, 1
loadi r0, 0
store r0, 2
loadi r0, 1
store r0, 0
L1:
load r0, 0
loadi r1, 0
addr r1, r0
loadi r0, 4
cmpr r1, r0
jge L0
loadi r0, 1
store r0, 1
L3:
load r0, 1
loadi r1, 0
addr r1, r0
loadi r0, 4
cmpr r1, r0
jge L2
load r0, 0
mul r0, 1
store r0, 2
load r0, 2
writed r0
loadi r0,'\n'
writec r0
load r0, 1
addi r0, 1
store r0, 1
jmp L3
L2:
load r0, 0
addi r0, 1
store r0, 0
jmp L1
L0:
