#include "stdafx.h"
#include "todo_queue.hpp"

#include "Container/BlockingQueue.hpp"
#include "third_party/leveldb/db.h"
#include "Unicode/string.hpp"
#include "Utility/utility.h"
#include "url.hpp"


namespace crawler
{
	std::string translate(const url_ptr &val)
	{
		return unicode::to_a(val->get_url());
	}

	template < typename DBT, typename QueueT>
	void db_to_queue(DBT *db, QueueT &queue, size_t max)
	{
		size_t i = 0;
		std::auto_ptr<leveldb::Iterator> it(db->NewIterator(leveldb::ReadOptions()));
		for(it->SeekToFirst(); it->Valid(); it->Next()) 
		{
			if( i >= max )
				break;

			url_ptr val(create_url(unicode::to_w(it->key().ToString())));
			queue.Put(val);

			db->Delete(leveldb::WriteOptions(), it->key());
		}
	}

	template < typename DBT, typename QueueT >
	void queue_to_db(DBT *db, QueueT &queue)
	{
		queue.for_each([&db](const url_ptr &val)
		{
			leveldb::WriteOptions options;
			db->Put(options, translate(val), leveldb::Slice());
		});
	}


	struct todo_queue::impl
	{
		enum 
		{
			MIN_SIZE = 0, 
			MAX_SIZE = 10000 
		};

		typedef async::container::BlockingQueue<url_ptr> queue_type;
		queue_type queue_;

		leveldb::DB *db_;

		todo_queue::handle_error_type handle_error_;

		impl()
			: db_(0)
		{}
	};

	todo_queue::todo_queue()
		: impl_(new impl)
	{

	}

	todo_queue::~todo_queue()
	{

	}


	void todo_queue::start(const handle_error_type &error)
	{
		impl_->handle_error_ = error;

		leveldb::Options opt;
		opt.create_if_missing = true;

		std::wstring path = utility::GetAppPath() + L"data/todo_queue.db";
		leveldb::Status status = leveldb::DB::Open(opt, unicode::to_a(path), &impl_->db_);
		assert(status.ok());
		if( !status.ok() )
			throw std::runtime_error(status.ToString());

		db_to_queue(impl_->db_, impl_->queue_, impl::MAX_SIZE);
	}

	void todo_queue::stop()
	{
		queue_to_db(impl_->db_, impl_->queue_);

		if( impl_->db_ != 0 )
			delete impl_->db_;
	}


	void todo_queue::put(const url_ptr &val)
	{
		try
		{
			if( impl_->queue_.Size() < impl::MAX_SIZE )
				impl_->queue_.Put(val);
			else
			{
				leveldb::WriteOptions options;
				impl_->db_->Put(options, translate(val), leveldb::Slice());
			}
		}
		catch(std::exception &e)
		{
			impl_->handle_error_(std::cref(std::string(e.what())));
		}
	}

	url_ptr todo_queue::get()
	{
		try
		{
			if( impl_->queue_.Size() <= impl::MIN_SIZE )
			{
				db_to_queue(impl_->db_, impl_->queue_, impl::MAX_SIZE);
			}
		}
		catch(std::exception &e)
		{
			impl_->handle_error_(std::cref(std::string(e.what())));
		}

		return impl_->queue_.Get();
	}
}