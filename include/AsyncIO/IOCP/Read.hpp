#ifndef __IOCP_READ_HELPER_HPP
#define __IOCP_READ_HELPER_HPP


#include "internal/Condition.hpp"


namespace async
{
	namespace iocp
	{

		namespace detail
		{
			typedef std::tr1::function<void()>	ReadCallback;
		}
		

		//
		template<typename SyncWriteStreamT, typename MutableBufferT>
		size_t Read(SyncWriteStreamT &s, MutableBufferT &buffer)
		{
			return Read(s, buffer, TransferAll(), 0);
		}

		template<typename SyncWriteStreamT, typename MutableBufferT>
		size_t Read(SyncWriteStreamT &s, MutableBufferT &buffer, const detail::ReadCallback &callback)
		{
			return Read(s, buffer, TransferAll(), callback);
		}

		template<typename SyncWriteStreamT, typename MutableBufferT, typename OffsetT>
		size_t Read(SyncWriteStreamT &s, MutableBufferT &buffer, const OffsetT &offset)
		{
			return Read(s, buffer, offset, TransferAll());
		}

		// 
		template<typename SyncWriteStreamT, typename MutableBufferT, typename CompleteConditionT>
		size_t Read(SyncWriteStreamT &s, MutableBufferT &buffer, CompleteConditionT &condition)
		{
			return Read(s, buffer, condition, 0);
		}

		template<typename SyncWriteStreamT, typename MutableBufferT, typename CompleteConditionT>
		size_t Read(SyncWriteStreamT &s, MutableBufferT &buffer, CompleteConditionT &condition, const detail::ReadCallback &callback)
		{
			size_t transfers = 0;
			const size_t bufSize = buffer.size();

			while( transfers <= condition(transfers) )
			{
				if( transfers >= bufSize )
					break;

				size_t ret = s.Read(buffer + transfers);
				if( ret == 0 )
				{
					s.Close();
				}

				transfers += ret;

				if( callback != 0 )
					callback();
			}

			return transfers;
		}

		template<typename SyncWriteStreamT, typename MutableBufferT, typename OffsetT, typename CompleteConditionT>
		size_t Read(SyncWriteStreamT &s, MutableBufferT &buffer, const OffsetT &offset, CompleteConditionT &condition)
		{
			size_t transfers = 0;
			const size_t bufSize = buffer.size();

			while( transfers <= condition(transfers) )
			{
				if( transfers >= bufSize )
					break;

				size_t ret = s.Read(buffer + transfers, offset);

				transfers += ret;
			}

			return transfers;
		}



		namespace detail
		{
			template<typename AsyncWriteStreamT, typename MutableBufferT, typename CompletionConditionT, typename ReadHandlerT>
			class ReadHandler
			{
				typedef ReadHandler<AsyncWriteStreamT, MutableBufferT, CompletionConditionT, ReadHandlerT>	ThisType;
				
			private:
				AsyncWriteStreamT &stream_;
				MutableBufferT buffer_;
				CompletionConditionT condition_;
				size_t transfers_;
				const size_t total_;
				ReadHandlerT handler_;

			public:
				ReadHandler(AsyncWriteStreamT &stream, MutableBufferT &buffer, size_t total, const CompletionConditionT &condition, size_t transfer, const ReadHandlerT &handler)
					: stream_(stream)
					, buffer_(buffer)
					, condition_(condition)
					, transfers_(transfer)
					, total_(total)
					, handler_(handler)
				{}

				void operator()(u_long size, u_long error)
				{
					transfers_ += size; 

					if( transfers_ < total_ && size != 0 && error == 0 )
					{
						if( transfers_ < condition_() )
						{
							stream_.AsyncRead(buffer_ + size, 
								ThisType(stream_, buffer_ + size, total_, condition_, transfers_, handler_));

							return;
						}
					}
					
					// 回调	
					handler_(error, transfers_);
				}
			};

			template<typename AsyncWriteStreamT, typename MutableBufferT, typename OffsetT, typename CompletionConditionT, typename ReadHandlerT>
			class ReadOffsetHandler
			{
				typedef ReadOffsetHandler<AsyncWriteStreamT, MutableBufferT, OffsetT, CompletionConditionT, ReadHandlerT>	ThisType;

			private:
				AsyncWriteStreamT &stream_;
				MutableBufferT buffer_;
				CompletionConditionT condition_;
				const OffsetT offset_;
				size_t transfers_;
				const size_t total_;
				ReadHandlerT handler_;

			public:
				ReadOffsetHandler(AsyncWriteStreamT &stream, MutableBufferT &buffer, size_t total, const OffsetT &offset, const CompletionConditionT &condition, size_t transfer, const ReadHandlerT &handler)
					: stream_(stream)
					, buffer_(buffer)
					, condition_(condition)
					, offset_(offset)
					, transfers_(transfer)
					, total_(total)
					, handler_(handler)
				{}

				void operator()(u_long size, u_long error)
				{
					transfers_ += size; 

					if( transfers_ < total_ && size != 0 && error == 0 )
					{
						if( transfers_ < condition_() )
						{
							stream_.AsyncRead(buffer_ + size, offset_,
								ThisType(stream_, buffer_ + size, total_, offset_, condition_, transfers_, handler_));

							return;
						}
					}

					// 回调
					handler_(error, transfers_);
				}
			};
		}

		// 异步读取指定的数据

		//
		template<typename SyncWriteStreamT, typename MutableBufferT, typename HandlerT>
		void AsyncRead(SyncWriteStreamT &s, MutableBufferT &buffer, const HandlerT &handler)
		{
			AsyncRead(s, buffer, TransferAll(), handler);
		}

		template<typename SyncWriteStreamT, typename MutableBufferT, typename HandlerT>
		void AsyncRead(SyncWriteStreamT &s, MutableBufferT &buffer, const LARGE_INTEGER &offset, const HandlerT &handler)
		{
			AsyncRead(s, buffer, offset, TransferAll(), handler);
		}

		// 
		template<typename SyncWriteStreamT, typename MutableBufferT, typename ComplateConditionT, typename HandlerT>
		void AsyncRead(SyncWriteStreamT &s, MutableBufferT &buffer, const ComplateConditionT &condition, const HandlerT &handler)
		{
			typedef detail::ReadHandler<SyncWriteStreamT, MutableBufferT, ComplateConditionT, HandlerT> HookReadHandler;

			s.AsyncRead(buffer, HookReadHandler(s, buffer, buffer.size(), condition, 0, handler));
		}
	
		template<typename SyncWriteStreamT, typename MutableBufferT, typename OffsetT, typename ComplateConditionT, typename HandlerT>
		void AsyncRead(SyncWriteStreamT &s, MutableBufferT &buffer, const OffsetT &offset, const ComplateConditionT &condition, const HandlerT &handler)
		{
			typedef detail::ReadOffsetHandler<SyncWriteStreamT, MutableBufferT, OffsetT, ComplateConditionT, HandlerT> HookReadHandler;

			s.AsyncRead(buffer, offset, HookReadHandler(s, buffer, buffer.size(), offset, condition, 0, handler));
		}

	}
}







#endif