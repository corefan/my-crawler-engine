#ifndef __SCOPE_LOG_STORAGE_HPP
#define __SCOPE_LOG_STORAGE_HPP

#include "Storage.hpp"
#include <vector>

namespace logsystem
{
	namespace storage
	{
		// ---------------------------------------------------------------------
		// PrintScopeMessage

		template<typename StorageT, typename CharT, typename SizeT>
		inline void PrintScopeMessage(StorageT &stg, const CharT *pMsg, const CharT *pMsgEnd, char chRep, SizeT uRepCount)
		{
			for(; ;)
			{
				if( pMsg == pMsgEnd )
					return;

				const CharT *iter1 = std::find(pMsg, pMsgEnd, '\n');
				const CharT *iter2 = (iter1 == pMsgEnd ? pMsgEnd : iter1 + 1);

				stg.Put(uRepCount, chRep);
				stg.Put(pMsg, iter2 - pMsg);

				if( iter1 == pMsgEnd )
				{
					stg.Put('\n');
					return;
				}

				pMsg = iter2;
			}
		}



		// ---------------------------------------------------------------------
		// class ScopeStorageT

		template<typename ImplStorageT, typename Base = StorageBase<typename ImplStorageT::char_type>>
		class ScopeStorageT
			: public Base
		{
			typedef Base								StorageType;
			typedef ImplStorageT						ImpleStorageType;

		public:
			typedef typename StorageType::char_type		char_type;
			typedef typename StorageType::size_type		size_type;

			typedef std::basic_string<char_type>		StringType;
			typedef StringStorageT<StringType>			StringStorageType;
			typedef std::vector<StringType>				QueuedMessageType;


		public:
			struct LogTo
			{
				ImpleStorageType &storage_;
				unsigned int level_;

				LogTo(ImpleStorageType &stg)
					: storage_(stg)
					, level_(0)
				{}

				void operator()(StringType &str)
				{
					if( str.size() > 0 )
					{
						const char_type *pMsg = str.c_str();
						PrintScopeMessage(storage_, pMsg, pMsg + str.size(), ' ', level_ << 2);
						str.erase();
					}

					++level_;
				}
			};

		private:
			QueuedMessageType scope_;
			StringStorageType stringStorage_;
			ImpleStorageType  implStorage_;

		public:
			ScopeStorageT()
			{}
			template<typename ArgT>
			ScopeStorageT(const ArgT &arg)
				: implStorage_(arg)
			{}
			template<typename ArgT1, typename ArgT2>
			ScopeStorageT(const ArgT1 &arg1, const ArgT2 &arg2)
				: implStorage_(arg1, arg2)
			{}
			~ScopeStorageT()
			{
				Commit();
			}


		public:
			void EnterScope()
			{
				scope_.push_back(stringStorage_());
				stringStorage_().erase();
			}

			void LeaveScope()
			{
				assert(!scope_.empty());

				if( !scope_.empty() )
				{
					stringStorage_().assign(scope_.back());
					scope_.pop_back();
				}
			}

			void Commit()
			{
				if( implStorage_.Good() )
				{
					LogTo log(implStorage_);
					std::for_each(scope_.begin(), scope_.end(), log)(stringStorage_());
				}
			}

			void Close()
			{
				implStorage_.Close();
			}


		public:
			void Put(char_type ch)
			{
				stringStorage_.Put(ch);
			}

			void Put(size_type count, char_type ch)
			{
				stringStorage_.Put(count, ch);
			}

			void Put(const char_type *pStr, size_type count)
			{
				stringStorage_.Put(pStr, count);
			}

			void Put(const char_type *pStr, va_list args)
			{
				stringStorage_.Put(pStr, args);
			}

			void Flush()
			{
				stringStorage_.Flush();
			}
		};

	}
}





#endif