#ifndef __SERIALIZE_CONTAINER_DISPATCH_HPP
#define __SERIALIZE_CONTAINER_DISPATCH_HPP



#include <string>
#include <map>
#include <set>
#include <list>
#include <vector>
#include <stack>
#include <queue>
#include <deque>


namespace utility
{
	namespace serialize
	{
		// forward declare
		template < typename CharT, typename OutT >
		class SerializeT;


		namespace detail
		{


			/*template < typename ContainerT >
			struct IsContainer
			{
				enum { value = false };
			};*/




			// -----------------------------------------------------------------
			// class ValueTraits

			template< typename ValTraitsT >
			class ValueTrait
			{
			public:
				typedef ValTraitsT								ValTraitsType;

			public:
				template < typename CharT, typename OutT >
				static void PushDispatch(SerializeT<CharT, OutT> &io, const ValTraitsType& val)
				{
					io << val;
				}

				template < typename CharT, typename OutT >
				static void PopDispatch(SerializeT<CharT, OutT> &io, ValTraitsType &val)
				{
					io >> val;
				}
			};



			template< typename FirstT, typename SecondT >
			class ValueTrait<std::pair<FirstT, SecondT>>
			{
			public:
				typedef std::pair<FirstT, SecondT>	PairTraitsType;

			public:
				template < typename CharT, typename OutT >
				static void PushDispatch(SerializeT<CharT, OutT> &io, const PairTraitsType& val)
				{
					ValueTrait<PairTraitsType::first_type>::PushDispatch(io, val.first);
					ValueTrait<PairTraitsType::second_type>::PushDispatch(io, val.second);
				}

				template < typename CharT, typename OutT >
				static void PopDispatch(SerializeT<CharT, OutT> &io, PairTraitsType &val)
				{
					ValueTrait<PairTraitsType::first_type>::PopDispatch(io, val.first);
					ValueTrait<PairTraitsType::second_type>::PopDispatch(io, val.second);
				}
			};


			// 关联容器 -- map, multimap, set, multiset

			// -----------------------------------------------------------------
			// class AssociativeTypeTrait

			template< typename AssocTypeT >
			class AssociativeTypeTrait
			{
			public:
				typedef typename AssocTypeT::size_type		size_type;
				typedef typename AssocTypeT::key_type		key_type;
				typedef typename AssocTypeT::mapped_type	mapped_type;

			public:
				template < typename CharT, typename OutT >
				static void PushDispatch(SerializeT<CharT, OutT> &io, const AssocTypeT& val)
				{
					ValueTrait<size_type>::PushDispatch(io, val.size());

					for(typename AssocTypeT::const_iterator iter = val.begin(); 
						iter != val.end(); ++iter)
					{
						ValueTrait<key_type>::PushDispatch(io, iter->first);
						ValueTrait<mapped_type>::PushDispatch(io, iter->second);
					}
				}

				template < typename CharT, typename OutT >
				static void PopDispatch(SerializeT<CharT, OutT> &io, AssocTypeT &val)
				{
					size_type valSize = 0;
					ValueTrait<size_type>::PopDispatch(io, valSize);

					for(; valSize > 0; --valSize)
					{
						key_type key;
						ValueTrait<key_type>::PopDispatch(io, key);

						mapped_type value;
						ValueTrait<mapped_type>::PopDispatch(io, value);

						val.insert(std::make_pair(key, value));
					}
				}
			};


			// 线性容器--list, vector, deque

			// -----------------------------------------------------------------
			// class SequceTrait

			template< typename SequenceTypeT >
			class SequenceTrait
			{
			public:
				typedef typename SequenceTypeT::size_type	size_type;	
				typedef typename SequenceTypeT::value_type	value_type;

			public:
				template < typename CharT, typename OutT >
				static void PushDispatch(SerializeT<CharT, OutT> &io, const SequenceTypeT &val)
				{
					ValueTrait<size_type>::PushDispatch(io, val.size());

					for(SequenceTypeT::const_iterator iter = val.begin(); 
						iter != val.end(); ++iter)
					{
						ValueTrait<value_type>::PushDispatch(io, *iter);
					}
				}

