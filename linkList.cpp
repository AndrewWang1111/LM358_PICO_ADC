#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/adc.h>
#include <hardware/vreg.h>
#include "node.h"
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

int main()
{
    hardwareInit();

    const size_t fftSize = 128; // Needs to be power of 2!

    std::vector<float> input(fftSize, 0.0f);
    std::vector<float> re(audiofft::AudioFFT::ComplexSize(fftSize));
    std::vector<float> im(audiofft::AudioFFT::ComplexSize(fftSize));
    std::vector<float> output(fftSize);

    audiofft::AudioFFT fft;
    fft.init(fftSize);
    std::vector<uint16_t> tenTimes;
    tenTimes.clear();
    char msg[25];

    uint64_t ticksStart = 0;
    uint64_t ticksEnd = 0;
    uint32_t frameCounts = 0;
    uint32_t fps = 0;
    float pointY = 0.0;

    while (1)
    {

        for (auto it = input.begin(); it != input.end(); it++)
        {

            *it = adc_read() * factor;
            sleep_us(50);
        }

        ssd1306_clear(&disp);
        auto it = input.begin();
        sprintf(msg, "%.4fV", *it);
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

        ticksEnd = time_us_64();
        if (ticksEnd - ticksStart >= 1000000)
        {
            fps = frameCounts;
            frameCounts = 0;
            ticksStart = time_us_64();
        }
        frameCounts++;
        sprintf(msg, "FPS:%d", fps);
        ssd1306_draw_string(&disp, 64, 0, 1, msg);
        ssd1306_show(&disp);
    }
    return 0;
}

void hardwareInit()
{
    set_sys_clock_khz(266 * 1000, true);
    stdio_init_all();
    adc_init();
    adc_select_input(0);
    adc_gpio_init(26);
    i2c_init(i2c0, 2400000);
    gpio_set_function(20, GPIO_FUNC_I2C);
    gpio_set_function(21, GPIO_FUNC_I2C);
    gpio_pull_up(20);
    gpio_pull_up(21);
    disp.external_vcc = false;
    ssd1306_init(&disp, 128, 64, 0x3C, i2c0);
    ssd1306_clear(&disp);
}