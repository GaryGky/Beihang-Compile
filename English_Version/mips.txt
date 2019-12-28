.data
space: .asciiz " "
newLine: .asciiz "\n"
Str1: .asciiz "global a:"
Str2: .asciiz "global b:"
Str3: .asciiz "Before assignment value to local a/b in function,a + b is "
Str4: .asciiz "After assignment value to local  a/b in function,a + b is "
Str5: .asciiz "Before assignment value to global a/b in function,a + b is "
Str6: .asciiz "The testCh is : "
Str7: .asciiz "Global a + b is "
Str8: .asciiz "Global a + b is "
Str9: .asciiz "Lets test array"
takeUp: .space 3
str: .space 40
abc: .space 40
testnum: .space 40
testCh: .space 4
a: .space 4
b: .space 4

.text
j main
print_global:
sw $fp, 0($sp)
addi $sp,$sp,-4
move $fp,$sp
addi $sp,$sp,-80
addi $sp,$sp,-4
sw $ra, -16($fp)
la $a0, Str1
li $v0, 4
syscall
la $a0, a
lw $a0, 0($a0)
li $v0, 1
syscall
la $a0, newLine
li $v0, 4
syscall
la $a0, Str2
li $v0, 4
syscall
la $a0, b
lw $a0, 0($a0)
li $v0, 1
syscall
la $a0, newLine
li $v0, 4
syscall
li $v0 0
lw $ra,-16($fp)
addi $sp,$fp,4
lw $fp, 4($fp)
lw $a1, -4($fp)
lw $a2, -8($fp)
lw $a3, -12($fp)
jr $ra
local_same_name:
sw $fp, 0($sp)
addi $sp,$sp,-4
move $fp,$sp
addi $sp,$sp,-80
addi $sp,$sp,-4
sw $ra, -16($fp)
la $a0, Str3
li $v0, 4
syscall
add $t1 $a1 $a2
move $a0 $t1
li $v0, 1
syscall
la $a0, newLine
li $v0, 4
syscall
li $k0 2
mult $k0 $a1
mflo $t2
move $a1 $t2
li $k0 2
mult $k0 $a2
mflo $t3
move $a2 $t3
la $a0, Str4
li $v0, 4
syscall
add $t4 $a1 $a2
move $a0 $t4
li $v0, 1
syscall
la $a0, newLine
li $v0, 4
syscall
li $v0 0
lw $ra,-16($fp)
addi $sp,$fp,4
lw $fp, 4($fp)
lw $t1, -24($fp)
lw $t2, -28($fp)
lw $t3, -32($fp)
lw $t4, -36($fp)
lw $a1, -4($fp)
lw $a2, -8($fp)
lw $a3, -12($fp)
jr $ra
global_variable:
sw $fp, 0($sp)
addi $sp,$sp,-4
move $fp,$sp
addi $sp,$sp,-80
addi $sp,$sp,-4
sw $ra, -16($fp)
la $a0, Str5
li $v0, 4
syscall
la $k0, a
lw $k0, 0($k0)
la $k1, b
lw $k1, 0($k1)
add $t5 $k0 $k1
move $a0 $t5
li $v0, 1
syscall
la $a0, newLine
li $v0, 4
syscall
la $k0, a
sw $a1 0($k0)
la $k0, b
sw $a2 0($k0)
li $v0 0
lw $ra,-16($fp)
addi $sp,$fp,4
lw $fp, 4($fp)
lw $t5, -40($fp)
lw $a1, -4($fp)
lw $a2, -8($fp)
lw $a3, -12($fp)
jr $ra
getCh:
sw $fp, 0($sp)
addi $sp,$sp,-4
move $fp,$sp
addi $sp,$sp,-80
addi $sp,$sp,-4
sw $ra, -16($fp)
li $v0,12
syscall
la $k0, testCh
sw $v0, 0($k0)
la $v0, testCh
lw $v0, 0($v0)
lw $ra,-16($fp)
addi $sp,$fp,4
lw $fp, 4($fp)
lw $a1, -4($fp)
lw $a2, -8($fp)
lw $a3, -12($fp)
jr $ra
init_str:
sw $fp, 0($sp)
addi $sp,$sp,-4
move $fp,$sp
addi $sp,$sp,-80
addi $sp, $sp, -8
addi $sp,$sp,-4
sw $ra, -16($fp)
li $s1 0
li $s0 10
li $s1 0
Label1:
bge $s1, 10, Label2
move $v1 $s1
add $t6 $s1 $s0
addi $t7 $t6 2
addi $t0 $t7 -1
sub $t1 $t0 $s0
sll $t9 $v1 2
la $t8 str
add $k0 $t8 $t9
sw $t1 0($k0)
addi $s1 $s1 1
j Label1
Label2:
li $v0 0
lw $ra,-16($fp)
addi $sp,$fp,4
lw $fp, 4($fp)
lw $s0, -52($fp)
lw $s1, -56($fp)
lw $t0, -20($fp)
lw $t1, -24($fp)
lw $t6, -44($fp)
lw $t7, -48($fp)
lw $a1, -4($fp)
lw $a2, -8($fp)
lw $a3, -12($fp)
jr $ra
init_abc:
sw $fp, 0($sp)
addi $sp,$sp,-4
move $fp,$sp
addi $sp,$sp,-80
addi $sp, $sp, -4
addi $sp,$sp,-4
sw $ra, -16($fp)
li $s0 0
li $s0 0
Label3:
bge $s0, 10, Label4
move $v1 $s0
li $k0 10
sub $t3 $k0 $s0
add $t4 $t3 $a1
addi $t5 $t4 2
addi $t6 $t5 -2
sub $t7 $t6 $a1
sll $t9 $v1 2
la $t8 abc
add $k0 $t8 $t9
sw $t7 0($k0)
addi $s0 $s0 1
j Label3
Label4:
li $v0 0
lw $ra,-16($fp)
addi $sp,$fp,4
lw $fp, 4($fp)
lw $s0, -52($fp)
lw $t3, -32($fp)
lw $t4, -36($fp)
lw $t5, -40($fp)
lw $t6, -44($fp)
lw $t7, -48($fp)
lw $a1, -4($fp)
lw $a2, -8($fp)
lw $a3, -12($fp)
jr $ra
Factorial:
sw $fp, 0($sp)
addi $sp,$sp,-4
move $fp,$sp
addi $sp,$sp,-80
addi $sp,$sp,-4
sw $ra, -16($fp)
ble $a1, 1, Label5
addi $t1 $a1 -1
sw $t1, -24($fp)
sw $t2, -28($fp)
sw $t3, -32($fp)
sw $a1, -4($fp)
sw $a2, -8($fp)
sw $a3, -12($fp)
move $a1 $t1
jal Factorial
move $t2, $v0
mult $a1 $t2
mflo $t3
move $v0 $t3
lw $ra,-16($fp)
addi $sp,$fp,4
lw $fp, 4($fp)
lw $t1, -24($fp)
lw $t2, -28($fp)
lw $t3, -32($fp)
lw $a1, -4($fp)
lw $a2, -8($fp)
lw $a3, -12($fp)
jr $ra
j Label6
Label5:
li $v0 1
lw $ra,-16($fp)
addi $sp,$fp,4
lw $fp, 4($fp)
lw $t1, -24($fp)
lw $t2, -28($fp)
lw $t3, -32($fp)
lw $a1, -4($fp)
lw $a2, -8($fp)
lw $a3, -12($fp)
jr $ra
Label6:
init:
sw $fp, 0($sp)
addi $sp,$sp,-4
move $fp,$sp
addi $sp,$sp,-80
addi $sp, $sp, -4
addi $sp,$sp,-4
sw $ra, -16($fp)
li $s0 0
Label7:
bge $s0, $a1, Label8
move $v1 $s0
sw $t1, -24($fp)
sw $t2, -28($fp)
sw $t3, -32($fp)
sw $a1, -4($fp)
sw $a2, -8($fp)
sw $a3, -12($fp)
move $a1 $s0
jal Factorial
move $t4, $v0
add $t5 $t4 $s0
sll $t9 $v1 2
la $t8 testnum
add $k0 $t8 $t9
sw $t5 0($k0)
addi $s0 $s0 1
j Label7
Label8:
li $v0 0
lw $ra,-16($fp)
addi $sp,$fp,4
lw $fp, 4($fp)
lw $s0, -52($fp)
lw $t4, -36($fp)
lw $t5, -40($fp)
lw $a1, -4($fp)
lw $a2, -8($fp)
lw $a3, -12($fp)
jr $ra
main:
sw $fp, 0($sp)
addi $sp,$sp,-4
move $fp,$sp
addi $sp,$sp,-80
addi $sp, $sp, -8
addi $sp,$sp,-4
sw $ra, -16($fp)
la $a0, Str6
li $v0, 4
syscall
sw $a1, -4($fp)
sw $a2, -8($fp)
sw $a3, -12($fp)
jal getCh
move $t7, $v0
move $a0 $t7
li $v0, 11
syscall
la $a0, newLine
li $v0, 4
syscall
li $v0,5
syscall
la $k0, a
sw $v0, 0($k0)
li $v0,5
syscall
la $k0, b
sw $v0, 0($k0)
la $a0, Str7
li $v0, 4
syscall
la $k0, a
lw $k0, 0($k0)
la $k1, b
lw $k1, 0($k1)
add $t0 $k0 $k1
move $a0 $t0
li $v0, 1
syscall
la $a0, newLine
li $v0, 4
syscall
sw $t1, -24($fp)
sw $t2, -28($fp)
sw $t3, -32($fp)
sw $t4, -36($fp)
sw $a1, -4($fp)
sw $a2, -8($fp)
sw $a3, -12($fp)
li $a1 100
li $a2 200
jal local_same_name
move $t1, $v0
sw $t5, -40($fp)
sw $a1, -4($fp)
sw $a2, -8($fp)
sw $a3, -12($fp)
li $a1 2152
li $a2 14243
jal global_variable
move $t2, $v0
la $a0, Str8
li $v0, 4
syscall
la $k0, a
lw $k0, 0($k0)
la $k1, b
lw $k1, 0($k1)
add $t3 $k0 $k1
move $a0 $t3
li $v0, 1
syscall
la $a0, newLine
li $v0, 4
syscall
sw $s0, -52($fp)
sw $t3, -32($fp)
sw $t4, -36($fp)
sw $t5, -40($fp)
sw $t6, -44($fp)
sw $t7, -48($fp)
sw $a1, -4($fp)
sw $a2, -8($fp)
sw $a3, -12($fp)
li $a1 10
jal init_abc
move $t4, $v0
sw $s0, -52($fp)
sw $s1, -56($fp)
sw $t0, -20($fp)
sw $t1, -24($fp)
sw $t6, -44($fp)
sw $t7, -48($fp)
sw $a1, -4($fp)
sw $a2, -8($fp)
sw $a3, -12($fp)
li $a1 512
jal init_str
move $t5, $v0
sw $s0, -52($fp)
sw $t4, -36($fp)
sw $t5, -40($fp)
sw $a1, -4($fp)
sw $a2, -8($fp)
sw $a3, -12($fp)
li $a1 10
jal init
move $t6, $v0
li $t9 3
sll $t9 $t9 2
la $t8 abc
add $k0 $t8 $t9
lw $k0, 0($k0)
addi $t7 $k0 0
sll $t9 $t7 2
la $t8 str
add $k0 $t8 $t9
lw $k0, 0($k0)
addi $t0 $k0 0
move $v1 $t0
li $k1 0
sll $t9 $v1 2
la $t8 str
add $k0 $t8 $t9
sw $k1 0($k0)
li $t9 8
sll $t9 $t9 2
la $t8 str
add $k0 $t8 $t9
lw $k0, 0($k0)
addi $t1 $k0 0
sll $t9 $t1 2
la $t8 abc
add $k0 $t8 $t9
lw $k0, 0($k0)
addi $t2 $k0 0
sll $t9 $t2 2
la $t8 str
add $k0 $t8 $t9
lw $k0, 0($k0)
addi $t3 $k0 0
move $v1 $t3
li $k1 0
sll $t9 $v1 2
la $t8 str
add $k0 $t8 $t9
sw $k1 0($k0)
li $t9 5
sll $t9 $t9 2
la $t8 abc
add $k0 $t8 $t9
lw $k0, 0($k0)
addi $t4 $k0 0
sll $t9 $t4 2
la $t8 abc
add $k0 $t8 $t9
lw $k0, 0($k0)
addi $t5 $k0 0
move $v1 $t5
li $k1 0
sll $t9 $v1 2
la $t8 abc
add $k0 $t8 $t9
sw $k1 0($k0)
li $t9 7
sll $t9 $t9 2
la $t8 str
add $k0 $t8 $t9
lw $k0, 0($k0)
addi $t6 $k0 0
sll $t9 $t6 2
la $t8 abc
add $k0 $t8 $t9
lw $k0, 0($k0)
addi $t7 $k0 0
sll $t9 $t7 2
la $t8 str
add $k0 $t8 $t9
lw $k0, 0($k0)
addi $t0 $k0 0
move $v1 $t0
li $k1 0
sll $t9 $v1 2
la $t8 abc
add $k0 $t8 $t9
sw $k1 0($k0)
li $t9 8
sll $t9 $t9 2
la $t8 abc
add $k0 $t8 $t9
lw $k0, 0($k0)
addi $t1 $k0 0
sll $t9 $t1 2
la $t8 abc
add $k0 $t8 $t9
lw $k0, 0($k0)
addi $t2 $k0 0
sll $t9 $t2 2
la $t8 abc
add $k0 $t8 $t9
lw $k0, 0($k0)
addi $t3 $k0 0
sll $t9 $t3 2
la $t8 str
add $k0 $t8 $t9
lw $k0, 0($k0)
addi $t4 $k0 0
sll $t9 $t4 2
la $t8 str
add $k0 $t8 $t9
lw $k0, 0($k0)
addi $t5 $k0 0
move $v1 $t5
li $t9 3
sll $t9 $t9 2
la $t8 str
add $k0 $t8 $t9
lw $k0, 0($k0)
addi $t6 $k0 0
sll $t9 $t6 2
la $t8 abc
add $k0 $t8 $t9
lw $k0, 0($k0)
addi $t7 $k0 0
sll $t9 $t7 2
la $t8 str
add $k0 $t8 $t9
lw $k0, 0($k0)
addi $t0 $k0 0
li $t9 7
sll $t9 $t9 2
la $t8 abc
add $k0 $t8 $t9
lw $k0, 0($k0)
addi $t1 $k0 0
sll $t9 $t1 2
la $t8 str
add $k0 $t8 $t9
lw $k0, 0($k0)
addi $t2 $k0 0
sll $t9 $t2 2
la $t8 abc
add $k0 $t8 $t9
lw $k0, 0($k0)
addi $t3 $k0 0
sll $t9 $t0 2
la $t8 str
add $k0 $t8 $t9
lw $k0, 0($k0)
sll $t9 $t3 2
la $t8 abc
add $k1 $t8 $t9
lw $k1, 0($k1)
add $t4 $k0 $k1
sll $t9 $v1 2
la $t8 abc
add $k0 $t8 $t9
sw $t4 0($k0)
la $a0, Str9
li $v0, 4
syscall
la $a0, newLine
li $v0, 4
syscall
li $t9 8
sll $t9 $t9 2
la $t8 abc
add $k0 $t8 $t9
lw $k0, 0($k0)
addi $t5 $k0 0
sll $t9 $t5 2
la $t8 abc
add $k0 $t8 $t9
lw $k0, 0($k0)
addi $t6 $k0 0
sll $t9 $t6 2
la $t8 abc
add $k0 $t8 $t9
lw $k0, 0($k0)
addi $t7 $k0 0
sll $t9 $t7 2
la $t8 str
add $k0 $t8 $t9
lw $k0, 0($k0)
addi $t0 $k0 0
sll $t9 $t0 2
la $t8 str
add $k0 $t8 $t9
lw $k0, 0($k0)
addi $t1 $k0 0
sll $t9 $t1 2
la $t8 abc
add $k0 $t8 $t9
lw $k0, 0($k0)
addi $t2 $k0 0
move $a0 $t2
li $v0, 1
syscall
la $a0, newLine
li $v0, 4
syscall
li $t9 8
sll $t9 $t9 2
la $t8 testnum
add $k0 $t8 $t9
lw $k0, 0($k0)
addi $t3 $k0 -8
li $t9 7
sll $t9 $t9 2
la $t8 testnum
add $k0 $t8 $t9
lw $k0, 0($k0)
addi $t4 $k0 -7
div $t3 $t4
mflo $t5
li $t9 7
sll $t9 $t9 2
la $t8 testnum
add $k0 $t8 $t9
lw $k0, 0($k0)
addi $t6 $k0 -7
li $t9 6
sll $t9 $t9 2
la $t8 testnum
add $k0 $t8 $t9
lw $k0, 0($k0)
addi $t7 $k0 -6
li $t9 5
sll $t9 $t9 2
la $t8 testnum
add $k0 $t8 $t9
lw $k0, 0($k0)
addi $t0 $k0 -5
div $t7 $t0
mflo $t1
sll $t9 $t1 2
la $t8 testnum
add $k0 $t8 $t9
lw $k0, 0($k0)
addi $t2 $k0 -6
li $t9 5
sll $t9 $t9 2
la $t8 testnum
add $k0 $t8 $t9
lw $k0, 0($k0)
addi $t3 $k0 -5
sll $t9 $t5 2
la $t8 testnum
add $k0 $t8 $t9
lw $k0, 0($k0)
div $k0 $t6
mflo $t4
div $t2 $t3
mflo $t5
sub $t6 $t4 $t5
sll $t9 $t6 2
la $t8 testnum
add $k0 $t8 $t9
lw $k0, 0($k0)
addi $t7 $k0 0
move $a0 $t7
li $v0, 1
syscall
la $a0, newLine
li $v0, 4
syscall
sw $t1, -24($fp)
sw $t2, -28($fp)
sw $t3, -32($fp)
sw $a1, -4($fp)
sw $a2, -8($fp)
sw $a3, -12($fp)
li $a1 3
jal Factorial
move $t0, $v0
sll $t9 $t0 2
la $t8 testnum
add $k0 $t8 $t9
lw $k0, 0($k0)
addi $t1 $k0 0
move $a0 $t1
li $v0, 1
syscall
la $a0, newLine
li $v0, 4
syscall
