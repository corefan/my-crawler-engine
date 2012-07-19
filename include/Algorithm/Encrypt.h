#pragma once

#include <wtypes.h>

namespace algorithm
{
    namespace crypt
    {
        inline void Encode(void* desData, unsigned int desSize, const void* srcData, unsigned int srcSize, const byte* key, unsigned int keySize)
        {
            if (desData == NULL || srcData == NULL || desSize < srcSize || key == NULL || keySize <= 0)
            {
                assert(0 && "加密的参数不正确");
                return;
            }

            const byte* encodeData = static_cast<const byte*>(srcData);
            byte* newData = static_cast<byte*>(desData);
            for (unsigned int i = 0; i < srcSize; ++i)
            {
                newData[i] = (unsigned int(encodeData[i]) + key[i % keySize]) % 256;
            }
        }

        inline void Decode(void* desData, unsigned int desSize, const void* srcData, unsigned int srcSize, const byte* key, unsigned int keySize)
        {
            if (desData == NULL || srcData == NULL || desSize < srcSize || key == NULL || keySize <= 0)
            {
                assert(0 && "解密的参数不正确");
                return;
            }

            const byte* decodeData = static_cast<const byte*>(srcData);
            byte* newData = static_cast<byte*>(desData);
            for (unsigned int i = 0; i < srcSize; ++i)
            {
                newData[i] = (256 + unsigned int(decodeData[i]) - key[i % keySize]) % 256;
            }
        }

        inline void Encode(void* desData, unsigned int desSize, const void* srcData, unsigned int srcSize, LPCSTR key)
        {
            Encode(desData, desSize, srcData, srcSize, reinterpret_cast<const byte*>(key), strlen(key));
        }

        inline void Decode(void* desData, unsigned int desSize, const void* srcData, unsigned int srcSize, LPCSTR key)
        {
            Decode(desData, desSize, srcData, srcSize, reinterpret_cast<const byte*>(key), strlen(key));
        }


		namespace detail
		{
			/*
				TEA算法由剑桥大学计算机实验室的David Wheeler和Roger Needham于1994年发明[3]。
				它是一种分组密码算法，其明文密文块为64比特，密钥长度为128比特。
				TEA算法利用不断增加的Delta(黄金分割率)值作为变化，使得每轮的加密是不同，该加密算法的迭代次数可以改变，
				建议的迭代次数为32轮。

				虽然TEA算法比 DES(Data Encryption Standard) 要简单得多， 但有很强的抗差分分析能力，加密速度也比 DES 快得多，
				而且对 64 位数据加密的密钥长达 128 位，安全性相当好。
				其可靠性是通过加密轮数而不是算法的复杂度来保证的。
				从中可以看到TEA 算法主要运用了移位和异或运算。
				密钥在加密过程中始终不变
			*/

			// 参数为8字节的明文输入和16字节的密钥，输出8字节密文
			inline void tea_encipher(const unsigned long * const v, unsigned long * const w, const unsigned long *const k)
			{
				unsigned long y = v[0];
				unsigned long z = v[1];
				unsigned long sum = 0;
				unsigned long delta = 0x9E3779B9;
				unsigned long n = 32;

				while(n-- > 0)
				{
					y += (z << 4 ^ z >> 5) + z ^ sum + k[sum & 3];
					sum += delta;
					z += (y << 4 ^ y >> 5) + y ^ sum + k[sum >> 11 & 3];
				}

				w[0] = y;
				w[1] = z;
			}

			inline void tea_decipher(const unsigned long * const v, unsigned long * const w, const unsigned long *const k)
			{
				unsigned long y = v[0];
				unsigned long z = v[1];
				unsigned long sum = 0xC6EF3720;
				unsigned long delta = 0x9E3779B9;
				unsigned long n = 32;

				while(n-- > 0)
				{
					z -= (y << 4 ^ y >> 5) + y ^ sum + k[sum >> 11 & 3];
					sum -= delta;
					y -= (z << 4 ^ z >> 5) + z ^ sum + k[sum & 3];
				}

				w[0] = y;
				w[1] = z;
			}

		}
		

		template < typename T, typename U >
		inline void tea_encipher(const T * const in, size_t in_size, T * const out, size_t out_size, const U *const key, size_t key_size)
		{
			assert(in_size == out_size);
			assert(in_size >= 2 * sizeof(unsigned long));
			assert(key_size > sizeof(unsigned long));

			detail::tea_encipher(reinterpret_cast<const unsigned long* const>(in), 
				reinterpret_cast<unsigned long* const>(out), 
				reinterpret_cast<const unsigned long* const>(key));
		}

		template < typename T, typename U >
		inline void tea_encipher(const T * const in, T * const out, size_t size, const U *const key, size_t key_size)
		{
			tea_encipher(in, size, out, size, key, key_size);
		}

		template < typename T, typename U >
		inline void tea_decipher(const T * const in, size_t in_size, T * const out, size_t out_size, const U *const key, size_t key_size)
		{
			assert(in_size == out_size);
			assert(in_size >= 2 * sizeof(unsigned long));
			assert(key_size > sizeof(unsigned long));

			detail::tea_decipher(reinterpret_cast<const unsigned long* const>(in), 
				reinterpret_cast<unsigned long* const>(out), 
				reinterpret_cast<const unsigned long* const>(key));
		}

		template < typename T, typename U >
		inline void tea_decipher(const T * const in, T * const out, size_t size, const U *const key, size_t key_size)
		{
			tea_decipher(in, size, out, size, key, key_size);
		}
    }
}
