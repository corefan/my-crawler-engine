#include "db_storage.hpp"

#include "Utility/utility.h"
#include "MultiThread/Lock.hpp"

#include <cstdint>
#include <fstream>
#include <vector>
#include <map>
#include <string>

#include "database/sqlitepp/sqlitepp/session.hpp"
#include "database/sqlitepp/sqlitepp/statement.hpp"
#include "database/sqlitepp/sqlitepp/use.hpp"
#include "database/sqlitepp/sqlitepp/into.hpp"


namespace crawler
{
	namespace storage
	{

		namespace
		{
			const std::wstring db_path = utility::GetAppPath() + L"data/crawler.db";
			const std::wstring db_key_file = utility::GetAppPath() + L"data/crawler_key_words.dat";

		}

		struct db::impl
		{
			sqlitepp::session session_;

			typedef async::thread::AutoLock<async::thread::AutoCriticalSection> AutoLock;
			async::thread::AutoCriticalSection mutex_;
	
			typedef std::map<sqlitepp::string_t, std::uint32_t> key_words_type;
			key_words_type key_words_;

			impl(const std::wstring &db_path)
				: session_(db_path)
			{
				
			}

			~impl()
			{
				session_.close();
			}

			std::vector<sqlitepp::string_t> load_key_words(const std::wstring &path)
			{
				std::wifstream file(path);
				if( !file.good() )
					throw std::runtime_error("key_words_file not exsit!");

				file.imbue(std::locale("chs"));

				std::vector<sqlitepp::string_t> words;
				sqlitepp::string_t word;

				while(std::getline(file, word))
				{
					if( !word.empty() )
						words.push_back(word);
				}

				return words;
			}

			template < typename R >
			void lock_run(const std::function<R()> &callback)
			{
				AutoLock lock(mutex_);
				callback();
			}
		};

		void db::start()
		{
			impl_.reset(new impl(db_path));

			assert(impl_->session_.is_open());
			
			auto key_words = impl_->load_key_words(db_key_file);

			
			sqlitepp::statement st(impl_->session_);

			std::uint32_t id = 0;
			sqlitepp::string_t word;
			st << "select id, key from key_word_t", sqlitepp::into(id), sqlitepp::into(word);
			while(st.exec())
			{
				impl_->key_words_[word] = id;
			}

			std::for_each(key_words.begin(), key_words.end(), [this, &st](const sqlitepp::string_t &val)
			{
				if( impl_->key_words_.find(val) == impl_->key_words_.end() )
				{
					sqlitepp::statement st(impl_->session_);
					st << "insert into key_word_t(key) values(:key)", sqlitepp::use(val);
					st.exec();


					std::uint32_t last_id = 0; 
					st << "select last_insert_rowid() from key_word_t", sqlitepp::into(last_id);
					impl_->key_words_[val] = last_id;
				}
			}); 
	
		}

		void db::stop()
		{
			impl_.reset();
		}

		void db::save(const url_ptr &url, const buffer_type &buffer, const handle_error_type &handle_error)
		{
			assert(impl_->session_.is_open());

			std::uint32_t url_id = 0;

			try
			{
				sqlitepp::statement st(impl_->session_);

				st << L"insert into url_t (url, num) values(:url, :num)",
					sqlitepp::use(url->get_url()), sqlitepp::use(url->No_);
				{
					impl::AutoLock lock(impl_->mutex_);
					st.exec();
				}

				st << L"select last_insert_rowid() from url_t", sqlitepp::into(url_id);
				{
					impl::AutoLock lock(impl_->mutex_);
					st.exec();
				}
				
			}
			catch(std::exception &e)
			{
				handle_error(std::cref(std::string(e.what())));
			}


			std::for_each(impl_->key_words_.begin(), impl_->key_words_.end(), [this, &buffer, url_id, &handle_error](const std::pair<sqlitepp::string_t, std::uint32_t> &val)
			{
				std::string key(CT2A(val.first.c_str()));
				const char *search = ::strstr(buffer.first.get(), key.c_str());
				if( search != 0 )
				{
					try
					{
						sqlitepp::statement st(impl_->session_);

						st << L"insert into index_t(key_id, url_id) values(:key_id, :url_id)",
							sqlitepp::use(val.second), sqlitepp::use(url_id);

						impl_->lock_run<bool>(std::bind(&sqlitepp::statement::exec, std::ref(st)));
						//impl::AutoLock lock(impl_->mutex);
						//st.exec();
					}
					catch(std::exception &e)
					{
						handle_error(std::cref(std::string(e.what())));
					}
				}
	
			});
			

		}
	}
}