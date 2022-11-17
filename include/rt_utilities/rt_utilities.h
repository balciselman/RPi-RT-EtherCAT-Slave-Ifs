



#ifndef __RT_UTILITIES_H
 #define __RT_UTILITIES_H









/*! \struct _STATBUFFER
*  \brief This struct holds the buffer to calculate statistic values */
typedef struct _STATBUFFER
{
	double v;					  /*!<  current value */
	unsigned long long count;     /*!<  count of calculation */

	double vmean;            /*!<  mean value */
	double vmax;             /*!<  maximum value */
	double vmin;             /*!<  minimum value */
	double vstddev;          /*!<  standard deviation */
	double vrange;           /*!<  range */

	// local
	double ic_sum;          /*!<  local sum of values */
	double ic_sum2;         /*!<  local sum of square values */
} STATBUFFER;


/*! \class stattool
 *  \brief This class implements a statistic Tool to calculate mean,min,max and stddeviation */
class stattool
{
	public:
		STATBUFFER sbuf;
		stattool();
		virtual ~stattool();
		void calc(double value);
		void reset();

};



#endif