#ifndef ROLA_MPL_TUPLE2_HPP
#define ROLA_MPL_TUPLE2_HPP

namespace rola
{
	namespace mpl
	{
		// EBCO
		template <typename ...Types>
		class Tuple2;

		// storage: tail as a subclass object
		// side effect: Tail first init before head, and storage is reversed from tail to head!
		template <typename Head, typename ...Tail>
		class Tuple2<Head, Tail...> : private Tuple2<Tail...>
		{
		private:
			Head head_;

		public:
			Head& get_head() { return head_; }
			Head const& get_head() const { return head_; }
			Tuple2<Tail...>& get_tail() { return *this; }
			Tuple2<Tail...> const& get_tail() const { return *this; }
		};
	} // namespace mpl
} // namespace rola

#endif // !ROLA_MPL_TUPLE2_HPP
