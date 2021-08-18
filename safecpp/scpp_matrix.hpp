#ifndef ROLA_SAFECPP_SCPP_MATRIX_HPP
#define ROLA_SAFECPP_SCPP_MATRIX_HPP

#include <ostream>
#include <vector>

#include "scpp_assert.hpp"

namespace rola
{
	template <typename T>
	class matrix
	{
	public:
		typedef unsigned size_type;

	private:
		class axis_x_proxy
		{
			friend class matrix;
			matrix& matrix_;
			size_type rows_;

		public:
			axis_x_proxy(matrix& m, size_type rows)
				: matrix_{m}
				, rows_{rows}
			{}

			T& operator[](int cols)
			{
				return matrix_.data_[matrix_.index(rows_, cols)];
			}

			const T& operator[](int cols) const
			{
				return matrix_.data_[matrix_.index(rows_, cols)];
			}
		};

	public:
		matrix(size_type num_rows, size_type num_cols)
			: rows_(num_rows)
			, cols_(num_cols)
			, data_(num_rows* num_cols)
		{
			SCPP_ASSERT(num_rows > 0,
				"Number of rows in a matrix must be positive");
			SCPP_ASSERT(num_cols > 0,
				"Number of columns in a matrix must be positive");
		}

		matrix(size_type num_rows, size_type num_cols, const T& init_value)
			: rows_(num_rows)
			, cols_(num_cols)
			, data_(num_rows* num_cols, init_value)
		{
			SCPP_ASSERT(num_rows > 0,
				"Number of rows in a matrix must be positive");
			SCPP_ASSERT(num_cols > 0,
				"Number of columns in a matrix must be positive");
		}

		size_type num_rows() const
		{
			return rows_;
		}

		size_type num_cols() const
		{
			return cols_;
		}

		T& operator()(size_type row, size_type col)
		{
			return data_[index(row, col)];
		}

		const T& operator()(size_type row, size_type col) const
		{
			return data_[index(row, col)];
		}

		axis_x_proxy operator[](size_type rows)
		{
			return axis_x_proxy{ *this, rows };
		}

		axis_x_proxy const operator[](size_type rows) const
		{
			return axis_x_proxy{ *this, rows };
		}

	private:
		size_type rows_, cols_;
		std::vector<T> data_;

		size_type index(size_type row, size_type col) const
		{
			SCPP_ASSERT(row < rows_,
				"Row " << row << " must be less than " << rows_);
			SCPP_ASSERT(col < cols_,
				"Column " << col << " must be less than " << cols_);
			return cols_ * row + col;
		}
	};

	template <typename T>
	inline
		std::ostream& operator << (std::ostream& os, const rola::matrix<T>& m) {
		for (unsigned r = 0; r < m.num_rows(); ++r) {
			for (unsigned c = 0; c < m.num_cols(); ++c) {
				os << m(r, c);
				if (c + 1 < m.num_cols())
					os << "\t";
			}
			os << "\n";
		}
		return os;
	}
} // namespace rola

#endif // ROLA_SAFECPP_SCPP_MATRIX_HPP
