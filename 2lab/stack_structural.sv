module stack_structural_lite(
    output wire[3:0] O_DATA, 
    input wire RESET, 
    input wire CLK, 
    input wire[1:0] COMMAND, 
    input wire[2:0] INDEX,
    input wire[3:0] I_DATA
    ); 

    wire push_wire, pop_wire, get_wire, free_wire;
    parse_command parser(COMMAND, push_wire, pop_wire, get_wire, free_wire);

    wire[2:0] push_incr_idx, pop_incr_idx;
    wire is_incr, is_decr;
    and and_increment(is_incr, push_wire, CLK);
    and and_decrement(is_decr, pop_wire, CLK);
    incr_index incr_push(is_incr, RESET, push_incr_idx);
    incr_index decr_push(is_decr, RESET, pop_incr_idx);

    wire[2:0] current_idx;
    wire c_ou;
    adder_subtractor diff_idx(push_incr_idx, pop_incr_idx, current_idx, c_ou);

    wire flag_0, flag_1, flag_2, flag_3, flag_4, flag_5, flag_6, flag_7;
    wire idx_0, idx_1, idx_2, idx_3, idx_4, idx_5, idx_6, idx_7;
    index_0 i0(idx_0, current_idx);
    index_1 i1(idx_1, current_idx);
    index_2 i2(idx_2, current_idx);
    index_3 i3(idx_3, current_idx);
    index_4 i4(idx_4, current_idx);
    index_5 i5(idx_5, current_idx);
    index_6 i6(idx_6, current_idx);
    index_7 i7(idx_7, current_idx);

    and and0(flag_0, idx_0, push_wire);
    and and1(flag_1, idx_1, push_wire);
    and and2(flag_2, idx_2, push_wire);
    and and3(flag_3, idx_3, push_wire);
    and and4(flag_4, idx_4, push_wire);
    and and5(flag_5, idx_5, push_wire);
    and and6(flag_6, idx_6, push_wire);
    and and7(flag_7, idx_7, push_wire);

    wire[3:0] elem_0, elem_1, elem_2, elem_3, elem_4, elem_5, elem_6, elem_7;

    storage_bits st0(CLK, flag_0, RESET, I_DATA, elem_0);
    storage_bits st1(CLK, flag_1, RESET, I_DATA, elem_1);
    storage_bits st2(CLK, flag_2, RESET, I_DATA, elem_2);
    storage_bits st3(CLK, flag_3, RESET, I_DATA, elem_3);
    storage_bits st4(CLK, flag_4, RESET, I_DATA, elem_4);
    storage_bits st5(CLK, flag_5, RESET, I_DATA, elem_5);
    storage_bits st6(CLK, flag_6, RESET, I_DATA, elem_6);
    storage_bits st7(CLK, flag_7, RESET, I_DATA, elem_7);

    wire[2:0] get_need_idx;
    wire[2:0] converted_bits;

    convert_to_3_bit convb(get_wire, converted_bits);
    wire[2:0] res_idx_;
    and_3_2 anresidx(res_idx_, converted_bits, INDEX);
    wire c_out;

    adder_subtractor ad1(current_idx, res_idx_, get_need_idx, c_out);

    wire[2:0] real_index_with_command;
    get_real_index get1(get_need_idx, real_index_with_command);

    wire is_idx_0, is_idx_1, is_idx_2, is_idx_3, is_idx_4, is_idx_5, is_idx_6, is_idx_7;
    index_0 is0(is_idx_0, real_index_with_command);
    index_1 is1(is_idx_1, real_index_with_command);
    index_2 is2(is_idx_2, real_index_with_command);
    index_3 is3(is_idx_3, real_index_with_command);
    index_4 is4(is_idx_4, real_index_with_command);
    index_5 is5(is_idx_5, real_index_with_command);
    index_6 is6(is_idx_6, real_index_with_command);
    index_7 is7(is_idx_7, real_index_with_command);

    wire[3:0] res_0, res_1, res_2, res_3, res_4, res_5, res_6, res_7;
    identification iden0(elem_0, is_idx_0, res_0);
    identification iden1(elem_1, is_idx_1, res_1);
    identification iden2(elem_2, is_idx_2, res_2);
    identification iden3(elem_3, is_idx_3, res_3);
    identification iden4(elem_4, is_idx_4, res_4);
    identification iden5(elem_5, is_idx_5, res_5);
    identification iden6(elem_6, is_idx_6, res_6);
    identification iden7(elem_7, is_idx_7, res_7);

    wire[3:0] answer;
    get_result ans(res_0, res_1, res_2, res_3, res_4, res_5, res_6, res_7, answer);

    wire sync;

    wire operation;
    or oper_or(operation, pop_wire, get_wire);
    and andr(sync, operation, CLK);

    supply0 grd;
    storage_bits odata(sync, sync, grd, answer, O_DATA);

