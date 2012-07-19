#ifndef TUT_RESTARTABLE_H_GUARD
#define TUT_RESTARTABLE_H_GUARD

#include "tut.hpp"


namespace tut
{

	namespace util
	{

		/**
		* Escapes non-alphabetical characters in string.
		*/
		tString Escape(const tString& orig)
		{
			tString rc;
			tString::const_iterator i,e;
			i = orig.begin();
			e = orig.end();

			while (i != e)
			{
				if( (*i >= 'a' && *i <= 'z') ||
					(*i >= 'A' && *i <= 'Z') ||
					(*i >= '0' && *i <= '9') )
				{
					rc += *i;
				}
				else
				{
					rc += '\\';
					rc += ('a'+(((unsigned int)*i) >> 4));
					rc += ('a'+(((unsigned int)*i) & 0xF));
				}

				++i;
			}
			return rc;
		}

		/**
		* Un-escapes string.
		*/
		tString UnEscape(const tString& orig)
		{
			tString rc;
			tString::const_iterator i,e;
			i = orig.begin();
			e = orig.end();

			while (i != e)
			{
				if( *i != '\\')
				{
					rc += *i;
				}
				else
				{
					++i;
					if( i == e)
					{
						throw std::invalid_argument("unexpected end of string");
					}
					unsigned int c1 = *i;
					++i;
					if( i == e)
					{
						throw std::invalid_argument("unexpected end of string");
					}
					unsigned int c2 = *i;
					rc += (((c1 - 'a') << 4) + (c2 - 'a'));
				}

				++i;
			}
			return rc;
		}

		/**
		* Serialize TestResult avoiding interfering with operator <<.
		*/
		void Serialize(tOStream& os, const tut::TestResult& tr)
		{
			os << Escape(tr.groupName_) << std::endl;
			os << tr.testNum_ << ' ';

			switch(tr.result_)
			{
			case TestResult::ok:
				os << 0;
				break;
			case TestResult::fail:
				os << 1;
				break;
			case TestResult::ex:
				os << 2;
				break;
			case TestResult::warn:
				os << 3;
				break;
			case TestResult::term:
				os << 4;
				break;
			case TestResult::rethrown:
				os << 5;
				break;
			case TestResult::ex_ctor:
				os << 6;
				break;
			case TestResult::dummy:
				assert(!"Should never be called");
			default:
				throw std::logic_error("operator << : bad result_type");
			}
			os << ' ' << Escape(tr.msg_) << std::endl;
		}

		/**
		* deserialization for TestResult
		*/
		bool DeSerialize(std::istream& is, tut::TestResult& tr)
		{
			std::getline(is,tr.groupName_);
			if( is.eof())
			{
				return false;
			}
			tr.groupName_ = UnEscape(tr.groupName_);

			tr.testNum_ = -1;
			is >> tr.testNum_;
			if( tr.testNum_ < 0)
			{
				throw std::logic_error("operator >> : bad test number");
			}

			int n = -1;
			is >> n;
			switch(n)
			{
			case 0:
				tr.result_ = TestResult::ok;
				break;
			case 1:
				tr.result_ = TestResult::fail;
				break;
			case 2:
				tr.result_ = TestResult::ex;
				break;
			case 3:
				tr.result_ = TestResult::warn;
				break;
			case 4:
				tr.result_ = TestResult::term;
				break;
			case 5:
				tr.result_ = TestResult::rethrown;
				break;
			case 6:
				tr.result_ = TestResult::ex_ctor;
				break;
			default:
				throw std::logic_error("operator >> : bad result_type");
			}

			is.ignore(1); // space
			std::getline(is,tr.msg_);
			tr.msg_ = UnEscape(tr.msg_);
			if( !is.good())
			{
				throw std::logic_error("malformed test result_");
			}
			return true;
		}
	}

	/**
	* Restartable test runner wrapper.
	*/
	class RestatableWrapper
	{
		TestRunner& runner_;
		Callbacks callbacks_;

		tString dir_;
		tString log_; // log file: last test being executed
		tString jrn_; // journal file: results of all executed tests

	public:
		/**
		* Default constructor.
		* @param dir Directory where to search/put log and journal files
		*/
		RestatableWrapper(const tString& dir = ".")
			: runner_(runner.get())
			, callbacks_()
			, dir_(dir)
			, log_( dir + '/' + "log.tut" )
			, jrn_( dir + '/' + "journal.tut" )
		{
			// dozen: it works, but it would be better to use system path separator
		}

