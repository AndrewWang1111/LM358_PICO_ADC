#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/adc.h>
#include <hardware/vreg.h>
#include <hardware/dma.h>
#include "AudioFFT.h"
#include <iostream>
#include <vector>
extern "C"
{
#include "ssd1306.h"
}

ssd1306_t disp;
void hardwareInit();
float factor = 3.3 / 4096;
dma_channel_config dmaCfg;
uint dmaChan;
uint16_t rawBuffer[128];

int main()
{
    hardwareInit();
    const size_t fftSize = 128; // Needs to be power of 2!
    std::vector<float> input(fftSize, 0.0f);
    std::vector<float> re(audiofft::AudioFFT::ComplexSize(fftSize));
    std::vector<float> im(audiofft::AudioFFT::ComplexSize(fftSize));

    audiofft::AudioFFT fft;
    fft.init(fftSize);
    char msg[25];
    float pointY = 0.0;
    uint64_t timestamp = 0;
    float freq = 0.0f;

    dmaChan = dma_claim_unused_channel(true);
    dmaCfg = dma_channel_get_default_config(dmaChan);
    channel_config_set_read_increment(&dmaCfg, false);
    channel_config_set_write_increment(&dmaCfg, true);
    channel_config_set_transfer_data_size(&dmaCfg, DMA_SIZE_16);
    channel_config_set_dreq(&dmaCfg, DREQ_ADC);
    dma_channel_configure(dmaChan, &dmaCfg, &rawBuffer, &adc_hw->fifo, 128, false);

    while (1)
    {
        adc_fifo_drain();
        adc_run(true);
        timestamp = time_us_64();
        dma_channel_configure(dmaChan, &dmaCfg, &rawBuffer, &adc_hw->fifo, 128, true);
        dma_channel_wait_for_finish_blocking(dmaChan);
        dma_channel_abort(dmaChan);
        timestamp = time_us_64() - timestamp;
        freq = 128.0 * 1000 / timestamp;

        adc_run(false);
        for (int i = 0; i < 128; i++)
        {
            input[i] = rawBuffer[i] * factor;
        }
        ssd1306_clear(&disp);
        auto it = input.begin();
        sprintf(msg, "%.4fV %.2fKhz", *it, freq);
        ssd1306_draw_string(&disp, 0, 0, 1, msg);
        for (int i = 0; i < 128; i++)
        {
            ssd1306_draw_pixel(&disp, i, (*it) * 32 / 1.25);
            it++;
        }

        fft.fft(input.data(), re.data(), im.data());
        for (int i = 1; i < 128; i++)
        {
            ssd1306_draw_line(&disp, i, 63 - abs(re[(i + 1) / 2]) * 4, i, 63);
        }
        ssd1306_show(&disp);
    }
    return 0;
}

void hardwareInit()
{
    set_sys_clock_khz(48 * 1000, true);
    stdio_init_all();
    adc_init();
    adc_set_clkdiv(2000);
    adc_select_input(0);
    adc_fifo_setup(true, true, 1, false, false);
    adc_gpio_init(26);
    adc_run(false);
    i2c_init(i2c0, 2400000);
    gpio_set_function(16, GPIO_FUNC_I2C);
    gpio_set_function(17, GPIO_FUNC_I2C);
    gpio_pull_up(16);
    gpio_pull_up(17);
    disp.external_vcc = false;
    ssd1306_init(&disp, 128, 64, 0x3C, i2c0);
    ssd1306_clear(&disp);
}