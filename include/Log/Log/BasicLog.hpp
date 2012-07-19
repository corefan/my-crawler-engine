#ifndef __LOG_BASIC_LOG_HPP
#define __LOG_BASIC_LOG_HPP

#include "../Filter/Priority.hpp"


namespace logsystem
{
	namespace impl
	{

		// --------------------------------------------------------
		// class BasicLog
		// 提供格式化控制

		class BasicLog
		{
		protected:
			size_t printLevel_;				// Priority Level
			bool showLevel_;					// 是否一行结束

		public:
			BasicLog()
				: printLevel_(filter::Info)
				, showLevel_(true)
			{}

		public:
			void Priority(size_t priority, bool show)
			{
				printLevel_ = priority;
				showLevel_	= show;
			}

			size_t Priority() const
			{
				return printLevel_;
			}

			bool Show()
			{
				bool tmp = showLevel_;
				showLevel_ = false;

				return tmp;
			}
		};
	}
}








#endif