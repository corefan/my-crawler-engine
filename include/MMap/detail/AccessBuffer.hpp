#ifndef __MMAP_ACCESS_BUFFER_HPP
#define __MMAP_ACCESS_BUFFER_HPP



namespace mmap
{

	namespace detail
	{
		template< int v >
		struct Int2Type
		{
			enum { value = v };
		};
	}
	// -------------------------------------------------------------------------
	// class SegmentAllocBuffer 
	//
	// 可作为Allocator
	// 
	// 	SegmentSize = (1 << SegBits) is maximum bytes that user can allocate.
	//

	#pragma pack(1)
	struct SegmentAllocInfo
	{
		DWORD basePage_;			// index number of current block for allocating
		DWORD freeSize_;			// free bytes of current block
	};
	#pragma pack()



	template< typename BaseT, size_t SegBits = 16 >
	class segment_buffer_t
		: public BaseT
		, private SegmentAllocInfo
	{
	public:
		typedef typename BaseT::size_type	size_type;
		typedef typename BaseT::pos_type	pos_type;

	private:
		char *view_;
	
	public:
		enum { AllocationGranularityBits	= BaseT::AllocationGranularityBits };
		enum { SegmentBits					= SegBits > AllocationGranularityBits ? SegBits : AllocationGranularityBits };
		enum { SegmentSize					= 1 << SegmentBits };
		enum { SegmentSizeMax				= SegmentSize - 1 };

	private:
		enum { AlignBits_ = SegmentBits - AllocationGranularityBits };
		enum { AlignSize_ = 1 << AlignBits_ };

		typedef typename BaseT::Utils		Utils;

	public:
		segment_buffer_t()
			: view_(0)
		{
			basePage_	= 0;
			freeSize_	= 0;
		}
		template<typename T1>
		explicit segment_buffer_t(T1 &arg1)
			: view_(0)
			, Base(arg1)
		{
			basePage_	= 0;
			freeSize_	= 0;
		}
		template<typename T1>
		explicit segment_buffer_t(const T1 &arg1)
			: view_(0)
			, Base(arg1)
		{
			basePage_	= 0;
			freeSize_	= 0;
		}
		~segment_buffer_t()
		{
			if( view_ != NULL )
				Utils::unmap(view_, SegmentSize);
		}
	
	private:
		segment_buffer_t(const segment_buffer_t &);
		segment_buffer_t &operator=(const segment_buffer_t &);

	
	public:
		void close()
		{
			if( view_ != NULL )
			{
				Utils::unmap(view_, SegmentSize);
				view_ = NULL;
				freeSize_ = 0;

				Base::close();
			}
		}

		void flush()
		{
			assert(view_ == 0);

			if( view_ != NULL )
			{
				Utils::flush(view_, SegmentSize);
			}
		}

		bool init_buffer(DWORD dwBasePage = 0)
		{
			assert(view_ == 0 && freeSize_ == 0);

			view_ = Base::access_segment(dwBasePage, AlignSize_);
			if( view_ != 0 )
			{
				basePage_	= dwBasePage;
				freeSize_	= SegmentSize;
				
				return true;
			}

			return false;
		}

		bool init_buffer(const SegmentAllocInfo &info)
		{
			assert(view_ == 0 && freeSize_ == 0);
			
			view_ = Base::access_segment(info.basePage_, AlignSize_);
			if( view_ != 0 )
			{
				*static_cast<SegmentAllocInfo *>(this) = info;
				return true;
			}

			return false;
		}


		void get_alloc_info(SegmentAllocInfo &info) const 
		{
			info = *static_cast<const SegmentAllocInfo *>(this);
		}
		const SegmentAllocInfo &get_alloc_info() const
		{
			return *static_cast<const SegmentAllocInfo *>(this);
		}

		char *alloc_data(size_type size, pos_type &pos)
		{
			assert(SegmentBits >= AllocationGranularityBits);
			assert(size <= SegmentSize);


			if( size <= freeSize_ )
			{
				char *pData = view_ + (SegmentSize - freeSize_);
				pos = static_cast<pos_type>(basePage_ << AllocationGranularityBits) + (SegmentSize - freeSize_);
				freeSize_ -= size;

				return pData;
			}

			if( size <= SegmentSize )
			{
				if( view_ != 0 )
					Utils::unmap(view_, SegmentSize);

				view_ = BaseT::alloc_segment(AlignSize_, basePage_);
				assert(view_);

				if( view_ != 0 )
				{
					freeSize_ = SegmentSize;
					
					char *pData = view_ + (SegmentSize - freeSize_);
					pos = static_cast<pos_type>(basePage_ << AllocationGranularityBits) + (SegmentSize - freeSize_);
					freeSize_ -= size;

					return pData;
				}

				freeSize_ = 0;
			}

			return NULL;
		}
	};



