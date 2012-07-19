#ifndef __IOCP_READ_WRITE_BUFFER_HPP
#define __IOCP_READ_WRITE_BUFFER_HPP


#include <vector>
#include <string>
#include <array>



namespace async
{
	namespace iocp
	{

		// ---------------------------------------------------------
		// class MutableBuffer

		// 提供安全的修改，由外界提供缓冲区
		class MutableBuffer
		{
		public:
			typedef char					value_type;
			typedef char *					pointer;
			typedef const char*				const_pointer;
			typedef value_type*				iterator;
			typedef const value_type*		const_iterator;

		public:
			value_type *data_;
			size_t size_;

		public:
			MutableBuffer()
				: data_(0)
				, size_(0)
			{}
			MutableBuffer(value_type *data, size_t size)
				: data_(data)
				, size_(size)
			{}

			MutableBuffer &operator=(const MutableBuffer &rhs)
			{
				if( &rhs != this )
				{
					data_ = rhs.data_;
					size_ = rhs.size_;
				}

				return *this;
			}

		public:
			pointer data()
			{
				return data_;
			}
			const_pointer data() const
			{
				return data_;
			}

			size_t size() const
			{
				return size_;
			}

			iterator begin()
			{
				return data_;
			}
			iterator end()
			{
				return data_ + size_;
			}

			const_iterator begin() const
			{
				return data_;
			}
			const_iterator end() const
			{
				return data_ + size_;
			}
		};

		inline MutableBuffer operator+(const MutableBuffer &buffer, size_t sz)
		{
			if( sz > buffer.size_ )
				return MutableBuffer();

			MutableBuffer::pointer newData = buffer.data_ + sz;
			size_t newSize = buffer.size_ - sz;

			return MutableBuffer(newData, newSize);
		}
		inline MutableBuffer operator+(size_t sz, const MutableBuffer &buffer)
		{
			return buffer + sz;
		}



		// ---------------------------------------------------------
		// class ConstBuffer

		// 不能修改缓冲区，由外界提供缓冲区
		class ConstBuffer
		{
		public:
			typedef char					value_type;
			typedef char *					pointer;
			typedef const char*				const_pointer;
			typedef value_type*				iterator;
			typedef const value_type*		const_iterator;

		public:
			const value_type *data_;
			size_t size_;

		public:
			ConstBuffer()
				: data_(0)
				, size_(0)
			{}
			ConstBuffer(const value_type *data, size_t size)
				: data_(data)
				, size_(size)
			{}
			ConstBuffer(const MutableBuffer &buf)
				: data_(buf.data())
				, size_(buf.size())
			{}

			ConstBuffer &operator=(const ConstBuffer &rhs)
			{
				if( &rhs != this )
				{
					data_ = rhs.data_;
					size_ = rhs.size_;
				}

				return *this;
			}

		public:
			const_pointer data() const
			{
				return data_;
			}

			size_t size() const
			{
				return size_;
			}


			const_iterator begin() const
			{
				return data_;
			}
			const_iterator end() const
			{
				return data_ + size_;
			}
		};


		inline ConstBuffer operator+(const ConstBuffer &buffer, size_t sz)
		{
			if( sz > buffer.size_ )
				return ConstBuffer();

			ConstBuffer::const_pointer newData = buffer.data_  + sz;
			size_t newSize = buffer.size_ - sz;

			return ConstBuffer(newData, newSize);
		}
		inline ConstBuffer operator+(size_t sz, const ConstBuffer &buffer)
		{
			return buffer + sz;
		}



		// ---------------------------------------------------------
		// class NullBuffer

		class NullBuffer
		{
		private:
			MutableBuffer buffer_;

		public:
			typedef MutableBuffer				value_type;
			typedef const MutableBuffer*		const_iterator;

		public:
			const_iterator begin() const
			{
				return &buffer_;
			}

			const_iterator end() const
			{
				return &buffer_;
			}
		};



		// ------------------------------------------------------
		// MutableBuffer helper

