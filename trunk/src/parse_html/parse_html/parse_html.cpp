// parse_html.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <string>
#include <list>
#include <fstream>
#include <iostream>
#include <regex>
#include <stack>
#include <queue>
#include <array>

#include <Extend STL\StringAlgorithm.h>

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"


int CompareDinosByChineseLength(const std::string &x, const std::string &y)
{
	if( x.empty() ||
		y.empty() )
		return 0;

	std::regex reg("[\u4e00-\u9fa5]");
	std::smatch result_x, result_y;

	std::regex_search(x, result_x, reg);
	std::regex_search(y, result_y, reg);

	size_t xCount = result_x.size() / x.length();
	size_t yCount = result_y.size() / y.length();

	return xCount < yCount;
}


template < typename CharT >
bool compare(const std::basic_string<CharT> &lhs, const std::basic_string<CharT> &rhs)
{
	return std::equal(lhs.begin(), lhs.end(), rhs.begin(), [](const CharT l, const CharT r)->bool
	{
		return ::toupper(l) == ::toupper(r);
	});
}



std::list<std::string> parse(const std::string &text, const std::string &tag)
{
	std::list<std::string> vals;

	std::stack<size_t> start_stack;
	size_t pos = 0;
	size_t index = 0;

	const char *start = text.c_str();
	const char *val = text.c_str();
	while( *val != '\0' )
	{
		++val;

		if( *val != '<' )
			continue;

		++val;
		std::string tmp = text.substr(val - start, tag.length());
		if( !compare(tmp, tag) )
			continue;

		pos = val - start - 1;

		while(1)
		{
			while( *val++ != '>' )
				;

			while( *val++ != '<' )
				;

			if( *val != '/' )
			{
				tmp = text.substr(val - start, tag.length());

				if( compare(tmp, tag) )
					start_stack.push(val - start);
			}
			else
			{
				++val;
				tmp = text.substr(val - start, tag.length());

				if( !compare(tmp, tag) )
					continue;

				if( !start_stack.empty() )
				{
					start_stack.pop();
					continue;
				}
				else
				{
					while( *val++ != '>' )
						;

					std::string content = text.substr(pos, (val - start) - pos);
					vals.push_front(content);
					break;
				}

			}


		}
	}


	return vals;
}


std::string GetMainContent(const std::string &input)
{
	std::string reg1 = "<(p|br)[^<]*>";
	std::string reg2 = "(\\[([^=]*)(=[^\\]]*)?\\][\\s\\S]*?\\[/\\1\\])|(?<lj>(?=[^\\u4E00-\\u9FA5\\uFE30-\\uFFA0,."");])<a\\s+[^>]*>[^<]{2,}</a>(?=[^\\u4E00-\\u9FA5\\uFE30-\\uFFA0,."");]))|(?<Style><style[\\s\\S]+?/style>)|(?<select><select[\\s\\S]+?/select>)|(?<Script><script[\\s\\S]*?/script>)|(?<Explein><\\!\\-\\-[\\s\\S]*?\\-\\->)|(?<li><li(\\s+[^>]+)?>[\\s\\S]*?/li>)|(?<Html></?\\s*[^> ]+(\\s*[^=>]+?=['""]?[^""']+?['""]?)*?[^\\[<]*>)|(?<Other>&[a-zA-Z]+;)|(?<Other2>\\#[a-z0-9]{6})|(?<Space>\\s+)|(\\&\\#\\d+\\;)";

	//1、获取网页的所有div标签
	std::list<std::string> list = parse(input, "DIV");

	//2、去除汉字少于200字的div
	std::list<std::string> needToRemove;
	for(auto iter = list.begin(); iter != list.end(); ++iter)
	{
		std::regex r("[\u4e00-\u9fa5]");
		std::smatch result;

		bool suc = std::regex_search(*iter, result, r);
		if( suc && result.size() < 200 )
		{
			needToRemove.push_back(*iter);
		}
	}


	std::list<std::string> tmp(list.size() - needToRemove.size());

	//3、把剩下的div按汉字比例多少倒序排列,
	list.sort(CompareDinosByChineseLength);
	if (list.empty())
	{
		return "";
	}

	std::string content = *list.rbegin();


	//5、去掉HTML标签，保留汉字
	std::regex rrr("\\<(.[^>]*)>", std::regex::icase);
	content = std::regex_replace(content, rrr, std::string(""));

	return content;
}



int _tmain(int argc, _TCHAR* argv[])
{

	std::ifstream in("1.htm");

	std::string val;
	in.seekg(0, std::ios_base::end);
	size_t len = in.tellg();
	in.seekg(0, std::ios_base::beg);

	val.resize(len + 1);
	in.read(&val[0], len);

	std::string content = GetMainContent(val);

	/*rapidxml::file<> file("1.htm");
	rapidxml::xml_document<> doc;
	doc.parse<rapidxml::parse_no_string_terminators>(file.data());*/


	//GetMainContent(val);

	system("pause");
	return 0;
}

