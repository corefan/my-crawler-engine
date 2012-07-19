#ifndef __IOCP_WRITE_HELPER_HPP
#define __IOCP_WRITE_HELPER_HPP


#include "internal/Condition.hpp"


namespace async
{
	namespace iocp
	{

		typedef std::tr1::function<void()> Callback;

		// 
		template<typename SyncWriteStreamT, typename ConstBufferT>
		size_t Write(SyncWriteStreamT &s, const ConstBufferT &buffer)
		{
			return Write(s, buffer, TransferAll(), 0);
		}

		template<typename SyncWriteStreamT, typename ConstBufferT>
		size_t Write(SyncWriteStreamT &s, const ConstBufferT &buffer, const Callback &callback)
		{
			return Write(s, buffer, TransferAll(), callback);
		}

		template<typename SyncWriteStreamT, typename ConstBufferT>
		size_t Write(SyncWriteStreamT &s, const ConstBufferT &buffer, const u_int64 &offset)
		{
			return Write(s, buffer, offset, TransferAll());
		}

		// 
		template<typename SyncWriteStreamT, typename ConstBufferT, typename CompleteConditionT>
		size_t Write(SyncWriteStreamT &s, const ConstBufferT &buffer, const CompleteConditionT &condition)
		{
			return Write(s, buffer, condition, 0);
		}

		template<typename SyncWriteStreamT, typename ConstBufferT, typename CompleteConditionT>
		size_t Write(SyncWriteStreamT &s, const ConstBufferT &buffer, const CompleteConditionT &condition, const Callback &callback)
		{
			size_t transfers = 0;
			const size_t bufSize = buffer.size();

			while( transfers < condition(transfers) )
			{
				if( transfers >= bufSize )
					break;

				size_t ret = s.Write(buffer + transfers);
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

		template<typename SyncWriteStreamT, typename ConstBufferT, typename CompleteConditionT>
		size_t Write(SyncWriteStreamT &s, const ConstBufferT &buffer, const u_int64 &offset, const CompleteConditionT &condition)
		{
			size_t transfers = 0;
			const size_t bufSize = buffer.size();

			while( transfers <= condition(transfers) )
			{
				if( transfers >= bufSize )
					break;

				size_t ret = s.Write(buffer + transfers, offset);
				if( ret == 0 )
				{
					s.Close();
				}

				transfers += ret;
			}

			return transfers;
		}



		namespace detail
		{
			template<typename AsyncWriteStreamT, typename ConstBufferT, typename CompletionConditionT, typename WriteHandlerT>
			class WriteHandler
			{
				typedef WriteHandler<AsyncWriteStreamT, ConstBufferT, CompletionConditionT, WriteHandlerT>	ThisType;

			private:
				AsyncWriteStreamT &stream_;
				ConstBufferT buffer_;
				CompletionConditionT condition_;
				size_t transfers_;
				const size_t total_;
				WriteHandlerT handler_;

			public:
				WriteHandler(AsyncWriteStreamT &stream, const ConstBufferT &buffer, size_t total, const CompletionConditionT &condition, size_t transfer, const WriteHandlerT &handler)
					: stream_(stream)
					, buffer_(buffer_)
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
						if( transfers_ <= condition_() )
						{
							stream_.AsyncWrite(buffer_ + size, 
								ThisType(stream_, buffer_ + size, total_, condition_, transfers_, handler_));

							return;
						}
					}
					
					// 回调
					handler_(error, transfers_);
				}
			};

			template<typename AsyncWriteStreamT, typename ConstBufferT, typename CompletionConditionT, typename WriteHandlerT>
			class WriteOffsetHandler
			{
				typedef WriteOffsetHandler<AsyncWriteStreamT, ConstBufferT, CompletionConditionT, WriteHandlerT>	ThisType;

			private:
				AsyncWriteStreamT &stream_;
				ConstBufferT buffer_;
				CompletionConditionT condition_;
				const u_int64 offset_;
				size_t transfers_;
				const size_t total_;
				WriteHandlerT handler_;

			public:
				WriteOffsetHandler(AsyncWriteStreamT &stream, const ConstBufferT &buffer, size_t total, const u_int64 &offset, const CompletionConditionT &condition, size_t transfer, const WriteHandlerT &handler)
					: stream_(stream)
					, buffer_(buffer_)
					, offset_(offset)
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
							stream_.AsyncWrite(buffer_ + size, offset_,
								ThisType(stream_, buffer_ + size, total_, offset_, condition_, transfers_, handler_));

							return;
						}
					}

					// 回调
					handler_(error, transfers_);
				}
			};
		}

		// 异步写入指定的数据

		//
		template<typename SyncWriteStreamT, typename ConstBufferT, typename HandlerT>
		void AsyncWrite(SyncWriteStreamT &s, const ConstBufferT &buffer, const HandlerT &handler)
		{
			AsyncWrite(s, buffer, TransferAll(), handler);
		}

		template<typename SyncWriteStreamT, typename ConstBufferT, typename HandlerT>
		void AsyncWrite(SyncWriteStreamT &s, const ConstBufferT &buffer, const u_int64 &offset, const HandlerT &handler)
		{
			AsyncWrite(s, buffer, offset, TransferAll(), handler);
		}

		// 
		template<typename SyncWriteStreamT, typename ConstBufferT, typename ComplateConditionT, typename HandlerT>
		void AsyncWrite(SyncWriteStreamT &s, const ConstBufferT &buffer, const ComplateConditionT &condition, const HandlerT &handler)
		{
			typedef detail::WriteHandler<SyncWriteStreamT, ConstBufferT, ComplateConditionT, HandlerT> HookWriteHandler;

			s.AsyncWrite(buffer, HookWriteHandler(s, buffer, buffer.size(), condition, 0, handler));
		}

		template<typename SyncWriteStreamT, typename ConstBufferT, typename ComplateConditionT, typename HandlerT>
		void AsyncWrite(SyncWriteStreamT &s, const ConstBufferT &buffer, const u_int64 &offset, const ComplateConditionT &condition, const HandlerT &handler)
		{
			typedef detail::WriteOffsetHandler<SyncWriteStreamT, ConstBufferT, ComplateConditionT, HandlerT> HookWriteHandler;

			s.AsyncWrite(buffer, offset, HookWriteHandler(s, buffer, buffer.size(), offset, condition, 0, handler));
		}

	}
}







#endif