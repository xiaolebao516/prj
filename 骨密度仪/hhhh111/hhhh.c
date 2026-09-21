#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "pico/time.h"
#include "blink.pio.h"


#define SM_RX1           0            // PIO SM 读取数据并 push RX FIFO
#define SM_RX2           1
#define SM_TRI           2
#define SM_PULSE         2
#define DATA_BITS          12


#define PULSE              40
#define SPI_MOSI           39
#define SPI_CS             37
#define SPI_SCLK           38
#define PIN_CLKA            2
#define PIN_CLKB            3
#define PIN_DATA_BASE1      4
#define PIN_DATA_BASE2      16
#define PIN_SYNC            30

#define SAMPLE_COUNT      2000

#define ADC_FS_HZ       20000000u

const float clk_div = 1.0f;
static uint off_rx1, off_rx2, off_pulse, off_trigger;
dma_channel_config c1,c2;
static uint16_t buffer1[SAMPLE_COUNT];
static uint16_t buffer2[SAMPLE_COUNT];
static uint16_t buffer3[SAMPLE_COUNT];
static uint16_t buffer4[SAMPLE_COUNT];

const uint32_t pat2 = 0xffff6f9f;
const uint32_t pat1 = 0xfffff6f9;

PIO pio_adc = pio0;
PIO pio_hi = pio1;

void dac_init()
{
    gpio_init(SPI_MOSI);
    gpio_init(SPI_CS);
    gpio_init(SPI_SCLK);
    gpio_set_dir(SPI_MOSI, GPIO_OUT);//设置IO为输入或输出
    gpio_set_dir(SPI_CS, GPIO_OUT);
    gpio_set_dir(SPI_SCLK, GPIO_OUT);
    gpio_put(SPI_MOSI, 0);//设置输出的高低电平
    gpio_put(SPI_CS, 1);
    gpio_put(SPI_SCLK, 0);
}
void dac_data_calculation(uint16_t *data, uint16_t input)
{
    input &= 0x0FFF;
    uint16_t PD = 0;      // PD1=0,PD0=0 → Normal mode
    
    *data = (PD << 14) | (input << 2);
}
void dac_spi_write(uint16_t data)
{
    for (int16_t bit = 15; bit >= 0; --bit)
    {
        gpio_put(SPI_MOSI, (data >> bit) & 1);
        sleep_us(2);
        gpio_put(SPI_SCLK, 1);//4us的时钟
        sleep_us(2);
        gpio_put(SPI_SCLK, 0);
        sleep_us(2);
    }
}
void dac_write(uint16_t data)
{
    gpio_put(SPI_CS, 0);
    dac_spi_write(data);
    gpio_put(SPI_CS, 1);
}
void dac(const uint16_t *input)
{
    uint16_t code = *input;
    uint16_t data;
    //printf("DAC writing started\n");
    dac_data_calculation(&data, code);
    dac_write(data);
    //printf("DAC writing ended\n");
}

