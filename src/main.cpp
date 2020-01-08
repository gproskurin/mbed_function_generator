#include "drivers/DigitalOut.h"
#include "rtos/ThisThread.h"
#include "mbed_wait_api.h"

#include "ad5932.h"

mbed::DigitalOut Led(PB_12, 1 /*led off*/);

void led_off() { Led = 1; }
void led_on() { Led = 0; }


void led_blink_times(int count)
{
	for (int i=0; i<count; ++i) {
		led_on();
		rtos::ThisThread::sleep_for(500);
		led_off();
		rtos::ThisThread::sleep_for(500);
	}
	rtos::ThisThread::sleep_for(2000);
}


int main()
{
	led_off();

	led_blink_times(1);
	ad5932_t Ad;
	led_blink_times(2);

	Ad.spi_write(
		ad5932_t::REG_Creg
		| ad5932_t::REG_Creg_B24
		| ad5932_t::REG_Creg_DAC_ENABLE
		| ad5932_t::REG_Creg_SINE_TRI
		| ad5932_t::REG_Creg_MSBOUTEN
		| ad5932_t::REG_Creg_INT_EXT_INCR
		| ad5932_t::REG_Creg_SYNCSEL
		| ad5932_t::REG_Creg_SYNCOUTEN
	);
	led_blink_times(3);

	int freq_lo, freq_hi;
	Ad.calc_delta_freq(500, &freq_hi, &freq_lo);
	Ad.spi_write(ad5932_t::REG_f_start_lo | freq_lo);
	Ad.spi_write(ad5932_t::REG_f_start_hi | freq_hi);

	Ad.spi_write(ad5932_t::REG_Nincr | 4095);

	Ad.spi_write(ad5932_t::REG_delta_f_lo | 10);
	Ad.spi_write(ad5932_t::REG_delta_f_hi | 0);

	Ad.spi_write(ad5932_t::REG_t_int_mclk_periods | ad5932_t::REG_t_int_multiplier_1 | 2047);

	Ad.ctrl_pulse();

	while (1) {
		rtos::ThisThread::sleep_for(900);
		led_on();
		rtos::ThisThread::sleep_for(100);
		led_off();
	}
}