	// -------------------------------------------------------------------------
	// class SegmentAccessBuffer
	//
	// 访问数据
	//	
	// SegmentSize = (1<<SegBits) is maximum bytes that user can access.
	// And user can't access memory across	the boundary of Segments.
	//
	template< typename BaseT, size_t SegBits = 16, bool ReadOnly = false >
	class segment_access_buffer_t
		: public BaseT
	{
	public:
		typedef typename BaseT::size_type size_type;
		typedef typename BaseT::pos_type  pos_type;

	public:
		enum { AllocationGranularityBits = BaseT::AllocationGranularityBits };
		enum { SegmentBits = SegBits > AllocationGranularityBits ? SegBits : AllocationGranularityBits };
		enum { SegmentSize = 1 << SegmentBits };
		enum { SegmentSizeMask = SegmentSize - 1 };

	private:
		enum { AlignBits_ = SegmentBits - AllocationGranularityBits };
		enum { AlignSize_ = 1 << AlignBits_ };

		typedef typename BaseT::Utils Utils;

	private:
		char* view_;
		DWORD seg_;

	public:
		segment_access_buffer_t() 
			: view_(0)
			, seg_(-1L) 
		{}

		template<typename T1>
		explicit segment_access_buffer_t(T1& arg1)
			: view_(0)
			, seg_(-1L)
			, Base(arg1)
		{}

		template<typename T1>
		explicit segment_access_buffer_t(const T1& arg1)
			: view_(0)
			, seg_((DWORD)-1)
			, Base(arg1) 
		{}

		~segment_access_buffer_t()
		{
			if( view_ != 0 )
				Utils::unmap(view_, SegmentSize);
		}

	private:
		segment_access_buffer_t(const segment_access_buffer_t&);
		segment_access_buffer_t &operator=(const segment_access_buffer_t&);

	public:
		char* get_current_view() const
		{
			return view_;
		}

		DWORD get_current_seg() const
		{
			return seg_;
		}

		void close()
		{
			if( view_ != 0 )
			{
				Utils::unmap(view_, SegmentSize);
				view_	= 0;
				seg_	= -1L;

				Base::close();
			}
		}

		void flush()
		{
			if( view_ != 0 )
				Utils::flush(view_, SegmentSize);
		}

		char* view_segment(DWORD dwSeg)
		{
			return dwSeg == seg_ ? view_ : _ViewSegment(dwSeg);
		}

		char* view(pos_type fc)
		{
			DWORD dwSeg = static_cast<DWORD>(fc >> SegmentBits);
			return dwSeg == seg_ ? view_ + (SegmentSizeMask & fc) : _View(dwSeg, fc);
		}


	private:
		char* _ViewSegment(DWORD dwSeg)
		{
			return _View(dwSeg, 0);
		}

		char* _View(DWORD dwSeg, pos_type fc)
		{
			if( view_ != NULL )
				Utils::unmap(view_, SegmentSize);

			view_ = _Access(dwSeg, detail::Int2Type<ReadOnly>());
			assert(view_);

			if( view_ != NULL )
			{
				seg_ = dwSeg;
				return view_ + (SegmentSizeMask & static_cast<size_type>(fc));
			}
			else 
			{
				seg_ = -1L;
				return NULL;
			}
		}

	private:
		char *_Access(DWORD dwSeg, detail::Int2Type<true>)
		{
			return BaseT::view_segment(dwSeg << AlignBits_, AlignSize_);
		}
		char *_Access(DWORD dwSeg, detail::Int2Type<false>)
		{
			return BaseT::access_segment(dwSeg << AlignBits_, AlignSize_);
		}
	};



	//-------------------------------------------------------------------------
	// class SegmentViewBuffer

