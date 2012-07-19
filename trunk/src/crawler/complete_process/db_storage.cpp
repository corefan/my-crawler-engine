#include "db_storage.hpp"

#include "Utility/utility.h"
#include "MultiThread/Lock.hpp"


namespace crawler
{
	namespace storage
	{

		namespace
		{
			const std::wstring db_path = utility::GetAppPath() + L"data/html/crawler.db";

			typedef async::thread::AutoLock<async::thread::AutoCriticalSection> AutoLock;
			async::thread::AutoCriticalSection mutex;
		}

		void db::start()
		{
			session_.reset(new sqlitepp::session(db_path));
		}

		void db::stop()
		{
			session_->close();
		}

		void db::save(const url_ptr &url, const buffer_type &buffer, const handle_error_type &handle_error)
		{
			assert(session_->is_open());

			try
			{
				sqlitepp::statement st(*session_);

				st << L"insert into url(url, num) values(:url, :num)",
					sqlitepp::use(url->get_url()), sqlitepp::use(url->No_);
				
				AutoLock lock(mutex);
				st.exec();
			}
			catch(std::exception &e)
			{
				handle_error(std::cref(std::string(e.what())));
			}
		}
	}
}