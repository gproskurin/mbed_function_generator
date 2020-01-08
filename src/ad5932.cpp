#include "mbed_wait_api.h"

#include "ad5932.h"

ad5932_t::ad5932_t()
	: mclk_period_us_(5) // 200 kHz
	, ctrl_pulsewidth_us_((mclk_period_us_ * 2) + (mclk_period_us_ / 2) + 1) // greater then times 2.5
	, fsync_(PB_15, 1)
	, ctrl_(PA_8, 0)
	, mclk_(PB_1)
	, spi_(PA_7/*mosi*/, PA_6 /*miso*/, PA_5/*clk*/)
{
	spi_.format(16, 2);
	spi_.frequency(1000000);

	mclk_.period_us(mclk_period_us_);
	mclk_.write(0.5f); // 50% duty
}

int ad5932_t::spi_write(int data)
{
	fsync_ = 0;
	wait_us(1);
	const int ret = spi_.write(data);
	fsync_ = 1;
	wait_us(1);
	return ret;
}

void ad5932_t::ctrl_pulse()
{
	ctrl_ = 1;
	wait_us(ctrl_pulsewidth_us_);
	ctrl_ = 0;
}

void ad5932_t::calc_delta_freq(const uint64_t desired_freq, int* const hi, int* const lo) const
{
	const uint64_t pow24 = (1 << 24); // 2^24
	const uint64_t pow12 = (1 << 12); // 2^12
	const uint64_t res = pow24 * desired_freq * mclk_period_us_ / 1000000;
	const uint64_t res_lo = res & (pow12 - 1);
	const uint64_t res_hi = res >> 12;
	*hi = res_hi;
	*lo = res_lo;
}

