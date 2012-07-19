#ifndef __MMAP_FILEMAPPING_HPP
#define __MMAP_FILEMAPPING_HPP

#include "MapFile.hpp"
#include "AccessBuffer.hpp"



namespace mmap
{

	// -------------------------------------------------------------------------
	// class HandleProxy	

	template<typename Owner>
	class proxy
	{
	public:
		typedef typename Owner::size_type	size_type;
		typedef typename Owner::pos_type	pos_type;
		typedef typename Owner::Utils		Utils;

	private: 
		Owner &owner_;

	public:
		enum { AllocationGranularityBits= Owner::AllocationGranularityBits };
		enum { AllocationGranularity	= Owner::AllocationGranularity	   };
		enum { AllocationGranularityMask= Owner::AllocationGranularityMask };
	
	public:
		proxy(Owner &owner)
			: owner_(owner)
		{}

	public:
		char *view_segment(DWORD dwBasePage, DWORD dwPageCount)
		{
			return owner_.view_segment(dwBasePage, dwPageCount);
		}

		char *access_segment(DWORD dwBasePage, DWORD dwPageCount)
		{
			return owner_.access_segment(dwBasePage, dwPageCount);
		}

		char *alloc_segment(DWORD dwBasePage, DWORD dwPageCount)
		{
			return owner_.alloc_segment(dwBasePage, dwPageCount);
		}
	};


	// -------------------------------------------------------------------------
	// class FileMapping

	// 对MapFile的一层包装，提供边界对齐的保护操作

	template<typename ConfigT>
	class file_mapping_t
		: public map_file_t<ConfigT>
	{
	private:
		typedef map_file_t<ConfigT>					BaseClass;

	public:
		typedef typename BaseClass::size_type		size_type;
		typedef typename BaseClass::pos_type		pos_type;

		typedef proxy<file_mapping_t>				HandleType;
		typedef BaseClass							Utils;

	private:
		DWORD total_pages_;

		enum { _AGBits = 16 };
		// 分配粒度
		enum { _AllocationGranularityInvBits = sizeof(DWORD) * 8 - _AGBits };


	public:
		enum { AllocationGranularityBits	= _AGBits };
		enum { AllocationGranularity		= (1 << _AGBits) };
		enum { AllocationGranularityMask	= (AllocationGranularity - 1) };

	private:
		file_mapping_t(const file_mapping_t &);
		file_mapping_t &operator=(const file_mapping_t &);


	public:
		file_mapping_t()
			: total_pages_(0)
		{}
		template < typename CharT >
		file_mapping_t(const std::basic_string<CharT> &path, pos_type *offset = NULL)
		{
			open(path, offset);
		}

	public:
		DWORD total_pages() const
		{
			return total_pages_;
		}

		void close()
		{
			BaseClass::close();
			total_pages_ = 0;
		}

		bool resize(pos_type size)
		{
			total_pages_ = (size + AllocationGranularityMask) >> AllocationGranularityBits;
	
			return BaseClass::resize(size);
		}

		template < typename CharT >
		bool open(const std::basic_string<CharT> &path, pos_type *offset = 0)
		{
			if( ConfigT::GetSizeOnOpen )
			{
				pos_type size = 0;
				bool hRes = BaseClass::open(path, &size);

				total_pages_ = static_cast<DWORD>((size + AllocationGranularityMask) >> AllocationGranularityBits);

				if( offset != 0 )
					*offset = size;

				return hRes;
			}
			else
			{
				total_pages_ = 0;
				return BaseClass::open(path, 0);
			}
		}

		char *view_segment(DWORD dwBasePage, DWORD dwPageCount)
		{
			assert(BaseClass::good());

			if( dwBasePage + dwPageCount > total_pages_ )
			{
				if( dwBasePage >= total_pages_ )
					return 0;
				else
					dwPageCount = total_pages_ - dwBasePage;
			}

			return reinterpret_cast<char *>(BaseClass::map(dwBasePage << AllocationGranularityBits, dwPageCount << AllocationGranularityBits));
		}

		char *access_segment(DWORD dwBasePage, DWORD dwPageCount)
		{
			assert(BaseClass::good());

			if( dwBasePage + dwPageCount > total_pages_ )
			{
				total_pages_ = dwBasePage + dwPageCount;

				BaseClass::resize(total_pages_ << AllocationGranularityBits);
			}

			return reinterpret_cast<char *>(BaseClass::map(dwBasePage << AllocationGranularityBits, dwPageCount << AllocationGranularityBits));
		}

		char *alloc_segment(DWORD dwBasePage, DWORD dwPageCount)
		{
			assert(BaseClass::good());

			dwBasePage += total_pages_;
			total_pages_ += dwPageCount;

			BaseClass::resize(total_pages_ << AllocationGranularityBits);

			return reinterpret_cast<char *>(BaseClass::map(dwBasePage << AllocationGranularityBits, dwPageCount << AllocationGranularityBits));
		}
		
	};


	typedef file_mapping_t<mapping_readwrite> file_mapping_readwrite;
	typedef file_mapping_t<mapping_readonly>  file_mapping_readonly;
}





#endif