void AB_CD(uint16_t *input, uint dma_ch1, uint dma_ch2){
    dac(input);
 
    
    pio_sm_clear_fifos(pio_adc, SM_RX1);
    pio_sm_clear_fifos(pio_adc, SM_RX2);
    pio_sm_clear_fifos(pio_adc, SM_TRI);
    pio_sm_clear_fifos(pio_hi, SM_PULSE);
    
    //DMA
    c1 = dma_channel_get_default_config(dma_ch1);
    channel_config_set_transfer_data_size(&c1, DMA_SIZE_16);
    channel_config_set_read_increment(&c1, false);
    channel_config_set_write_increment(&c1, true);
    channel_config_set_dreq(&c1, pio_get_dreq(pio_adc, SM_RX1, false));
    dma_channel_configure(dma_ch1, &c1, buffer1, &pio_adc->rxf[SM_RX1], SAMPLE_COUNT, false);
    
    c2 = dma_channel_get_default_config(dma_ch2);
    channel_config_set_transfer_data_size(&c2, DMA_SIZE_16);
    channel_config_set_read_increment(&c2, false);
    channel_config_set_write_increment(&c2, true);
    channel_config_set_dreq(&c2, pio_get_dreq(pio_adc, SM_RX2, false));
    dma_channel_configure(dma_ch2, &c2, buffer2, &pio_adc->rxf[SM_RX2], SAMPLE_COUNT, false);
    gpio_put(PIN_SYNC, 0);
    //使能pio状态机
    pio_enable_sm_mask_in_sync(pio_adc, (1u<<SM_RX2) | (1u<<SM_RX1));
    pio_sm_set_enabled(pio_adc, SM_TRI, true);
    pio_sm_set_enabled(pio_hi, SM_PULSE, true);

    dma_start_channel_mask((1u << dma_ch1) | (1u << dma_ch2));

    //sleep_us(10); 
    
    pio_sm_put_blocking(pio_adc, SM_RX2, SAMPLE_COUNT);
    pio_sm_put_blocking(pio_adc, SM_RX1, SAMPLE_COUNT);
    pio_sm_put_blocking(pio_adc, SM_TRI, 1);
    //sleep_us(10);
    pio_sm_put_blocking(pio_hi, SM_PULSE, pat1);
    sleep_us(500);

    dma_channel_wait_for_finish_blocking(dma_ch1);
    dma_channel_wait_for_finish_blocking(dma_ch2);

    pio_sm_set_enabled(pio_adc, SM_RX1, false);
    pio_sm_set_enabled(pio_adc, SM_RX2, false);
    pio_sm_set_enabled(pio_adc, SM_TRI, false);
    pio_sm_set_enabled(pio_hi, SM_PULSE, false);

    sleep_us(200);//延时，开始B发CD收==================================

    pio_pulse_init(pio_hi, SM_PULSE, off_pulse, PULSE, PIN_SYNC);
    pio_sm_clear_fifos(pio_adc, SM_RX1);
    pio_sm_clear_fifos(pio_adc, SM_RX2);
    pio_sm_clear_fifos(pio_adc, SM_TRI);
    pio_sm_clear_fifos(pio_hi, SM_PULSE);

    dma_channel_configure(dma_ch1, &c1, buffer3, &pio_adc->rxf[SM_RX1], SAMPLE_COUNT, false);
    dma_channel_configure(dma_ch2, &c2, buffer4, &pio_adc->rxf[SM_RX2], SAMPLE_COUNT, false);
    gpio_put(PIN_SYNC, 0);
    //使能pio状态机
    pio_enable_sm_mask_in_sync(pio_adc, (1u<<SM_RX1) | (1u<<SM_RX2));
    pio_sm_set_enabled(pio_adc, SM_TRI, true);
    pio_sm_set_enabled(pio_hi, SM_PULSE, true);

    dma_start_channel_mask((1u << dma_ch1) | (1u << dma_ch2));

    pio_sm_put_blocking(pio_adc, SM_RX1, SAMPLE_COUNT);
    pio_sm_put_blocking(pio_adc, SM_RX2, SAMPLE_COUNT);
    pio_sm_put_blocking(pio_adc, SM_TRI, 1);
    //sleep_us(10);
    pio_sm_put_blocking(pio_hi, SM_PULSE, pat2);
    sleep_us(500);

    dma_channel_wait_for_finish_blocking(dma_ch1);
    dma_channel_wait_for_finish_blocking(dma_ch2);

    pio_sm_set_enabled(pio_adc, SM_RX1, false);
    pio_sm_set_enabled(pio_adc, SM_RX2, false);
    pio_sm_set_enabled(pio_adc, SM_TRI, false);
    pio_sm_set_enabled(pio_hi, SM_PULSE, false);
}

typedef struct __attribute__((packed)){
    uint8_t magic[2];
    uint16_t gain;
    uint16_t idx;
    uint8_t ch;
    uint16_t length;
}FRAME;

void send_frame(uint16_t Gain,uint16_t Idx,uint8_t Ch,uint16_t *data,uint16_t Count)
{
    uint8_t head[2] = {0xAA, 0x55};
    uint8_t tail[2] = {0xEE, 0xEE};
    uint16_t length = Count;

    fwrite(head, 1, 2, stdout);                   // AA55
    fwrite(&Gain, sizeof(uint16_t), 1, stdout);   // gain (2 bytes)
    fwrite(&Idx, sizeof(uint16_t), 1, stdout);    // frameID (2 bytes)
    fwrite(&Ch, 1, 1, stdout);                    // ch (1 byte)
    fwrite(&length, sizeof(uint16_t), 1, stdout); // sample count (2 bytes)
    fwrite(data, sizeof(uint16_t), Count, stdout);// payload
    fwrite(tail, 1, 2, stdout);                   // EE EE

    fflush(stdout);
}



