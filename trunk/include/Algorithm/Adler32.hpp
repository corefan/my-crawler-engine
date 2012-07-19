#ifndef __ALGORITHM_HPP
#define __ALGORITHM_HPP

#include "../Unicode/string.hpp"


namespace algorithm
{
	namespace adler
	{
		static const unsigned long BASE = 65521L; /* largest prime smaller than 65536 */
		static const unsigned long NMAX = 5552;
		/* NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1 */

		#define DO1(buf,i)  {s1 += buf[i]; s2 += s1;}
		#define DO2(buf,i)  DO1(buf,i); DO1(buf,i+1);
		#define DO4(buf,i)  DO2(buf,i); DO2(buf,i+2);
		#define DO8(buf,i)  DO4(buf,i); DO4(buf,i+4);
		#define DO16(buf)   DO8(buf,0); DO8(buf,8);

		namespace detail
		{
			inline unsigned long adler32(unsigned long adler, const unsigned char *buf, unsigned int len)
			{
				unsigned long s1 = adler & 0xffff;
				unsigned long s2 = (adler >> 16) & 0xffff;
				int k = 0;

				if (buf == 0) 
					return 1L;

				while (len > 0) 
				{
					k = len < NMAX ? len : NMAX;
					len -= k;

					while (k >= 16) 
					{
						DO16(buf);
						buf += 16;
						k -= 16;
					}

					if (k != 0) 
					{
						do 
						{
							s1 += *buf++;
							s2 += s1;
						} while (--k);
					}

					s1 %= BASE;
					s2 %= BASE;
				}

				return (s2 << 16) | s1;
			}
		}

		

		template < typename CharT >
		inline unsigned long adler32(const std::basic_string<CharT> &buf, unsigned long alder = 0)
		{
			const std::string &tmp = unicode::to_a(buf);
			return detail::adler32(alder, tmp.c_str(), tmp.length());
		}

		inline unsigned long adler32(const unsigned char *buf, unsigned long len, unsigned long alder = 1)
		{
			return detail::adler32(alder, buf, len);
		}
	}
}


#endif