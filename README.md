# ComputerArchitecture
<b> 2021.1학기 성균관대학교 컴퓨터구조개론 수업 </b>  

## Project1 : Interpret MIPS binary code  

<b>Excutable Name : mips-sim  
Example Execute : ./mips-sim test1.bin</b>

<b>Instructions To Support</b>  
add, addu, and, div, divu, jalr, jr, mfhi, mflo, mthi, mtlo, mult, multu, nor, or, sll, sllv, slt, sltu, sra, srav, srl, srlv, sub, subu, syscall, xor, addi, addiu, andi, beq, bne, lb, lbu, lh, lhu, lui, lw, ori, sb, slti, sltiu, sh, sw, xori, j, jal

<b>EXAMPLE RESULT</b>  
test1.bin  
00000000: 0022 0020 8d42 0020 2230 0008 1440 0004  
00000010: 0000 0000 03e0 0008 0000 0000 a7c4 0008  
00000020: 0013 5940 0000 000d  

test1.txt  
inst 0: 00220020 add $0, $1, $2  
inst 1: 8d420020 lw $2, 32($10)  
inst 2: 22300008 addi $16, $17, 8  
inst 3: 14400004 bne $2, $0, 4  
inst 4: 00000000 sll $0, $0, 0  
inst 5: 03e00008 jr $31  
inst 6: 00000000 sll $0, $0, 0  
inst 7: a7c40008 sh $4, 8($30)  
inst 8: 00135940 sll $11, $19, 5  
inst 9: 0000000d unknown instruction  
