




#include "ethercatslave.h"




//***********************/
// small statistic tool */
//***********************/

stattool::stattool()
{
  reset();
}

stattool::~stattool(){}


void stattool::reset()
{
	memset(&sbuf, 0, sizeof(sbuf));
}

void stattool::calc(double value)
{
	double _sqrtval;

	sbuf.v = value;
	if (sbuf.count == 0)
	{
		sbuf.vmin = value;
		sbuf.vmax = value;
	}
	else
	{
		if (value > sbuf.vmax) sbuf.vmax = value;
		if (value < sbuf.vmin) sbuf.vmin = value;
	}
	sbuf.vrange = sbuf.vmax - sbuf.vmin;

	sbuf.ic_sum += value;
	sbuf.ic_sum2 += (value * value);

	sbuf.count++;
	sbuf.vmean = (sbuf.ic_sum / (double)sbuf.count);
	sbuf.vstddev = 0.0;
	if (sbuf.count > 1)
	{
		_sqrtval = (sbuf.ic_sum2 - ((sbuf.ic_sum * sbuf.ic_sum) / (double)sbuf.count)) / (double)(sbuf.count - 1);
		if (_sqrtval > 0) sbuf.vstddev = sqrt(_sqrtval);
	}
}
