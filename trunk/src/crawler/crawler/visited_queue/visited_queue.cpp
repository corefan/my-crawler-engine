#include "stdafx.h"
#include "visited_queue.hpp"


#include "third_party/leveldb/db.h"
#include "Unicode/string.hpp"
#include "Utility/utility.h"
#include "MultiThread/atomic/atomic.hpp"
#include "url.hpp"


namespace crawler
{
	namespace
	{
		std::string translate(const url_ptr &val)
		{
			return unicode::to_a(val->get_url());
		}
	}
	


	struct visited_queue::impl
	{
		leveldb::DB *db_;
		
	};	

	visited_queue::visited_queue()
		: impl_(new impl)
	{

	}

	visited_queue::~visited_queue()
	{

	}

	void visited_queue::start()
	{
		leveldb::Options opt;
		opt.create_if_missing = true;

		std::wstring path = utility::GetAppPath() + L"data/visited_queue.db";
		leveldb::Status status = leveldb::DB::Open(opt, unicode::to_a(path), &impl_->db_);
		assert(status.ok());
		if( !status.ok() )
			throw std::runtime_error(status.ToString());
	}

	void visited_queue::stop()
	{
		if( impl_->db_ != 0 )
			delete impl_->db_;
	}

	void visited_queue::put(const url_ptr &val)
	{
		leveldb::Status status = impl_->db_->Put(leveldb::WriteOptions(), leveldb::Slice(translate(val)), leveldb::Slice());
		assert(status.ok());
	}

	bool visited_queue::is_exsit(const url_ptr &val) const
	{
		static std::string value;
		leveldb::Status status = impl_->db_->Get(leveldb::ReadOptions(), leveldb::Slice(translate(val)), &value);

		return !status.IsNotFound();
	}
}