namespace async {
	namespace detail {

		// Allocate an aligned block of memory
		LIBASYNC_EXPORT void* aligned_alloc(std::size_t size, std::size_t align);

		// Free an aligned block of memory
		LIBASYNC_EXPORT void aligned_free(void* addr) LIBASYNC_NOEXCEPT;

		// Class representing an aligned array and its length
		template<typename T, std::size_t Align = std::alignment_of<T>::value>
		class aligned_array {
			std::size_t length;
			T* ptr;

		public:
			aligned_array()
				: length(0), ptr(nullptr) {}
			aligned_array(std::nullptr_t)
				: length(0), ptr(nullptr) {}
			explicit aligned_array(std::size_t length)
				: length(length)
			{
				ptr = static_cast<T*>(aligned_alloc(length * sizeof(T), Align));
				std::size_t i;
				LIBASYNC_TRY{
					for (i = 0; i < length; i++)
						new(ptr + i) T;
				} LIBASYNC_CATCH(...) {
					for (std::size_t j = 0; j < i; j++)
						ptr[i].~T();
					aligned_free(ptr);
					LIBASYNC_RETHROW();
				}
			}
			aligned_array(aligned_array&& other) LIBASYNC_NOEXCEPT
				: length(other.length), ptr(other.ptr)
			{
				other.ptr = nullptr;
				other.length = 0;
			}
			aligned_array& operator=(aligned_array&& other) LIBASYNC_NOEXCEPT
			{
				aligned_array(std::move(*this));
				std::swap(ptr, other.ptr);
				std::swap(length, other.length);
				return *this;
			}
			aligned_array& operator=(std::nullptr_t)
			{
				return *this = aligned_array();
			}
			~aligned_array()
			{
				for (std::size_t i = 0; i < length; i++)
					ptr[i].~T();
				aligned_free(ptr);
			}

			T& operator[](std::size_t i) const
			{
				return ptr[i];
			}
			std::size_t size() const
			{
				return length;
			}
			T* get() const
			{
				return ptr;
			}
			explicit operator bool() const
			{
				return ptr != nullptr;
			}
		};

	} // namespace detail
} // namespace async
