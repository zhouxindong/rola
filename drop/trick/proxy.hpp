/**
 * 1. ���������:
 һ�����������int a[10][20]����ȡ�÷�ʽ��������: a[2][6];
 һ����ά������Array2D ar, ar[2][6]��Ч���޷�����[][]
 ���: proxy��
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