void send_all_channels(uint16_t Gain ,uint16_t frame_idx)
{
    send_frame(Gain, frame_idx, 1, buffer1, SAMPLE_COUNT);
    sleep_ms(1);
    send_frame(Gain, frame_idx, 2, buffer2, SAMPLE_COUNT);
    sleep_ms(1);
    send_frame(Gain, frame_idx, 3, buffer3, SAMPLE_COUNT);
    sleep_ms(1);
    send_frame(Gain, frame_idx, 4, buffer4, SAMPLE_COUNT);
}

typedef struct __attribute__((packed)){
    uint8_t magic[2];    // 0xA5 0x5A
    uint16_t gain;        // 增益值
    uint16_t frame_idx;   // 当前第几帧
} CMD_FRAME;

int read_frame_cmd_blocking(CMD_FRAME *cmd){
    uint8_t buf[sizeof(CMD_FRAME)];

    size_t got = 0;

    // 朴素阻塞：不停读 getchar_timeout_us(-1)
    while (got < sizeof(buf)) {
        int c = getchar_timeout_us(1000000); // 1ms * 1000 = 1s超时
        if (c == PICO_ERROR_TIMEOUT) {
            // 如果你希望死等而不是1秒超时，就把上面参数改成 0xFFFFFFFF 或用另一个循环
            continue;
        }
        buf[got++] = (uint8_t)c;
    }

    memcpy(cmd, buf, sizeof(CMD_FRAME));
    if((cmd->magic[0]!=0xA5)||(cmd->magic[1]!=0x5A)){
        return 0;
    }
    return 1;
}

int main(){
    stdio_init_all();
    #if PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS>0
    if(stdio_usb_connected()){
        stdio_set_translate_crlf(&stdio_usb,false);
    }
    #else
        stdio_set_translate_crlf(&stdio_usb,false);
    #endif
    set_sys_clock_khz(125000, true);
    dac_init();
    sleep_ms(10);
    //等待 AD9238 参考充电稳定（datasheet 建议 >=5 ms）
    sleep_ms(6);
    gpio_init(PIN_SYNC);
    gpio_set_dir(PIN_SYNC, GPIO_OUT);
    gpio_put(PIN_SYNC, 0);
    uint dma_ch1=1, dma_ch2=2;
    uint16_t g=0x0000;
    //uint16_t g;
    uint16_t i;
    pio_set_gpio_base(pio_hi, 16);
    off_rx1 = pio_add_program(pio_adc, &rx_prog1_program);
    off_rx2 = pio_add_program(pio_adc, &rx_prog2_program);
    off_trigger = pio_add_program(pio_adc, &trigger_prog_program);
    off_pulse = pio_add_program(pio_hi, &pulse_prog_program);
    pio_rx1_init(pio_adc, SM_RX1, off_rx1, PIN_DATA_BASE1, PIN_CLKA, DATA_BITS, clk_div);
    pio_rx2_init(pio_adc, SM_RX2, off_rx2, PIN_DATA_BASE2, PIN_CLKB, DATA_BITS, clk_div);
    pio_tr_init(pio_adc, SM_TRI, off_trigger, PIN_SYNC);
    pio_pulse_init(pio_hi, SM_PULSE, off_pulse, PULSE, PIN_SYNC);
    //gpio_put(5, 1);
    //gpio_put(7, 0);
    //gpio_put(34, 0);
    sleep_us(100);
    dac(&g);

    //for(int i=0;i<8;i++){
    //    gpio_init(PULSE+i);
    //    gpio_set_dir(PULSE+i, GPIO_OUT);
    //}
    //sleep_ms(100);
    

    while(1){
        //gpio_put(PULSE,1);
        //gpio_put(PULSE+1,1);
        //gpio_put(PULSE+2,1);
        //gpio_put(PULSE+3,0);
        //gpio_put(PULSE+4,1);
        //gpio_put(PULSE+5,1);
        //gpio_put(PULSE+6,0);
        //gpio_put(PULSE+7,1);
        //sleep_ms(50);


        CMD_FRAME cmd;
        
        if (read_frame_cmd_blocking(&cmd)) {
            //printf("Got: %02X %02X  gain=%u idx=%u\n",
            //cmd.magic[0], cmd.magic[1],
            //cmd.gain,
            //cmd.frame_idx);
            fflush(stdout);
        }   
        g = cmd.gain;
        i = cmd.frame_idx;
        pio_interrupt_clear(pio_adc, 0);  // 推荐加在这里

        AB_CD(&g, dma_ch1, dma_ch2);
        send_all_channels(g, i);
        
    
    }    

    return 0;
}