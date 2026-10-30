`timescale 1ns / 1ps

module tb_PreProcess();

    // 1. 信号定义 (输入用 reg，输出用 wire)
    reg         clk;
    reg         rst_n;
    reg signed [23:0] tb_das_data;
    reg         tb_das_valid;

    wire [7:0]  out_tdata;
    wire        out_tvalid;
    wire        out_tlast;

    // 2. 例化我们刚才写的顶层模块
    PreProcess_Top u_top (
        .clk          (clk),
        .rst_n        (rst_n),
        .das_data     (tb_das_data),
        .das_valid    (tb_das_valid),
        .m_axis_tdata (out_tdata),
        .m_axis_tvalid(out_tvalid),
        .m_axis_tlast (out_tlast)
    );

    // 3. 生成 50MHz 时钟 (周期 20ns，半周期 10ns)
    initial begin
        clk = 0;
        forever #10 clk = ~clk;
    end

    // 4. 编写测试激励流程
    initial begin
        // 初始化信号
        rst_n        = 0;
        tb_das_data  = 24'd0;
        tb_das_valid = 0;

        // 保持复位一段时间
        #100;
        rst_n = 1; // 释放复位
        #100;

        // -----------------------------------------------------
        // 模拟：DAS_Core 开始输出一帧图像的数据
        // 我们强行塞入一些包含正负数的数据，验证整流和滤波
        // 注意：因为降采样是 4 倍，我们至少发几千个数据进去
        // -----------------------------------------------------
        $display("Simulation Start: Sending DAS Data...");
        
        repeat (30000) begin // 循环 30000 次，模拟产生 30000 个时钟的数据
            @(posedge clk);
            tb_das_valid <= 1'b1;
            
            // 之前是 tb_das_data <= $random % 100;
            // 现在直接给一个占满 24 位的巨大震荡信号
            tb_das_data <= $random % 8000000 + 8000000;
        end

        // 停止发送数据
        @(posedge clk);
        tb_das_valid <= 1'b0;
        tb_das_data  <= 24'd0;

        // 等待打包模块把缓冲区里的数据处理完并发出 TLAST
        #5000;
        
        $display("Simulation Finished.");
        $stop; // 暂停仿真
    end

endmodule