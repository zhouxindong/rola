#ifndef ROLA_BASE_MEMBER_DATA_HOLDER_HPP
#define ROLA_BASE_MEMBER_DATA_HOLDER_HPP

#define PRIVATE_DATA_CLASS(Class)  \
	inline Class##_private_data* p_data() { return reinterpret_cast<Class##_private_data*>(p_data_);} \
	inline const Class##_private_data* p_data() const { return reinterpret_cast<const Class##_private_data*>(p_data_); } \
	friend class Class##_private_data;

namespace rola
{
	// base class for private members data
	// place all commons members data in here
	// can't be instanced directly, only as a subclass object
	class private_data_base
	{
	protected:
		private_data_base() :
			Version{ 1 }
		{}

	public:
		int Version;

	protected:
		virtual ~private_data_base() = 0;
	};

	inline private_data_base::~private_data_base()
	{}

	// the private data for pimpl_base
	class pimpl_base_private_data : public private_data_base
	{
	};
	
	// base class for those apply pimpl mode
	class pimpl_base
	{
		PRIVATE_DATA_CLASS(pimpl_base) // required pimpl_base_private_data class defined

	public:
		pimpl_base()
			: p_data_(new pimpl_base_private_data)
		{}
			
	protected:
		// used for derived class ctor
		pimpl_base(pimpl_base_private_data& dd)
			: p_data_(&dd)
		{}

	protected:
		private_data_base* p_data_;
	};

} // namespace rola

#endif // !ROLA_BASE_MEMBER_DATA_HOLDER_HPP