				template < typename CharT, typename OutT >
				static void PopDispatch(SerializeT<CharT, OutT> &io, SequenceTypeT& val)
				{
					size_type valSize = 0;
					ValueTrait<size_type>::PopDispatch(io, valSize);


					for(; valSize > 0; --valSize)
					{
						value_type element;
						ValueTrait<value_type>::PopDispatch(io, element);

						val.push_back(element);
					}
				}
			};


			// STL map datatype serializer class.

			template< typename KeyT, typename DataT, typename LessT, typename AllocT > 
			class ValueTrait<std::map<KeyT, DataT, LessT, AllocT>> 
			{
			public:
				typedef std::map<KeyT, DataT, LessT, AllocT>	MapTraitType;

			public:
				template < typename CharT, typename OutT >
				static void PushDispatch(SerializeT<CharT, OutT> &io, const MapTraitType &val)
				{
					AssociativeTypeTrait<MapTraitType>::PushDispatch(io, val);
				}

				template < typename CharT, typename OutT >
				static void PopDispatch(SerializeT<CharT, OutT> &io, MapTraitType& val)
				{
					AssociativeTypeTrait<MapTraitType>::PopDispatch(io, val);
				}
			};


			// STL multimap datatype serializer class.

			template< typename KeyT, typename DataT, typename LessT, typename AllocT > 
			class ValueTrait<std::multimap<KeyT, DataT, LessT, AllocT>> 
			{
			public:
				typedef std::multimap<KeyT, DataT, LessT, AllocT> MultimapTraitType;


			public:
				template < typename CharT, typename OutT >
				static void PushDispatch(SerializeT<CharT, OutT> &io, const MultimapTraitType &val)
				{
					AssociativeTypeTrait<MultimapTraitType>::PushDispatch(io, val);
				}

				template < typename CharT, typename OutT >
				static void PopDispatch(SerializeT<CharT, OutT> &io, MultimapTraitType& val)
				{
					AssociativeTypeTrait<MultimapTraitType>::PopDispatch(io, val);
				}
			};



			// STL set datatype serializer class.

			template< typename KeyT, typename LessT, typename AllocT > 
			class ValueTrait<std::set<KeyT, LessT, AllocT>> 
			{
			public:
				typedef std::set<KeyT, LessT, AllocT>		SetTratiType;
				typedef typename SetTratiType::size_type	size_type;
				typedef typename SetTratiType::value_type	value_type;

			public:
				template < typename CharT, typename OutT >
				static void PushDispatch(SerializeT<CharT, OutT> &io, const SetTratiType& val)
				{
					ValueTrait<size_type>::PushDispatch(io, val.size());

					for(typename SetTratiType::const_iterator iter = val.begin(); 
						iter != val.end(); ++iter)
					{
						ValueTrait<value_type>::PushDispatch(io, *iter);
					}
				}

				template < typename CharT, typename OutT >
				static void PopDispatch(SerializeT<CharT, OutT> &io, SetTratiType &val)
				{
					size_type valSize = 0;
					ValueTrait<size_type>::PopDispatch(io, valSize);

					for(; valSize > 0; --valSize)
					{
						value_type key;
						ValueTrait<value_type>::PopDispatch(io, key);

						val.insert(key);
					}
				}
			};

			// STL multiset datatype serializer class.

			template<typename KeyT, typename LessT, typename AllocT > 
			class ValueTrait<std::multiset<KeyT, LessT, AllocT>> 
			{
			public:
				typedef std::multiset<KeyT, LessT, AllocT>		MultisetTraitType;
				typedef typename MultisetTraitType::size_type	size_type;
				typedef typename MultisetTraitType::value_type	value_type;

			public:
				template < typename CharT, typename OutT >
				static void PushDispatch(SerializeT<CharT, OutT> &io, const MultisetTraitType& val)
				{
					ValueTrait<size_type>::PushDispatch(io, val.size());

					for(typename MultisetTraitType::const_iterator iter = val.begin(); 
						iter != val.end(); ++iter)
					{
						ValueTrait<value_type>::PushDispatch(io, *iter);
					}
				}

