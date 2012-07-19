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
                assert(0 && "���ܵĲ�������ȷ");
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
                assert(0 && "���ܵĲ�������ȷ");
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
				TEA�㷨�ɽ��Ŵ�ѧ�����ʵ���ҵ�David Wheeler��Roger Needham��1994�귢��[3]��
				����һ�ַ��������㷨�����������Ŀ�Ϊ64���أ���Կ����Ϊ128���ء�
				TEA�㷨���ò������ӵ�Delta(�ƽ�ָ���)ֵ��Ϊ�仯��ʹ��ÿ�ֵļ����ǲ�ͬ���ü����㷨�ĵ����������Ըı䣬
				����ĵ�������Ϊ32�֡�

				��ȻTEA�㷨�� DES(Data Encryption Standard) Ҫ�򵥵ö࣬ ���к�ǿ�Ŀ���ַ��������������ٶ�Ҳ�� DES ��ö࣬
				���Ҷ� 64 λ���ݼ��ܵ���Կ���� 128 λ����ȫ���൱�á�
				��ɿ�����ͨ�����������������㷨�ĸ��Ӷ�����֤�ġ�
				���п��Կ���TEA �㷨��Ҫ��������λ��������㡣
				��Կ�ڼ��ܹ�����ʼ�ղ���
			*/

			// ����Ϊ8�ֽڵ����������16�ֽڵ���Կ�����8�ֽ�����
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