endmodule

module and_3(output wire out_d, input wire[2:0] in_d);
    and and1(temp1, in_d[1], in_d[0]);
    and and2(out_d, temp1, in_d[2]);

endmodule;

module and_3_1(output wire out_d, input wire in1, input wire in2, input wire in3);
    and and1(temp1, in1, in2);
    and and2(out_d, temp1, in3);

endmodule;

module and_3_2(output wire[2:0] out_d, input wire[2:0] in1, input wire[2:0] in2);
    and and1(out_d[0], in1[0], in2[0]);
    and and2(out_d[1], in1[1], in2[1]);
    and and3(out_d[2], in1[2], in2[2]);

endmodule;

module index_0(output wire out_d, input wire [2:0] in_d);
    wire [2:0] nwire;

    not not1(nwire[0], in_d[0]);
    not not2(nwire[1], in_d[1]);
    not not3(nwire[2], in_d[2]);

    and_3 and_3_1(out_d, nwire);
endmodule

module index_1(output wire out_d, input wire [2:0] in_d);
    wire [2:0] nwire;

    not not1(nwire[0], in_d[0]);
    not not2(nwire[1], in_d[1]);
    not not3(nwire[2], in_d[2]);

    and_3_1 and_3_1_(out_d, in_d[0], nwire[1], nwire[2]);
endmodule

module index_2(output wire out_d, input wire [2:0] in_d);
    wire [2:0] nwire;

    not not1(nwire[0], in_d[0]);
    not not2(nwire[1], in_d[1]);
    not not3(nwire[2], in_d[2]);

    and_3_1 and_3_1_(out_d, nwire[0], in_d[1], nwire[2]);
endmodule

module index_3(output wire out_d, input wire [2:0] in_d);
    wire [2:0] nwire;

    not not1(nwire[0], in_d[0]);
    not not2(nwire[1], in_d[1]);
    not not3(nwire[2], in_d[2]);

    and_3_1 and_3_1_(out_d, in_d[0], in_d[1], nwire[2]);
endmodule

module index_4(output wire out_d, input wire [2:0] in_d);
    wire [2:0] nwire;

    not not1(nwire[0], in_d[0]);
    not not2(nwire[1], in_d[1]);
    not not3(nwire[2], in_d[2]);

    and_3_1 and_3_1_(out_d, nwire[0], nwire[1], in_d[2]);
endmodule

module index_5(output wire out_d, input wire [2:0] in_d);
    wire [2:0] nwire;

    not not1(nwire[0], in_d[0]);
    not not2(nwire[1], in_d[1]);
    not not3(nwire[2], in_d[2]);

    and_3_1 and_3_1_(out_d, in_d[0], nwire[1], in_d[2]);
endmodule

module index_6(output wire out_d, input wire [2:0] in_d);
    wire [2:0] nwire;

    not not1(nwire[0], in_d[0]);
    not not2(nwire[1], in_d[1]);
    not not3(nwire[2], in_d[2]);

    and_3_1 and_3_1_(out_d, nwire[0], in_d[1], in_d[2]);
endmodule

module index_7(output wire out_d, input wire [2:0] in_d);
    wire [2:0] nwire;

    not not1(nwire[0], in_d[0]);
    not not2(nwire[1], in_d[1]);
    not not3(nwire[2], in_d[2]);

    and_3_1 and_3_1(out_d, in_d[0], in_d[1], in_d[2]);
endmodule

module rs(input wire r, input wire s, input wire reset, output wire q, output wire nq);
    wire temp;

    or or1(nq, reset, temp);
    nor nor1(q, r, nq);
    nor nor2(temp, s, q);

endmodule

module sync_rs(input wire r, input wire s, input wire reset, input wire clk, output wire q, output wire nq);
    wire temp1, temp2;

    and and1(temp1, r, clk);
    and and2(temp2, s, clk);

    rs rs_sync(temp1, temp2, reset, q, nq);

endmodule

module d_trigger(input wire d, input wire clk, input wire reset, output wire q, output wire nq);
    wire nd;
    not not1(nd, d);
    sync_rs rs_sync(nd, d, reset, clk, q, nq);

