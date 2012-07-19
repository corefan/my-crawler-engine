#ifndef __STATIC_SELECT_HPP
#define __STATIC_SELECT_HPP


namespace utility
{
	/************************************************************************
	*  
	*	Int2Type		: ������ӳ��Ϊ���ͣ��ɱ����ڼ�������Ľ��ѡ��ͬ�ĺ������ﵽ��̬����
	*	���÷�ʽ			: Int2Type<isPolymorphics>
	*	��������			: ����Ҫ����ĳ�������ڳ�������һ����������ͬ�ĺ���
	*					  �б�Ҫ�ڱ�����ʵʩ��̬����
	*
	*	Type2Type		: ���ú������ػ��ƣ�ģ��templateƫ�ػ��������������ͱ��������ͱ���Ϣ
	*
	***********************************************************************/
	template< int v >
	struct Int2Type
	{
		enum { value = v };
	};

	template< typename T >
	struct Type2Type
	{
		typedef T value_type;
	};
}





#endif