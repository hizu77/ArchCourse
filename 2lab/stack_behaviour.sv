module stack_behaviour_lite(
    output wire[3:0] O_DATA, 
    input wire RESET, 
    input wire CLK, 
    input wire[1:0] COMMAND, 
    input wire[2:0] INDEX,
    input wire[3:0] I_DATA
    ); 

    reg[3:0] full_data[7:0];

    reg[3:0] cur_index = 0;
    reg[3:0] push_count = 0;
    reg[3:0] pop_count = 0;
    reg[3:0] index_command = 0;

    reg[3:0] answer;

    assign O_DATA = answer;

    always @(CLK or RESET) begin
        if (RESET) begin
            push_count = 0;
            pop_count = 0;
            cur_index = 0;

            for (integer i = 0; i < 8; i += 1) begin
                full_data[i] = 0;
            end
        end    

        else if (CLK) begin
            case (COMMAND)
                2'b01: begin
                    push_count += 1;
                    push_count %= 8;
                    cur_index = (push_count > pop_count ? push_count - pop_count - 1 : push_count - pop_count + 7);
                    full_data[cur_index] = I_DATA;

                end


                2'b10: begin
                    answer = full_data[cur_index];
                    pop_count += 1;
                    pop_count %= 8;
                    cur_index = (push_count > pop_count ? push_count - pop_count - 1 : push_count - pop_count + 7);

                end

                2'b11: begin
                    index_command = INDEX;
                    index_command = (index_command > cur_index ? cur_index - index_command + 8 : cur_index - index_command);
                    answer = full_data[index_command];
                    end
            endcase
        end
    end
endmodule