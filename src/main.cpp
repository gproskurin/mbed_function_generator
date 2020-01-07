#include "drivers/DigitalOut.h"
#include "drivers/PwmOut.h"
#include "drivers/SPI.h"
#include "rtos/ThisThread.h"

class ad5932_t {
	mbed::SPI spi_;
	mbed::PwmOut mclk_;
	mbed::DigitalOut fsync_;

public:
	enum {
		REG_Creg = 0b0000 << 12,
		REG_Nincr = 0b0001 << 12,
		REG_delta_f_lo = 0b0010 << 12,
		REG_delta_f_hi = 0b0011 << 12,
		REG_t_int_num_cycles = 0b0100 << 12,
		REG_t_int_mclk_periods = 0b0110 << 12,
		REG_f_start_lo = 0b1100 << 12,
		REG_f_start_hi = 0b1101 << 12
	};

public:
	ad5932_t()
		: spi_(PA_7/*mosi*/, NC /*PA_6*/ /*miso*/, PA_5/*clk*/)
		, mclk_(PB_1)
		, fsync_(PB_15, 1)
	{
		spi_.format(16, 1);
		spi_.frequency(1000000);

		mclk_.period_us(1000); // 1kHz
		mclk_.write(0.5f); // 50% duty
	};

	int spi_write(int data)
	{
		fsync_ = 0;
		//ThisThread::sleep_for_us(1);
		const int ret = spi_.write(data);
		fsync_ = 1;
		return ret;
	}
};

ad5932_t Ad;
mbed::DigitalOut Led(PB_12, 1/*off*/);

void led_off()
{
	Led = 1;
}

void led_on()
{
	Led = 0;
}

int main()
{
	led_off();

	Ad.spi_write(ad5932_t::REG_Creg | 0b011111011111);
	Ad.spi_write(ad5932_t::REG_f_start_lo | 0);
	Ad.spi_write(ad5932_t::REG_f_start_hi | 0);
	Ad.spi_write(ad5932_t::REG_Nincr | 4095);
	Ad.spi_write(ad5932_t::REG_delta_f_lo | 10);
	Ad.spi_write(ad5932_t::REG_delta_f_hi | 0);
	Ad.spi_write(ad5932_t::REG_t_int_mclk_periods | 2047);

	while (1) {
		rtos::ThisThread::sleep_for(900);
		led_on();
		rtos::ThisThread::sleep_for(100);
		led_off();
	}
}
