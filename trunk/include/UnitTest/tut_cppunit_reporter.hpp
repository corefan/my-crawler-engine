
#ifndef TUT_CPPUNIT_REPORTER
#define TUT_CPPUNIT_REPORTER

#include "tut.hpp"



namespace tut
{

	/**
	* CppUnit TUT reporter
	*/
	class CPPUnitReporter 
		: public tut::Callback
	{
		std::vector<tut::TestResult>	failedTests_;
		std::vector<tut::TestResult>	passedTests_;
		const tString					filename_;
		std::auto_ptr<tOStream>			stream_;

	public:
		explicit CPPUnitReporter(const tString &filename = _T("testResult.xml"))
			: failedTests_(),
			passedTests_(),
			filename_(filename),
			stream_(new tOfstream(filename_.c_str()))
		{
			if( !stream_->good()) 
				throw TutError(_T("Cannot open output file `") + filename_ + _T("`"));
		}

		explicit CPPUnitReporter(tOStream &stream)
			: failedTests_(),
			passedTests_(),
			filename_(),
			stream_(&stream)
		{
		}

		~CPPUnitReporter()
		{
			if( filename_.empty() )
			{
				stream_.release();
			}
		}

	private:
		CPPUnitReporter(const CPPUnitReporter &);
		CPPUnitReporter &operator=(const CPPUnitReporter &);

	public:
		void RunStart()
		{
			failedTests_.clear();
			passedTests_.clear();
		}

		void TestCompleted(const tut::TestResult& tr)
		{
			assert(tr.result_ != TestResult::dummy );
			
			if(  (tr.result_ == TestResult::ok) ||
				(tr.result_ == TestResult::skipped) )
			{
				passedTests_.push_back(tr);
			}
			else
			{
				failedTests_.push_back(tr);
			}
		}

		void RunCompleted()
		{
			int errors = 0;
			int failures = 0;
			tString failure_type;
			tString failure_msg;

			*stream_ << _T("<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\" ?>") << std::endl
				<< _T("<TestRun>") << std::endl;

			if( failedTests_.size() > 0)
			{
				*stream_ << _T("  <FailedTests>") << std::endl;

				for( size_t i= 0; i<failedTests_.size(); i++ )
				{
					switch (failedTests_[i].result_)
					{
					case TestResult::fail:
						failure_type = _T("Assertion");
						failure_msg  = _T("");
						failures++;
						break;
					case TestResult::ex:
						failure_type = _T("Assertion");
						failure_msg  = _T("Thrown exception: ") + failedTests_[i].exceptionTypeid_ + _T("\n");
						failures++;
						break;
					case TestResult::warn:
						failure_type = _T("Assertion");
						failure_msg  = _T("Destructor failed\n");
						failures++;
						break;
					case TestResult::term:
						failure_type = _T("Error");
						failure_msg  = _T("Test application terminated abnormally\n");
						errors++;
						break;
					case TestResult::ex_ctor:
						failure_type = _T("Error");
						failure_msg  = _T("Constructor has thrown an exception: ") + failedTests_[i].exceptionTypeid_ + _T("\n");
						errors++;
						break;
					case TestResult::rethrown:
						failure_type = _T("Assertion");
						failure_msg  = _T("Child failed\n");
						failures++;
						break;
					default: // ok, skipped, dummy
						failure_type = _T("Error");
						failure_msg  = _T("Unknown testNum_ status, this should have never happened. ")
							_T("You may just have found a bug in TUT, please report it immediately.\n");
						errors++;
						break;
					}

					*stream_ << _T("    <FailedTest id=\"") << failedTests_[i].testNum_ << _T("\">") << std::endl
						<< _T("      <Name>") << Encode(failedTests_[i].groupName_) + _T("::") + Encode(failedTests_[i].name_) << _T("</Name>") << std::endl
						<< _T("      <FailureType>") << failure_type << _T("</FailureType>") << std::endl
						<< _T("      <Location>") << std::endl
						<< _T("        <File>Unknown</File>") << std::endl
						<< _T("        <Line>Unknown</Line>") << std::endl
						<< _T("      </Location>") << std::endl
						<< _T("      <Message>") << Encode(failure_msg + failedTests_[i].msg_) << _T("</Message>") << std::endl
						<< _T("    </FailedTest>") << std::endl;
				}

				*stream_ << _T("  </FailedTests>") << std::endl;
			}

			/* *********************** passed tests ***************************** */
			if( passedTests_.size() > 0) 
			{
				*stream_ << _T("  <SuccessfulTests>") << std::endl;

				for (unsigned int i=0; i<passedTests_.size(); i++)
				{
					*stream_ << _T("    <Test id=\"") << passedTests_[i].testNum_ << _T("\">") << std::endl
						<< _T("      <Name>") << Encode(passedTests_[i].groupName_) + _T("::") + Encode(passedTests_[i].name_) << _T("</Name>") << std::endl
						<< _T("    </Test>") << std::endl;
				}

				*stream_ << _T("  </SuccessfulTests>") << std::endl;
			}

			/* *********************** statistics ***************************** */
			*stream_ << _T("  <Statistics>") << std::endl
				<< _T("    <Tests>") << (failedTests_.size() + passedTests_.size()) << _T("</Tests>") << std::endl
				<< _T("    <FailuresTotal>") << failedTests_.size() << _T("</FailuresTotal>") << std::endl
				<< _T("    <Errors>") << errors << _T("</Errors>") << std::endl
				<< _T("    <Failures>") << failures << _T("</Failures>") << std::endl
				<< _T("  </Statistics>") << std::endl;

			/* *********************** footer ***************************** */
			*stream_ << _T("</TestRun>") << std::endl;
		}

		bool AllOK() const
		{
			return failedTests_.empty();
		}

		/**
		* \brief Encodes text to XML
		* XML-reserved characters (e.g. "<") are encoded according to specification
		* @param text text to be encoded
		* @return encoded string
		*/
		static tString Encode(const tString & text)
		{
			tString out;

			for (unsigned int i=0; i<text.length(); ++i) 
			{
				TCHAR c = text[i];
				switch (c) 
				{
				case '<':
					out += _T("&lt;");
					break;
				case '>':
					out += _T("&gt;");
					break;
				case '&':
					out += _T("&amp;");
					break;
				case '\'':
					out += _T("&apos;");
					break;
				case '"':
					out += _T("&quot;");
					break;
				default:
					out += c;
				}
			}

			return out;
		}
	};

}

#endif