	template< typename OwnerT, size_t SegBits = 16 >
	class segment_view_buffer_t 
		: public segment_access_buffer_t<OwnerT, SegBits, true>
	{
	private:
		typedef segment_access_buffer_t<OwnerT, SegBits, true> BaseClass;

	public:
		segment_view_buffer_t() 
		{}

		template<typename T1>
		explicit segment_view_buffer_t(T1& arg1)
			: BaseClass(arg1) 
		{}

		template<typename T1>
		explicit segment_view_buffer_t(const T1& arg1)
			: BaseClass(arg1) 
		{}
	};



	// -------------------------------------------------------------------------
	// class AccessBuffer

	template<typename Base, bool ReadOnly = false>
	class access_buffer_t 
		: public Base
	{
	public:
		typedef typename Base::size_type size_type;
		typedef typename Base::pos_type	pos_type;


	private:
		char* view_;
		DWORD m_dwBasePage;
		DWORD m_dwNextPage;

	public:
		enum { AllocationGranularity	 = Base::AllocationGranularity };
		enum { AllocationGranularityBits = Base::AllocationGranularityBits };
		enum { AllocationGranularityMask = Base::AllocationGranularityMask };

	public:
		access_buffer_t() 
			: view_(NULL)
			, m_dwBasePage(LONG_MAX)
			, m_dwNextPage(0) 
		{}
		template<typename T1>
		access_buffer_t(T1& arg1)
			: view_(NULL)
			, m_dwBasePage(LONG_MAX)
			, m_dwNextPage(0)
			, Base(arg1)
		{}
		template<typename T1>
		access_buffer_t(const T1& arg1)
			: view_(NULL)
			, m_dwBasePage(LONG_MAX)
			, m_dwNextPage(0)
			, Base(arg1) 
		{}
		~access_buffer_t()
		{
			if( view_ != NULL )
			{
				Utils::unmap(view_, (m_dwNextPage - m_dwBasePage) << AllocationGranularityBits);
			}
		}

	private:
		access_buffer_t(const access_buffer_t&);
		access_buffer_t &operator=(const access_buffer_t&);

		typedef typename Base::Utils Utils;

	public:
		void close()
		{
			if( view_ != NULL )
			{
				Utils::unmap(view_, (m_dwNextPage - m_dwBasePage) << AllocationGranularityBits);

				view_		 = NULL;
				m_dwBasePage = LONG_MAX;
				m_dwNextPage = 0;

				Base::close();
			}
		}

		void flush()
		{
			if( view_ != NULL )
			{
				assert(Utils::flush(view_, (m_dwNextPage - m_dwBasePage) << AllocationGranularityBits));
			}
		}

		char* view(pos_type offset, size_type count)
		{
			DWORD dwBasePage = static_cast<DWORD>(offset >> AllocationGranularityBits);
			DWORD dwNextPage = static_cast<DWORD>((offset + count + AllocationGranularityMask) >> AllocationGranularityBits);

			assert(count > 0);
			assert(dwBasePage < dwNextPage);

			if( m_dwNextPage < dwNextPage || m_dwBasePage > dwBasePage )
			{
				if( view_ != 0 )
				{
					Utils::unmap(view_, (m_dwNextPage - m_dwBasePage) << AllocationGranularityBits);
				}

				view_ = _Access(dwBasePage, dwNextPage, detail::Int2Type<ReadOnly>());

				assert(view_);

				if( view_ == 0 )
					return NULL;

				m_dwBasePage = dwBasePage;
				m_dwNextPage = dwNextPage;
			}

			size_t offsetInPage = static_cast<size_t>(offset - (m_dwBasePage << AllocationGranularityBits));

			return view_ + offsetInPage;
		}


	private:
		char *_Access(DWORD dwBasePage, DWORD dwNextPage, detail::Int2Type<true>)
		{
			return Base::view_segment(dwBasePage, dwNextPage - dwBasePage);
		}
		char *_Access(DWORD dwBasePage, DWORD dwNextPage, detail::Int2Type<false>)
		{
			return Base::access_segment(dwBasePage, dwNextPage - dwBasePage);
		}
	};





	//-------------------------------------------------------------------
	// class AccessBuffer

	template<typename Owner>
	class view_buffer_t 
		: public access_buffer_t<Owner, true>
	{
	private:
		typedef access_buffer_t<Owner, true> Base;

	public:
		view_buffer_t() 
		{}
		template<typename T1>
		view_buffer_t(T1& arg1)
			: Base(arg1) 
		{}
		template<typename T1>
		view_buffer_t(const T1& arg1)
			: Base(arg1) 
		{}
	};
}


#endif