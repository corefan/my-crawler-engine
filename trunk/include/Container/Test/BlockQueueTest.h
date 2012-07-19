#pragma once
#include "../../MultiThread/Thread.hpp"
#include "../BlockingQueue.hpp"

#include <iostream>


struct Consumer
{
	static DWORD Run(async::container::BlockingQueue<int> &queue, async::thread::ThreadImplEx &thread)
	{
		while(!thread.IsAborted())
		{
			int n = queue.Get();
			std::cout << n << std::endl;

			::Sleep(100);
		}

		return 0;
	}
};

struct Productor
{
	static DWORD Run(async::container::BlockingQueue<int> &queue, async::thread::ThreadImplEx &thread)
	{
		int n = 0;

		while(!thread.IsAborted())
		{
			++n;
			queue.Put(n);
			::Sleep(500);
		}

		return 0;
	}
};

void Test()
{
	

	async::container::BlockingQueue<int> queue;

	async::thread::ThreadImplEx consumer1, consumer2, productor;
	consumer1.RegisterFunc(std::tr1::bind(&Consumer::Run, std::tr1::ref(queue), std::tr1::ref(consumer1)));
	consumer2.RegisterFunc(std::tr1::bind(&Consumer::Run, std::tr1::ref(queue), std::tr1::ref(consumer2)));
	productor.RegisterFunc(std::tr1::bind(&Productor::Run, std::tr1::ref(queue), std::tr1::ref(productor)));

	consumer1.Start();
	consumer2.Start();
	productor.Start();

	system("pause");

	consumer1.Stop();
	consumer2.Stop();
	productor.Stop();
}