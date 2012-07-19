#pragma once

#include <cassert>

#include <string>
#include <iostream>

#include "../AssioVector.hpp"


typedef std::map< std::string, unsigned int > StudentGradeMap;
typedef StudentGradeMap::iterator StudentGradeMapIter;
typedef StudentGradeMap::const_iterator StudentGradeMapCIter;

typedef async::container::AssocVector< std::string, unsigned int > StudentGrades;
typedef StudentGrades::iterator StudentGradeIter;
typedef StudentGrades::const_iterator StudentGradeCIter;

typedef std::pair< ::std::string, unsigned int > GradeInfo;


GradeInfo oneStudent = ::std::make_pair( "Anne", 100 );

GradeInfo noDuplicates[] =
{
	::std::make_pair( "Anne", 100 ),
	::std::make_pair( "Bill",  88 ),
	::std::make_pair( "Clay",  91 ),
	::std::make_pair( "Dina",  62 ),
	::std::make_pair( "Evan",  77 ),
	::std::make_pair( "Fran",  84 ),
	::std::make_pair( "Greg",  95 )
};


GradeInfo hasDuplicates[] =
{
	std::make_pair( "Anne", 100 ),
	std::make_pair( "Anne",  73 ),
	std::make_pair( "Bill",  88 ),
	std::make_pair( "Clay",  91 ),
	std::make_pair( "Dina",  62 ),
	std::make_pair( "Evan",  77 ),
	std::make_pair( "Fran",  74 ),
	std::make_pair( "Fran",  84 ),
	std::make_pair( "Greg",  95 )
};

// ----------------------------------------------------------------------------

void TestEmptyAssocVector( void )
{
	std::cout << "Starting TestEmptyAssocVector" << std::endl;

	StudentGrades grades;
	const StudentGrades & cGrades = grades;
	assert( grades.empty() );
	assert( grades.size() == 0 );
	assert( cGrades.empty() );
	assert( cGrades.size() == 0 );

	StudentGradeIter it1( grades.begin() );
	assert( it1 == grades.end() );

	const StudentGradeIter it2( grades.begin() );
	assert( it2 == grades.end() );
	assert( it2 == it1 );
	assert( it1 == it2 );

	StudentGradeCIter cit1( grades.begin() );
	assert( cit1 == grades.end() );
	assert( cit1 == it1 );
	assert( cit1 == it2 );
	assert( it1 == cit1 );
	assert( it2 == cit1 );

	const StudentGradeCIter cit2( grades.begin() );
	assert( cit2 == grades.end() );
	assert( cit1 == cit2 );
	assert( cit2 == cit1 );
	assert( cit2 == it1 );
	assert( cit2 == it2 );
	assert( it1 == cit2 );
	assert( it2 == cit2 );

	StudentGradeCIter cit3( cGrades.begin() );
	assert( cit3 == cGrades.end() );
	assert( cit3 == it1 );
	assert( cit3 == it2 );
	assert( it1 == cit3 );
	assert( it2 == cit3 );

	const StudentGradeCIter cit4( cGrades.begin() );
	assert( cit4 == cGrades.end() );
	assert( cit1 == cit4 );
	assert( cit4 == cit1 );
	assert( cit4 == it1 );
	assert( cit4 == it2 );
	assert( it1 == cit4 );
	assert( it2 == cit4 );

	std::cout << "Finished TestEmptyAssocVector" << std::endl;
}

// ----------------------------------------------------------------------------

void TestAssocVectorCtor( void )
{
	std::cout << "Starting TestAssocVectorCtor" << std::endl;

	static const unsigned int  noDuplicateCount = ( sizeof(noDuplicates)  / sizeof(noDuplicates[0])  );
	static const unsigned int hasDuplicateCount = ( sizeof(hasDuplicates) / sizeof(hasDuplicates[0]) );

	{
		// This test demonstrates the iterator constructor does not allow any duplicate elements.
		StudentGrades grades1(  noDuplicates,  noDuplicates +  noDuplicateCount );
		StudentGrades grades2( hasDuplicates, hasDuplicates + hasDuplicateCount );
		assert( grades1.size() != 0 );
		assert( grades2.size() != 0 );
		assert( grades1.size() == noDuplicateCount );
		assert( grades2.size() == noDuplicateCount );
		assert( grades1.size() == grades2.size() );
	}

	{
		// This test demonstrates copy construction.
		StudentGrades grades1( noDuplicates, noDuplicates + noDuplicateCount );
		const StudentGrades grades2( grades1 );
		assert( grades1.size() != 0 );
		assert( grades2.size() != 0 );
		assert( grades1.size() == noDuplicateCount );
		assert( grades2.size() == noDuplicateCount );
		assert( grades1.size() == grades2.size() );
		assert( grades1 == grades2 );

		StudentGrades grades3;
		grades3 = grades1;
		assert( grades3.size() != 0 );
		assert( grades3.size() == noDuplicateCount );
		assert( grades3.size() == grades1.size() );
		assert( grades1 == grades3 );
	}

	std::cout << "Finished TestAssocVectorCtor" << std::endl;
}


void TestAsMap()
{
	static const unsigned int  noDuplicateCount = sizeof(noDuplicates)  / sizeof(noDuplicates[0]); 

	StudentGrades grades(  noDuplicates,  noDuplicates +  noDuplicateCount );

	assert(grades.find("Anne") != grades.end());

	StudentGrades::const_iterator iter = grades.find("Evan");
	assert(iter != grades.end());
}


namespace std
{
	template < typename CharT >
	basic_ostream<CharT, char_traits<CharT>> &operator<<(basic_ostream<CharT, char_traits<CharT>> &os, 
		const pair<string, string> &val)
	{
		os << "{" << val.first << "," << val.second << "}";
		return os;
	}
}


void TestFind()
{
	typedef async::container::AssocVector<std::string, std::string> Persons;

	Persons persons;
	persons.insert(std::make_pair("chenyu", "25"));
	persons.insert(std::make_pair("heyao", "24"));
	persons.insert(std::make_pair("ceshi", "2313"));
	persons.insert(std::make_pair("heyao", "24"));
	persons.insert(std::make_pair("123", "123"));

	Persons::iterator beg = persons.lower_bound("chenyu");
	Persons::iterator end = persons.upper_bound("heyao");

	
	std::cout << *beg << std::endl;
	persons.erase(beg, end);

	std::copy(persons.begin(), persons.end(), std::ostream_iterator<Persons::value_type>(std::cout, " "));
}