		/**
		* Stores another groupName_ for getting by name.
		*/
		void RegisterGroup(const tString& name, GroupBase* gr)
		{
			runner_.RegisterGroup(name,gr);
		}

		/**
		* Stores Callback object.
		*/
		void SetCallback(Callback* cb)
		{
			callbacks_.clear();
			callbacks_.insert(cb);
		}

		void InsertCallback(Callback* cb)
		{
			callbacks_.insert(cb);
		}

		void EraseCallback(Callback* cb)
		{
			callbacks_.erase(cb);
		}

		void SetCallbacks(const Callbacks& cb)
		{
			callbacks_ = cb;
		}

		const Callbacks& GetCallbacks() const
		{
			return runner_.GetCallbacks();
		}

		/**
		* Returns list of known test groups.
		*/
		GroupNames ListGroups() const
		{
			return runner_.ListGroups();
		}

		/**
		* Runs all tests in all groups.
		*/
		void RunTests() const
		{
			// where last run was failed
			tString fail_group;
			int fail_test;
			_ReadLog(fail_group,fail_test);
			bool fail_group_reached = (fail_group == "");

			// iterate over groups
			tut::GroupNames gn = ListGroups();
			tut::GroupNames::const_iterator gni,gne;
			gni = gn.begin();
			gne = gn.end();
			while (gni != gne)
			{
				// skip all groups before one that failed
				if( !fail_group_reached)
				{
					if( *gni != fail_group)
					{
						++gni;
						continue;
					}
					fail_group_reached = true;
				}

				// first or restarted run
				int test = (*gni == fail_group && fail_test >= 0) ? fail_test + 1 : 1;
				while(true)
				{
					// last executed test pos
					RegisterExecution(*gni,test);

					tut::TestResult tr;
					if( !runner_.RunTest(*gni,test, tr) || tr.result_ == TestResult::dummy )
					{
						break;
					}
					_RegisterTest(tr);

					++test;
				}

				++gni;
			}

			// show final results to user
			_InvokeCallback();

			// truncate files as mark of successful finish
			_Trauncate();
		}

	private:
		/**
		* Shows results from journal file.
		*/
		void _InvokeCallback() const
		{
			runner_.SetCallbacks(callbacks_);
			runner_._RunStart();

			tString current_group;
			std::ifstream ijournal(jrn_.c_str());
			while (ijournal.good())
			{
				tut::TestResult tr;
				if( !util::DeSerialize(ijournal,tr) )
				{
					break;
				}
				runner_._TestCompleted(tr);
			}

			runner_._RunCompleted();
		}

		/**
		* Register test into journal.
		*/
		void _RegisterTest(const TestResult& tr) const
		{
			std::ofstream ojournal(jrn_.c_str(), std::ios::app);
			util::Serialize(ojournal, tr);
			ojournal << std::flush;
			if( !ojournal.good())
			{
				throw std::runtime_error("unable to register test result_ in file "
					+ jrn_);
			}
		}

		/**
		* Mark the fact test going to be executed
		*/
		void RegisterExecution(const tString& grp, int test) const
		{
			// last executed test pos
			std::ofstream olog(log_.c_str());
			olog << util::Escape(grp) << std::endl << test << std::endl << std::flush;
			if( !olog.good())
			{
				throw std::runtime_error("unable to register execution in file "
					+ log_);
			}
		}

		/**
		* Truncate tests.
		*/
		void _Trauncate() const
		{
			std::ofstream olog(log_.c_str());
			std::ofstream ojournal(jrn_.c_str());
		}

		/**
		* Read log file
		*/
		void _ReadLog(tString& fail_group, int& fail_test) const
		{
			// read failure point, if any
			std::ifstream ilog(log_.c_str());
			std::getline(ilog,fail_group);
			fail_group = util::UnEscape(fail_group);
			ilog >> fail_test;

			if( !ilog.good())
			{
				fail_group = "";
				fail_test = -1;
				_Trauncate();
			}
			else
			{
				// testNum_ was terminated...
				tut::TestResult tr(fail_group, fail_test, "", tut::TestResult::term);
				_RegisterTest(tr);
			}
		}
	};

}

#endif

