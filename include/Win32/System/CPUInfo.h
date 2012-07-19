#pragma once

#include "../../Unicode/string.hpp"

//=====================================================================================
/*                 CPUID指令是intel IA32架构下获得CPU信息的汇编指令，
可以得到CPU类型，型号，制造商信息，商标信息，序列号，
缓存等一系列CPU相关的东西。
*/
#include <windows.h>
#include <iostream>
#include <string>

namespace win32
{
    namespace system
    {
        //用来存储eax,ebx,ecx,edx四个寄存器的信息
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

        void _ExeCPUID(_Register& reg, DWORD veax)  //初始化CPU
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

/*	在Intel Pentium以上级别的CPU中，有一个称为“时间戳（Time Stamp）”的部件，
它以64位无符号整型数的格式，记录了自CPU上电以来所经过的时钟周期数。
由于目前的CPU主频都非常高，因此这个部件可以达到纳秒级的计时精度。
这个精确性是上述两种方法所无法比拟的。
在Pentium以上的CPU中，提供了一条机器指令RDTSC（Read Time Stamp Counter）
来读取这个时间戳的数字，并将其保存在EDX:EAX寄存器对中
*/
        long GetCPUFreq()       //获取CPU频率,单位: MHZ
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



/*   把eax = 0作为输入参数，可以得到CPU的制造商信息。
cpuid指令执行以后，会返回一个12字符的制造商信息，
前四个字符的ASC码按低位到高位放在ebx，中间四个放在edx，最后四个字符放在ecx。
*/

        template<class CharT>
        inline std::basic_string<CharT> GetCPUManufacturer()   //获取制造商信息
        {
            char manufacturer[25] = {0};

            _Register reg;
            _ExeCPUID(reg, 0);          //初始化
            memcpy(manufacturer + 0, &reg.debx, 4); //制造商信息复制到数组
            memcpy(manufacturer + 4, &reg.dedx, 4);
            memcpy(manufacturer + 8, &reg.decx, 4);

            return unicode::translate_t<CharT>::utf(manufacturer);
        }


/*  在我的电脑上点击右键，选择属性，可以在窗口的下面看到一条CPU的信息，
这就是CPU的商标字符串。CPU的商标字符串也是通过cpuid得到的。
由于商标的字符串很长(48个字符)，所以不能在一次cpuid指令执行时全部得到，
所以intel把它分成了3个操作，eax的输入参数分别是0x80000002,0x80000003,0x80000004，
每次返回的16个字符，按照从低位到高位的顺序依次放在eax, ebx, ecx, edx。
因此，可以用循环的方式，每次执行完以后保存结果，然后执行下一次cpuid。
*/
        template<class CharT>
        inline std::basic_string<CharT> GetCPUName()
        {
            const DWORD id = 0x80000002; //从0x80000002开始,到0x80000004结束
            char cpuType[49] = {0};//用来存储CPU型号信息

            for (DWORD t = 0 ; t < 3 ; ++t)
            {
                _Register reg;
                _ExeCPUID(reg, id + t);
                //每次循环结束,保存信息到数组
                memcpy(cpuType + 16 * t +  0, &reg.deax, 4);
                memcpy(cpuType + 16 * t +  4, &reg.debx, 4);
                memcpy(cpuType + 16 * t +  8, &reg.decx, 4);
                memcpy(cpuType + 16 * t + 12, &reg.dedx, 4);
            }            

            return unicode::translate_t<CharT>::utf(cpuType);
        }
    }
}


