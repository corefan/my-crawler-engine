#pragma once

#include "../../Unicode/string.hpp"

//=====================================================================================
/*                 CPUIDָ����intel IA32�ܹ��»��CPU��Ϣ�Ļ��ָ�
���Եõ�CPU���ͣ��ͺţ���������Ϣ���̱���Ϣ�����кţ�
�����һϵ��CPU��صĶ�����
*/
#include <windows.h>
#include <iostream>
#include <string>

namespace win32
{
    namespace system
    {
        //�����洢eax,ebx,ecx,edx�ĸ��Ĵ�������Ϣ
        struct _Register
        {
            DWORD deax;
            DWORD debx;
            DWORD decx;
            DWORD dedx;
            _Register()
                : deax(0)
                , debx(0)
                , decx(0)
                , dedx(0)
            {
            }
        };

        void _ExeCPUID(_Register& reg, DWORD veax)  //��ʼ��CPU
        {
            DWORD deax;
            DWORD debx;
            DWORD decx;
            DWORD dedx;
            __asm
            {
                mov eax,veax
                    cpuid
                    mov deax, eax
                    mov debx, ebx
                    mov decx, ecx
                    mov dedx, edx
            }
            reg.deax = deax;
            reg.debx = debx;
            reg.decx = decx;
            reg.dedx = dedx;
        }

/*	��Intel Pentium���ϼ����CPU�У���һ����Ϊ��ʱ�����Time Stamp�����Ĳ�����
����64λ�޷����������ĸ�ʽ����¼����CPU�ϵ�������������ʱ����������
����Ŀǰ��CPU��Ƶ���ǳ��ߣ��������������Դﵽ���뼶�ļ�ʱ���ȡ�
�����ȷ�����������ַ������޷�����ġ�
��Pentium���ϵ�CPU�У��ṩ��һ������ָ��RDTSC��Read Time Stamp Counter��
����ȡ���ʱ��������֣������䱣����EDX:EAX�Ĵ�������
*/
        long GetCPUFreq()       //��ȡCPUƵ��,��λ: MHZ
        {
            int start, over;
            _asm 
            {
                RDTSC
                    mov start, eax
            }
            Sleep(50);
            _asm 
            {
                RDTSC
                    mov over, eax
            }
            return (over - start) / 50000;
        }



/*   ��eax = 0��Ϊ������������Եõ�CPU����������Ϣ��
cpuidָ��ִ���Ժ󣬻᷵��һ��12�ַ�����������Ϣ��
ǰ�ĸ��ַ���ASC�밴��λ����λ����ebx���м��ĸ�����edx������ĸ��ַ�����ecx��
*/

        template<class CharT>
        inline std::basic_string<CharT> GetCPUManufacturer()   //��ȡ��������Ϣ
        {
            char manufacturer[25] = {0};

            _Register reg;
            _ExeCPUID(reg, 0);          //��ʼ��
            memcpy(manufacturer + 0, &reg.debx, 4); //��������Ϣ���Ƶ�����
            memcpy(manufacturer + 4, &reg.dedx, 4);
            memcpy(manufacturer + 8, &reg.decx, 4);

            return unicode::translate_t<CharT>::utf(manufacturer);
        }


/*  ���ҵĵ����ϵ���Ҽ���ѡ�����ԣ������ڴ��ڵ����濴��һ��CPU����Ϣ��
�����CPU���̱��ַ�����CPU���̱��ַ���Ҳ��ͨ��cpuid�õ��ġ�
�����̱���ַ����ܳ�(48���ַ�)�����Բ�����һ��cpuidָ��ִ��ʱȫ���õ���
����intel�����ֳ���3��������eax����������ֱ���0x80000002,0x80000003,0x80000004��
ÿ�η��ص�16���ַ������մӵ�λ����λ��˳�����η���eax, ebx, ecx, edx��
��ˣ�������ѭ���ķ�ʽ��ÿ��ִ�����Ժ󱣴�����Ȼ��ִ����һ��cpuid��
*/
        template<class CharT>
        inline std::basic_string<CharT> GetCPUName()
        {
            const DWORD id = 0x80000002; //��0x80000002��ʼ,��0x80000004����
            char cpuType[49] = {0};//�����洢CPU�ͺ���Ϣ

            for (DWORD t = 0 ; t < 3 ; ++t)
            {
                _Register reg;
                _ExeCPUID(reg, id + t);
                //ÿ��ѭ������,������Ϣ������
                memcpy(cpuType + 16 * t +  0, &reg.deax, 4);
                memcpy(cpuType + 16 * t +  4, &reg.debx, 4);
                memcpy(cpuType + 16 * t +  8, &reg.decx, 4);
                memcpy(cpuType + 16 * t + 12, &reg.dedx, 4);
            }            

            return unicode::translate_t<CharT>::utf(cpuType);
        }
    }
}


