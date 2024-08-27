`include "stack_behaviour.sv"

`define NOP 2'b00
`define PUSH 2'b01 
`define POP 2'b10
`define GET 2'b11

module stack_behaviour_tb;
    reg[3:0] I_DATA; wire[3:0] O_DATA;
    reg RESET, CLK;
    reg[1:0] COMMAND; 
    reg[2:0] INDEX;
    integer i, k, fd, TIMES; string OUTCSV;

    assign O_DATA = I_DATA;
    stack_behaviour_normal _stack(.IO_DATA(O_DATA), .RESET(RESET), .CLK(CLK), .COMMAND(COMMAND), .INDEX(INDEX));

    initial begin
        if (!$value$plusargs("TIMES=%d", TIMES))
            TIMES = 5;
        if (!$value$plusargs("OUTCSV=%s", OUTCSV))
            OUTCSV = "st_stack_b.csv";
        $display("tb -> %s", OUTCSV);
            $display("TIMES -> %d", TIMES);
        $display("T\tO_DATA\tRESET\tCLK\tCOMMAND\tINDEX\tI_DATA");
        $monitor("%2d\t%5b\t%3b\t%6b\t%4b\t%4b\t%5b", $time, O_DATA, RESET, CLK, COMMAND, INDEX, I_DATA); 
        fd = $fopen(OUTCSV, "w");
        $fdisplay(fd, "T\tO_DATA\tRESET\tCLK\tCOMMAND\tINDEX\tI_DATA");
        k = (TIMES < 5) ? 5 : TIMES;
    end
    
    always #1 CLK = ~CLK;

    initial begin      
        CLK = 0; COMMAND = `NOP; RESET = 1; INDEX = 0; 
        #2 RESET = 0;
        for (i = 0; i < TIMES; i+=1) begin
            #2; COMMAND = `PUSH; #0.01 I_DATA = i+1;
        end
        #2 COMMAND = `NOP;
        #2 I_DATA = 4'b?;
        for (i = 0; i < k; i+=1) begin
            #2; COMMAND = `GET; INDEX = i;
        end
        #2 I_DATA = 4'b?; COMMAND = `NOP;
        for (i = 0; i < TIMES; i+=1) begin
            #2; COMMAND = `POP;       
            #2; COMMAND = `GET; INDEX = i;              
        end
        #2; COMMAND = `NOP; RESET = 1;             
        #2; RESET = 0;  
        #2 I_DATA = 4'b?; COMMAND = `NOP;
        for (i = 0; i < TIMES; i+=1) begin
            #2; COMMAND = `POP;                            
        end
        
        $fclose(fd);
        $finish;
    end

    always @(CLK) begin
        $fstrobe(fd, "%d\t%b\t%b\t%b\t%b\t%b\t%b", $time, O_DATA, RESET, CLK, COMMAND, INDEX, I_DATA);
    end

endmodule