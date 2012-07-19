#ifndef __IOCP_MEMORY_MANAGER_HPP
#define __IOCP_MEMORY_MANAGER_HPP

#include "../../MemoryPool/SGIMemoryPool.hpp"
#include "../../MemoryPool/FixedMemoryPool.hpp"
#include "../../Allocator/ContainerAllocator.hpp"

//#include "../../MemoryPool/MemoryLeak.h"

namespace async
{


	namespace iocp
	{

		// -------------------------------------------------------
		// class MemoryMgr 
		// �ڴ����

		class MemoryMgr
		{
		public:
			// С����ʹ��--Socket,File, Object...
			typedef async::memory::SGIMTMemoryPool SmallObjectMemoryPool;
			typedef async::allocator::ContainerAllocator<char, SmallObjectMemoryPool>	AllocType;


			//  Socket��File�ڴ��
			static const size_t DEFAULT_SIZE = 8 * 1024;		
			static const size_t DEFAULT_FILE_SIZE	= 8 * 1024;

			typedef async::memory::SGIMemoryPool<true, DEFAULT_SIZE>		MemoryPool;
			//typedef async::memory::SGIMemoryPool<true, DEFAULT_FILE_SIZE>		FileMemoryPool;

		public:
			// С����ʹ��
			static AllocType &GetMemory()
			{
				static SmallObjectMemoryPool memoryPool;
				static AllocType alloc(memoryPool);

				return alloc;
			}
		};

	} // end of iocp

} // end of async












#endif