endmodule

module jk_trigger(input wire k, input wire j, input wire clk, input wire reset, output wire q, output wire nq);

    wire temp1, temp2;
    wire k1, j1;

    and and1(k1, q, k);
    and and2(j1, j, nq);

    sync_rs srs1(k1, j1, reset, clk, temp1, temp2);
    wire nclk;
    not not_clk(nclk, clk);
    sync_rs srs2(temp2, temp1, reset, nclk, q, nq);

endmodule

module parse_command(input wire[1:0] in, output wire push, output wire pop, output wire get, output wire free);

    wire nin0, nin1;
    not not0(nin0, in[0]);
    not not1(nin1, in[1]);

    and and1(push, in[0], nin1);
    and and2(pop, nin0, in[1]);
    and and3(get, in[0], in[1]);
    and and4(free, nin0, nin1);

endmodule

module storage_bits(input wire clk, input wire key, input wire reset, input wire[3:0] data, output wire[3:0] out_data);
    wire new_sync;
    and and1(new_sync, clk, key);
    wire[3:0] temp_data;

    d_trigger d1(data[0], new_sync, reset, out_data[0], temp_data[0]);
    d_trigger d2(data[1], new_sync, reset, out_data[1], temp_data[1]);
    d_trigger d3(data[2], new_sync, reset, out_data[2], temp_data[2]);
    d_trigger d4(data[3], new_sync, reset, out_data[3], temp_data[3]);

endmodule

module full_adder(input wire a, input wire b, input wire c_in, output wire summ, output wire c_out);
    wire temp1, temp2, temp3, temp4;

    xor xor1(temp1, a, b);
    xor xor2(temp2, a, b);

    xor xor3(summ, temp1, c_in);

    and and1(temp3, c_in, temp2);
    and and2(temp4, a, b);

    or or1(c_out, temp3, temp4);


endmodule;

module adder_subtractor(input wire[2:0] first, input wire[2:0] second, output wire[2:0] res, output wire c_out);

    wire temp1, temp2, temp3;
    wire bit_ = 1'b1;

    xor xor1(temp1, second[0], bit_);
    xor xor2(temp2, second[1], bit_);
    xor xor3(temp3, second[2], bit_);

    wire out1, out2;
    full_adder ad1(first[0], temp1, bit_, res[0], out1);
    full_adder ad2(first[1], temp2, out1, res[1], out2);
    full_adder ad3(first[2], temp3, out2, res[2], c_out);

endmodule;

module get_result(input wire[3:0] i1, input wire[3:0] i2, input wire[3:0] i3, input wire[3:0] i4, input wire[3:0] i5, input wire[3:0] i6, input wire[3:0] i7, input wire[3:0] i8, output wire[3:0] out);

    or or0(out[0], i1[0], i2[0], i3[0], i4[0], i5[0], i6[0], i7[0], i8[0]);
    or or1(out[1], i1[1], i2[1], i3[1], i4[1], i5[1], i6[1], i7[1], i8[1]);
    or or2(out[2], i1[2], i2[2], i3[2], i4[2], i5[2], i6[2], i7[2], i8[2]);
    or or3(out[3], i1[3], i2[3], i3[3], i4[3], i5[3], i6[3], i7[3], i8[3]);

endmodule

module identification(input wire[3:0] data, input wire flag, output wire[3:0] res);

    and and0(res[0], data[0], flag);
    and and1(res[1], data[1], flag);
    and and2(res[2], data[2], flag);
    and and3(res[3], data[3], flag);

endmodule

module incr_index(input wire clk, input wire reset, output wire[2:0] res_idx);

    wire[2:0] temp;
    supply1 pwr;
    wire new_k;
    and and1(new_k, res_idx[0], res_idx[1]);

    jk_trigger jk1(pwr, pwr, clk, reset, res_idx[0], temp[0]);
    jk_trigger jk2(res_idx[0], res_idx[0], clk, reset, res_idx[1], temp[1]);
    jk_trigger jk3(new_k, new_k, clk, reset, res_idx[2], temp[2]);

endmodule

module get_real_index(input wire[2:0] in_data, output wire[2:0] real_idx);
    wire c_out;
    reg[2:0] bit_ = 3'b1;

    adder_subtractor adder(in_data, bit_, real_idx, c_out);

endmodule

module convert_to_3_bit(input wire in, output wire[2:0] out);
    assign out[0] = in;
    assign out[1] = in;
    assign out[2] = in;

endmodule
