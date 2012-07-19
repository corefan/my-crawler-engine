#ifndef TUT_XML_REPORTER
#define TUT_XML_REPORTER


#include "tut.hpp"
#include "tut_cppunit_reporter.hpp"


namespace tut
{


	class XMLReporter 
		: public tut::Callback
	{
	private:
		typedef std::vector<tut::TestResult> TestResults;
		typedef std::map<tString, TestResults> TestGroups;

		TestGroups all_tests_; /// holds all test results
		const tString filename_; /// filename base
		std::auto_ptr<tOStream> stream_;


	public:
		int ok_count;           // number of passed tests
		int exceptions_count;   // number of tests that threw exceptions
		int failures_count;     // number of tests that failed
		int terminations_count; // number of tests that would terminate
		int warnings_count;     // number of tests where destructors threw an exception

		/**
		* \brief Default constructor
		* @param filename base filename
		*/
		XMLReporter(const tString & filename)
			: all_tests_(),
			filename_(filename),
			stream_(new std::ofstream(filename_.c_str())),
			ok_count(0),
			exceptions_count(0),
			failures_count(0),
			terminations_count(0),
			warnings_count(0)
		{
			if( !stream_->good())
				throw TutError(_T("Cannot open output file `") + filename_ + _T("`"));
		}

		XMLReporter(tOStream & stream)
			: all_tests_(),
			filename_(),
			stream_(&stream),
			ok_count(0),
			exceptions_count(0),
			failures_count(0),
			terminations_count(0),
			warnings_count(0)
		{}

		~XMLReporter()
		{
			if(filename_.empty())
			{
				stream_.release();
			}
		}

	public:
		/**
		* \brief Builds "testcase" XML entity with given parameters
		* Builds \<testcase\> entity according to given parameters. \<testcase\>-s are part of \<testsuite\>.
		* @param tr test result_ to be used as source data
		* @param failure_type type of failure to be reported ("Assertion" or "Error", empty if test passed)
		* @param failure_msg failure msg_ to be reported (empty, if test passed)
		* @return string with \<testcase\> entity
		*/
		tString XMLBuildCase(const tut::TestResult & tr, const tString & failure_type, const tString & failure_msg)
		{
			using std::endl;
			using tString;

			std::ostringstream out;

			if(  (tr.result_ == TestResult::ok) ||
				(tr.result_ == TestResult::skipped) )
			{
				out << _T("    <testcase classname=\"") << CPPUnitReporter::Encode(tr.groupName_) << _T("\" name=\"") << CPPUnitReporter::Encode(tr.name_) << _T("\"/>");
			}
			else
			{
				string err_msg = CPPUnitReporter::Encode(failure_msg + tr.msg_);

				string tag; // determines tag name_: "failure" or "error"
				if(  tr.result_ == TestResult::fail || tr.result_ == TestResult::warn ||
					tr.result_ == TestResult::ex || tr.result_ == TestResult::ex_ctor || tr.result_ == TestResult::rethrown )
				{
					tag = _T("failure");
				}
				else
				{
					tag = _T("error");
				}

				out << _T("    <testcase classname=\"") << CPPUnitReporter::Encode(tr.groupName_) << _T("\" name=\"") << CPPUnitReporter::Encode(tr.name_) << _T("\">") << endl;
				out << _T("      <") << tag << _T(" msg_=\"") << err_msg << _T("\"") << _T(" type=\"") << failure_type << _T("\"");
				out << _T(">") << err_msg << _T("</") << tag << _T(">") << endl;
				out << _T("    </testcase>");
			}

			return out.str();
		}

		/**
		* \brief Builds "testsuite" XML entity
		* Builds \<testsuite\> XML entity according to given parameters.
		* @param errors number of errors to be reported
		* @param failures number of failures to be reported
		* @param total total number of tests to be reported
		* @param name test suite name
		* @param testcases CPPUnitReporter::encoded XML string containing testcases
		* @return string with \<testsuite\> entity
		*/
		tString XMLBuildSuite(int errors, int failures, int total,
			const tString & name, const tString & testcases)
		{
			std::ostringstream out;

			out << _T("  <testsuite errors=\"") << errors << _T("\" failures=\"") << failures << _T("\" tests=\"") << total << _T("\" name=\"") << CPPUnitReporter::Encode(name) << _T("\">") << std::endl;
			out << testcases;
			out << _T("  </testsuite>");

			return out.str();
		}

		/**
		* \brief Callback function
		* This function is called before the first test is executed. It initializes counters.
		*/
		virtual void RunStart()
		{
			ok_count = 0;
			exceptions_count = 0;
			failures_count = 0;
			terminations_count = 0;
			warnings_count = 0;
			all_tests_.clear();
		}

		/**
		* \brief Callback function
		* This function is called when test completes. Counters are updated here, and test results stored.
		*/
		virtual void TestCompleted(const tut::TestResult& tr)
		{
			// update global statistics
			switch (tr.result_) {
	case TestResult::ok:
	case TestResult::skipped:
		ok_count++;
		break;
	case TestResult::fail:
	case TestResult::rethrown:
		failures_count++;
		break;
	case TestResult::ex:
	case TestResult::ex_ctor:
		exceptions_count++;
		break;
	case TestResult::warn:
		warnings_count++;
		break;
	case TestResult::term:
		terminations_count++;
		break;
	case tut::TestResult::dummy:
		assert(!"Should never be called");
			} // switch

			// add test result_ to results table
			all_tests_[tr.groupName_].push_back(tr);
		}

		/**
		* \brief Callback function
		* This function is called when all tests are completed. It generates XML output
		* to file(s). File name_ base can be set with constructor.
		*/
		virtual void RunCompleted()
		{
			/* *********************** header ***************************** */
			*stream_ << _T("<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>") << std::endl;
			*stream_ << _T("<testsuites>") << std::endl;

			// iterate over all test groups
			for (TestGroups::const_iterator tgi = all_tests_.begin(); tgi != all_tests_.end(); ++tgi)
			{
				/* per-groupName_ statistics */
				int passed = 0;         // passed in single groupName_
				int exceptions = 0;     // exceptions in single groupName_
				int failures = 0;       // failures in single groupName_
				int terminations = 0;   // terminations in single groupName_
				int warnings = 0;       // warnings in single groupName_
				int errors = 0;         // errors in single groupName_


				// output is written to string stream buffer, because JUnit format <testsuite> tag
				// contains statistics, which aren't known yet
				std::ostringstream out;

				// iterate over all test cases in the current test groupName_
				const TestResults &results = tgi->second;
				for (TestResults::const_iterator tri = results.begin(); tri != results.end(); ++tri)
				{
					tString failure_type;    // string describing the failure type
					tString failure_msg;     // a string with failure msg_

					switch (tri->result_)
					{
					case TestResult::ok:
					case TestResult::skipped:
						passed++;
						break;
					case TestResult::fail:
						failure_type = _T("Assertion");
						failure_msg  = _T("");
						failures++;
						break;
					case TestResult::ex:
						failure_type = _T("Assertion");
						failure_msg  = _T("Thrown exception: ") + tri->exceptionTypeid_ + '\n';
						exceptions++;
						break;
					case TestResult::warn:
						failure_type = _T("Assertion");
						failure_msg  = _T("Destructor failed.\n");
						warnings++;
						break;
					case TestResult::term:
						failure_type = _T("Error");
						failure_msg  = _T("Test application terminated abnormally.\n");
						terminations++;
						break;
					case TestResult::ex_ctor:
						failure_type = _T("Assertion");
						failure_msg  = _T("Constructor has thrown an exception: ") + tri->exceptionTypeid_ + _T(".\n");
						exceptions++;
						break;
					case TestResult::rethrown:
						failure_type = _T("Assertion");
						failure_msg  = _T("Child failed.\n");
						failures++;
						break;
					default:
						failure_type = _T("Error");
						failure_msg  = _T("Unknown test status, this should have never happened. ")
							_T("You may just have found a bug in TUT, please report it immediately.\n");
						errors++;
						break;
					} // switch

					out << XMLBuildCase(*tri, failure_type, failure_msg) << std::endl;

				} // iterate over all test cases

				// calculate per-groupName_ statistics
				int stat_errors = terminations + errors;
				int stat_failures = failures + warnings + exceptions;
				int stat_all = stat_errors + stat_failures + passed;

				*stream_ << XMLBuildSuite(stat_errors, stat_failures, stat_all, (*tgi).first/* name_ */, out.str()/* testcases */) << std::endl;
			} // iterate over all test groups

			*stream_ << _T("</testsuites>" << std::endl;
		}

		/**
		* \brief Returns true, if all tests passed
		*/
		virtual bool AllOK() const
		{
			return ( (terminations_count + failures_count + warnings_count + exceptions_count) == 0);
		};
	};

}

#endif