		inline MutableBuffer Buffer(const MutableBuffer &buf, size_t maxSize)
		{
			return MutableBuffer(buf.data_, buf.size_ < maxSize ? buf.size_ : maxSize);
		}

		inline MutableBuffer Buffer(char *data, size_t sz)
		{
			return MutableBuffer(data, sz);
		}
		
		template<typename PodT, size_t _N>
		inline MutableBuffer Buffer(PodT (&data)[_N])
		{
			return MutableBuffer(data, _N * sizeof(PodT));
		}
		template<typename PodT, size_t _N>
		inline MutableBuffer Buffer(PodT (&data)[_N], size_t maxSize)
		{
			return MutableBuffer(data,  _N * sizeof(PodT) < maxSize ? _N * sizeof(PodT) : maxSize);
		}

		template<typename PodT, size_t _N>
		inline MutableBuffer Buffer(std::tr1::array<PodT, _N> &data)
		{
			return MutableBuffer(data.data(), _N * sizeof(PodT));
		}
		template<typename PodT, size_t _N>
		inline MutableBuffer Buffer(std::tr1::array<PodT, _N> &data, size_t maxSize)
		{
			return MutableBuffer(data.data(), _N * sizeof(PodT) < maxSize ? _N * sizeof(PodT) : maxSize);
		}

		template<typename PodT>
		inline MutableBuffer Buffer(std::vector<PodT> &data)
		{
			return MutableBuffer(&data[0], data.size() * sizeof(PodT));
		}
		template<typename PodT>
		inline MutableBuffer Buffer(std::vector<PodT> &data, size_t maxSize)
		{
			return MutableBuffer(&data[0], data.size() * sizeof(PodT) < maxSize ? data.size() * sizeof(PodT) : maxSize);
		}
		
		// ------------------------------------------------------
		// ConstBuffer helper

		inline ConstBuffer Buffer(const ConstBuffer &buf)
		{
			return ConstBuffer(buf.data_, buf.size_);
		}
		inline ConstBuffer Buffer(const ConstBuffer &buf, size_t maxSize)
		{
			return ConstBuffer(buf.data_, buf.size_ < maxSize ? buf.size_ : maxSize);
		}
		inline ConstBuffer Buffer(const char *data, size_t sz)
		{
			return ConstBuffer(data, sz);
		}

		template<typename PodT, size_t _N>
		inline ConstBuffer Buffer(const PodT (&data)[_N])
		{
			return ConstBuffer(data, _N * sizeof(PodT));
		}
		template<typename PodT, size_t _N>
		inline ConstBuffer Buffer(const PodT (&data)[_N], size_t maxSize)
		{
			return ConstBuffer(data,  _N * sizeof(PodT) < maxSize ? _N * sizeof(PodT) : maxSize);
		}

		template<typename PodT, size_t _N>
		inline ConstBuffer Buffer(const std::tr1::array<PodT, _N> &data)
		{
			return ConstBuffer(data.data(), _N * sizeof(PodT));
		}
		template<typename PodT, size_t _N>
		inline ConstBuffer Buffer(const std::tr1::array<PodT, _N> &data, size_t maxSize)
		{
			return ConstBuffer(data.data(), _N * sizeof(PodT) < maxSize ? _N * sizeof(PodT) : maxSize);
		}

		template<typename PodT>
		inline ConstBuffer Buffer(const std::vector<PodT> &data)
		{
			return ConstBuffer(&data[0], data.size() * sizeof(PodT));
		}
		template<typename PodT>
		inline ConstBuffer Buffer(const std::vector<PodT> &data, size_t maxSize)
		{
			return ConstBuffer(&data[0], data.size() * sizeof(PodT) < maxSize ? data.size() * sizeof(PodT) : maxSize);
		}

		inline ConstBuffer Buffer(const std::string &data)
		{
			return ConstBuffer(data.data(), data.length());
		}	
		inline ConstBuffer Buffer(const std::string &data, size_t maxSize)
		{
			return ConstBuffer(data.data(), data.length() < maxSize ? data.length() : maxSize);
		}
	}
}




#endif