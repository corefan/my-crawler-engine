#ifndef __WIN_HTTP_QUERY_HEADER_HPP
#define __WIN_HTTP_QUERY_HEADER_HPP


namespace http
{
	namespace query
	{

		template < int LevelT >
		struct header
		{
			wchar_t buffer_[1024];
			DWORD buffer_len_;
			std::vector<wchar_t> dynamic_buffer_;

			header()
				: buffer_len_(_countof(buffer_))
			{
				std::uninitialized_fill(buffer_, buffer_ + buffer_len_, 0);
			}

			DWORD level() const
			{
				return LevelT;
			}

			const wchar_t *name() const
			{
				return WINHTTP_HEADER_NAME_BY_INDEX;
			}

			void *buffer()
			{
				if( dynamic_buffer_.size() != 0 )
					return dynamic_buffer_.data();

				return buffer_;
			}

			DWORD *length()
			{
				return &buffer_len_;
			}

			DWORD *index()
			{
				return WINHTTP_NO_HEADER_INDEX;
			}

			void resize(size_t len)
			{
				dynamic_buffer_.resize(len);
			}
		};



		template < >
		struct header<WINHTTP_QUERY_CUSTOM>
		{
			~header()
			{
				//static_assert(false, "not support");
			}
		};



		typedef header<WINHTTP_QUERY_RAW_HEADERS_CRLF>				raw_headers;
		typedef header<WINHTTP_QUERY_CONTENT_TYPE>					content_type;
		typedef header<WINHTTP_QUERY_DATE>							date;
		typedef header<WINHTTP_QUERY_EXPIRES>						expires;
		
	}
}




#endif