				template < typename CharT, typename OutT >
				static void PopDispatch(SerializeT<CharT, OutT> &io, MultisetTraitType &val)
				{
					size_type valSize = 0;
					ValueTrait<size_type>::PopDispatch(io, valSize);

					for(; valSize > 0; --valSize)
					{
						value_type key;
						ValueTrait<value_type>::PopDispatch(io, key);

						val.insert(key);
					}
				}
			};


			// STL list datatype serializer class.

			template< typename KeyT, typename AllocT > 
			class ValueTrait<std::list<KeyT, AllocT>>
			{
			public:
				typedef std::list<KeyT, AllocT> ListTraitType;

			public:
				template < typename CharT, typename OutT >
				static void PushDispatch(SerializeT<CharT, OutT> &io, const ListTraitType &val)
				{
					SequenceTrait<ListTraitType>::PushDispatch(io, val);
				}

				template < typename CharT, typename OutT >
				static void PopDispatch(SerializeT<CharT, OutT> &io, ListTraitType &val)
				{
					SequenceTrait<ListTraitType>::PopDispatch(io, val);
				}
			};

			//STL vector datatype serializer class.

			template< typename KeyT, typename AllocT > 
			class ValueTrait<std::vector<KeyT, AllocT>>
			{
			public:
				typedef std::vector<KeyT, AllocT> VectorTraitType;


			public:
				template < typename CharT, typename OutT >
				static void PushDispatch(SerializeT<CharT, OutT> &io, const VectorTraitType &val)
				{
					SequenceTrait<VectorTraitType>::PushDispatch(io, val);
				}

				template < typename CharT, typename OutT >
				static void PopDispatch(SerializeT<CharT, OutT> &io, VectorTraitType &val)
				{
					SequenceTrait<VectorTraitType>::PopDispatch(io, val);
				}
			};


			// STL deque datatype serializer class.

			template< typename KeyT, typename AllocT >
			class ValueTrait<std::deque<KeyT, AllocT>>
			{
			public:
				typedef std::deque<KeyT, AllocT> DequeTraitType;


			public:
				template < typename CharT, typename OutT >
				static void PushDispatch(SerializeT<CharT, OutT> &io, const DequeTraitType &val)
				{
					SequenceTrait<DequeTraitType>::PushDispatch(io, val);
				}

				template < typename CharT, typename OutT >
				static void PopDispatch(SerializeT<CharT, OutT> &io, DequeTraitType &val)
				{
					SequenceTrait<DequeTraitType>::PopDispatch(io, val);
				}
			};

			template< typename ValueT, typename ContainerT >
			class ValueTrait<std::queue<ValueT, ContainerT>>
			{
			public:
				typedef std::queue<ValueT, ContainerT> QueueTraitType;

			public:
				template < typename CharT, typename OutT >
				static void PushDispatch(SerializeT<CharT, OutT> &io, const QueueTraitType &val)
				{
					static_assert(false);
				}

				template < typename CharT, typename OutT >
				static void PopDispatch(SerializeT<CharT, OutT> &io, QueueTraitType &val)
				{
					static_assert(false);
				}
			};

			template< typename ValueT, typename ContainerT >
			class ValueTrait<std::stack<ValueT, ContainerT>>
			{
			public:
				typedef std::stack<ValueT, ContainerT> StackTraitType;

			public:
				template < typename CharT, typename OutT >
				static void PushDispatch(SerializeT<CharT, OutT> &io, const StackTraitType &val)
				{
					static_assert(false);
				}

				template < typename CharT, typename OutT >
				static void PopDispatch(SerializeT<CharT, OutT> &io, StackTraitType &val)
				{
					static_assert(false);
				}
			};

			template< typename ValueT, typename ContainerT, typename LessT >
			class ValueTrait<std::priority_queue<ValueT, ContainerT, LessT>>
			{
			public:
				typedef std::priority_queue<ValueT, ContainerT, LessT> PriorityQueueTraitType;

			public:
				template < typename CharT, typename OutT >
				static void PushDispatch(SerializeT<CharT, OutT> &io, const PriorityQueueTraitType &val)
				{
					static_assert(false);
				}

				template < typename CharT, typename OutT >
				static void PopDispatch(SerializeT<CharT, OutT> &io, PriorityQueueTraitType &val)
				{
					static_assert(false);
				}
			};
		}
	}
}






#endif