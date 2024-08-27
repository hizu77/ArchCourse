// a: 0x100
// b: 0x900
// c: 0x1800

//mmul
lui t0, 0x100        // 0x0               // int8 *pa = a;
srli t0, t0, 12      // 0x4 = 0x0 + 4     // right shift = 12
lui t2, 0x1800       // 0x8               // int32 *pc = c;
srli t2, t2, 12      // 0xc = 0x4 + 4     // right shift = 12

addi t4, zero, 0     // 0x10 = 0xc + 4    // int y = 0;
//loop1.begin
addi t6, zero, 64    // 0x14 = 0x10 + 4   // t6 = M = 64
beq t4, t6, 0x88     // 0x18 = 0x14 + 4   // y < M -> +4
                     // y == M -> 0x18 + 0x88 = 0xa0 = loop1.end

addi t3, zero, 0     // 0x1c = 0x18 + 4   // int x = 0;
//loop2.begin
addi t6, zero, 60    // 0x20 = 0x1c + 4   // t6 = N = 60
beq t3, t6, 0x68     // 0x24 = 0x20 + 4   // x < N -> +4
                     // x == N -> 0x24 + 0x68 = 0x8c = loop2.end

lui t1, 0x900        // 0x28 = 0x24 + 4   // int16 *pb = b;
srli t1, t1, 12      // 0x2c = 0x28 + 4   // right shift = 12
addi s1, zero, 0     // 0x30 = 0x2c + 4   // int32 s = 0;

addi t5, zero, 0     // 0x34 = 0x30 + 4   // int k = 0
//loop3.begin
addi t6, zero, 32    // 0x38 = 0x34 + 4   // t6 = K = 32
beq t5, t6, 0x34     // 0x3c = 0x38 + 4   // k < K -> +4
                     // k == K -> 0x3c + 0x34 = 0x70 = loop3.end

// s += pa[k] * pb[x];
addi t6, t5, 0       // 0x40 = 0x3c + 4   // k = t5 -> t6
add t6, t6, t0       // 0x44 = 0x40 + 4   // t6 += pa = t0
lb s2, 0, t6         // 0x48 = 0x44 + 4   // s2 = pa[k]

addi t6, t3, 0       // 0x4c = 0x48 + 4   // x = t3 -> t6
slli t6, t6, 1       // 0x50 = 0x4c + 4   // t6 *= 2 // left shift = 1
add t6, t6, t1       // 0x54 = 0x50 + 4   // t6 += pb = t1
lh s3, 0, t6         // 0x58 = 0x50 + 4   // s3 = pb[x]

mul t6, s2, s3       // 0x5c = 0x58 + 4   // t6 = pa[k] * pb[x]
add s1, s1, t6       // 0x60 = 0x5c + 4   // s += pa[k] * pb[x];

// pb += N;
addi t1, t1, 120     // 0x64 = 0x60 + 4   // pb += N;

addi t5, t5, 1       // 0x68 = 0x64 + 4   // k++
jal zero, -0x34      // 0x6c = 0x68 + 4   // jump to loop3.begin
                     // 0x6c - 0x34 = 0x38 = loop3.begin

addi zero, zero, 0   // 0x70 = 0x3c + 0x34
                     // nop = loop3.end

// pc[x] = s;
addi t6, t3, 0       // 0x74 = 0x70 + 4   // x = t3 -> t6
slli t6, t6, 2       // 0x78 = 0x74 + 4   // t6 *= 4 // left shift = 2
add t6, t6, t2       // 0x7c = 0x78 + 4   // t6 += pc
sw s1, 0, t6         // 0x80 = 0x7c + 4   // pc[x] = s;

addi t3, t3, 1       // 0x84 = 0x80 + 4   // x++
jal zero, -0x68      // 0x88 = 0x80 + 4   // jump to loop2 begin
                     // 0x88 - 0x68 = 0x20 = loop2.begin

addi zero, zero, 0   // 0x8c = 0x68 + 0x24
                     // nop == loop2.end

// pa += K;
addi t0, t0, 32      // 0x90 = 0x8c + 4   // pa += K;
// pc += N;
addi t2, t2, 240     // 0x94 = 0x90 + 4   // pc += N;

addi t4, t4, 1       // 0x98 = 0x94 + 4   // y++
jal zero, -0x88      // 0x9c = 0x98 + 4   // jump to loop1 begin
                     // 0x9c - 0x88 = 0x14 = loop1.begin

addi zero, zero, 0   // 0xa0 = 0x18 + 0x88
                     // nop  = loop1.end

jalr zero, ra, 0     // 0xa4  = 0xa0 + 4
                     // return
