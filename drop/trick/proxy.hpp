/**
 * 1. 问题的引出:
 一个数组的数组int a[10][20]，其取用方式可以如下: a[2][6];
 一个二维数组类Array2D ar, ar[2][6]无效，无法重载[][]
 解决: proxy类
 */
template <typename T>
class array2d
{
public:
	class array1d // proxy class
 	{
	public:
		T& operator[](int index);
		T const& operator[](int index) const;
	};

	array1d operator[](int index);
	array1d const operator[](int index) const;
};
