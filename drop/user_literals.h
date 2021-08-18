constexpr size_t operator "" _KB(unsigned long long const size)
{
	return static_cast<size_t>(size * 1024);
}

auto size{ 4_KB };

namespace units
{
	enum class unit { kilogram, liter, meter, piece, };

	template <unit U>
	class quantity
	{
		const double amount;

	public:
		constexpr explicit quantity(double const a)
			: amount(a) {}

		explicit operator double() const { return amount; }
	};

	template <unit U>
	constexpr quantity<U> operator+(quantity<U> const& q1, quantity<U> const& q2)
	{
		return quantity<U>(static_cast<double>(q1) + static_cast<double>(q2));
	}

	template <unit U>
	constexpr quantity<U> operator-(quantity<U> const& q1, quantity<U> const& q2)
	{
		return quantity<U>(static_cast<double>(q1) - static_cast<double>(q2));
	}

	namespace unit_literals
	{
		constexpr quantity<unit::kilogram> operator "" _kg(long double const amount)
		{
			return quantity<unit::kilogram>{static_cast<double>(amount)};
		}

		constexpr quantity<unit::kilogram> operator "" _kg(unsigned long long amount)
		{
			return quantity<unit::kilogram>{static_cast<double>(amount)};
		}

		constexpr quantity<unit::liter> operator "" _l(long double const amount)
		{
			return quantity<unit::liter>{static_cast<double>(amount)};
		}

		constexpr quantity<unit::meter> operator "" _m(long double const amount)
		{
			return quantity<unit::meter>{static_cast<double>(amount)};
		}

		constexpr quantity<unit::piece> operator "" _pcs(unsigned long long const amount)
		{
			return quantity<unit::piece>{static_cast<double>(amount)};
		}
	}
}

using namespace units;
using namespace unit_literals;

auto q1{ 1_kg };
auto q2{ 4.5_kg };
auto q3{ q1 + q2 };
