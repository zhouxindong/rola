#ifndef ROLA_MPL_VARIANT_HPP
#define ROLA_MPL_VARIANT_HPP

//#include <new> // std::launder()

#include "type_list.hpp"

namespace rola
{
	namespace mpl
	{
#pragma region Variant_storage

		template <typename ...Types>
		class Variant_storage
		{
			using LargestT = rola::TL::Largest_type_t<rola::Type_list<Types...>>;
			alignas(Types...) unsigned char buffer_[sizeof(LargestT)];
			unsigned char discriminator_ = 0;

		public:
			unsigned char get_discriminator() const
			{
				return discriminator_;
			}

			void set_discriminator(unsigned char d)
			{
				discriminator_ = d;
			}

			void* get_raw_buffer()
			{
				return buffer_;
			}

			const void* get_raw_buffer() const
			{
				return buffer_;
			}

			template <typename T>
			T* get_buffer_as()
			{
				return reinterpret_cast<T*>(buffer);
			}

			template <typename T>
			T const* get_buffer_as() const
			{
				return reinterpret_cast<T const*>(buffer);
			}
		};

#pragma endregion

		template <typename... Types>
		class Variant;

#pragma region Variant_choice

		template <typename T, typename... Types>
		class Variant_choice
		{
			using Derived = Variant<Types...>; // CRTP

			Derived& get_derived()
			{
				return *static_cast<Derived*>(this);
			}

			Derived const& get_derived() const
			{
				return *static_cast<Derived const*>(this);
			}

		protected:
			// compute the discriminator to be used for this type
			constexpr static unsigned Discriminator = rola::TL::Index_of_v<rola::Type_list<Types...>, T> +1;

		public:
			Variant_choice() {}
			Variant_choice(T const& value);
			Variant_choice(T&& value);

			bool destroy();
			Derived& operator=(T const& value);
			Derived& operator=(T&& value);
		};

#pragma endregion

#pragma region Variant

		template <typename... Types>
		class Variant 
			: private Variant_storage<Types...>
			, private Variant_choice<Types, Types...>...

		{
			template <typename T, typename... OtherTypes>
			friend class VariantChoice;

		public:
			template <typename T>
			bool is() const;

			template <typename T>
			T& get()&;

			template <typename T>
			T const& get() const&;

			template <typename T>
			T&& get()&&;
		};

#pragma endregion
	} // namespace mpl
} // namespace rola
#endif // !ROLA_MPL_VARIANT_HPP
