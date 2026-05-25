`timescale 1ns / 1ps

module tb_DAS_Core_Multi();

    // ==========================================
    // 1. 定义测试台的参数 (和源文件保持一致)
    // ==========================================
    parameter CH_NUM       = 8;
    parameter ADC_WIDTH    = 12;
    parameter DELAY_WIDTH  = 10;
    parameter WEIGHT_WIDTH = 8;

    // ==========================================
    // 2. 声明仪表的连线
    // ==========================================
    reg clk;
    reg rst_n;
    reg adc_valid;
    
    // 展平的极宽总线
    reg [CH_NUM * ADC_WIDTH - 1 : 0]    adc_data_in;
    reg [CH_NUM * DELAY_WIDTH - 1 : 0]  delay_in;
    reg [CH_NUM * WEIGHT_WIDTH - 1 : 0] weight_in;

    // 接收输出结果
    wire [(ADC_WIDTH + WEIGHT_WIDTH + 3) - 1 : 0] beamformed_out;
    wire out_valid;

    // ==========================================
    // 3. 把 8 通道核心模块搬上测试台
    // ==========================================
    DAS_Core #(
        .CH_NUM(CH_NUM),
        .ADC_WIDTH(ADC_WIDTH),
        .DELAY_WIDTH(DELAY_WIDTH),
        .BRAM_DEPTH(1024),
        .WEIGHT_WIDTH(WEIGHT_WIDTH)
    ) uut (
        .clk(clk),
        .rst_n(rst_n),
        .adc_valid(adc_valid),
        .adc_data_in(adc_data_in),
        .delay_in(delay_in),
        .weight_in(weight_in),
        .beamformed_out(beamformed_out),
        .out_valid(out_valid)
    );

    // ==========================================
    // 4. 供电：生成 50MHz 时钟
    // ==========================================
    initial clk = 0;
    always #10 clk = ~clk;

    // 辅助变量
    integer i;
    reg [ADC_WIDTH-1:0] test_val;

    // ==========================================
    // 5. 核心测试逻辑 (喂数据)
    // ==========================================
    initial begin
        // --- 初始化清零 ---
        rst_n = 0;
        adc_valid = 0;
        adc_data_in = 0;
        delay_in = 0;
        weight_in = 0;
        test_val = 0;

        // -----------------------------------------------------------
        // 【关键动作：模拟 PS 端的 C 语言，提前配好延迟和权重】
        // 我们给 8 个通道统一配置：延迟 = 5， 权重 = 2
        // -----------------------------------------------------------
        for (i = 0; i < CH_NUM; i = i + 1) begin
            delay_in [i * DELAY_WIDTH +: DELAY_WIDTH]   = 10'd5;
            weight_in[i * WEIGHT_WIDTH +: WEIGHT_WIDTH] = 8'd2;
        end

        // 松开复位，系统激活
        #100;
        rst_n = 1;
        #20;

        // -----------------------------------------------------------
        // 【关键动作：模拟超声探头开始疯狂回传数据】
        // -----------------------------------------------------------
        adc_valid = 1;
        
        // 连续灌入 20 个递增的数据 (1, 2, 3, 4...)
        repeat(20) begin
            test_val = test_val + 1; // 模拟数据的变化
            
            // 把当前的数字，同时赋值给 8 个通道的输入端
            for (i = 0; i < CH_NUM; i = i + 1) begin
                adc_data_in[i * ADC_WIDTH +: ADC_WIDTH] = test_val;
            end
            
            #20; // 等待一个时钟周期
        end

        // 测试结束
        adc_valid = 0;
        #100;
        $finish;
    end

endmodule