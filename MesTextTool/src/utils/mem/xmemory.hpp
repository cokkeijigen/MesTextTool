#pragma once
#define _xmemory_
#include <vector>
#include <span>

namespace utils::xmem {

	template <class T, class elem_t>
	concept valid_iterator_t = requires(T it) 
	{
		{ ++it } -> std::same_as<T&>;
		{ it++ } -> std::convertible_to<T>;
		{ it - it } -> std::convertible_to<size_t>;
		typename std::enable_if<sizeof(decltype(*it)) == sizeof(elem_t)>::type;
	};

	template<class T, class elem_t>
	concept iterable_t = requires(T val)
	{
		{ val.begin() } -> std::convertible_to<class T::iterator>;
		{ val.end  () } -> std::convertible_to<class T::iterator>;
		typename std::enable_if<xmem::valid_iterator_t<class T::iterator, elem_t>>::type;
	};

	template<class elem_t>
	class buffer
	{
	protected:

		std::vector<elem_t> m_Buffer{};
		size_t m_Count{};

		auto check(const size_t length) -> buffer&;

	public:

		static constexpr inline auto unused{ static_cast<size_t>(-1) };
		using iterator = typename std::vector<elem_t>::iterator;

		buffer() = default;
		~buffer() = default;
		
		buffer(size_t size);
		buffer(const buffer& other);
		buffer(buffer&& other) noexcept;

		auto write(elem_t elem) -> buffer&;

		template<class T>
		requires (sizeof(T) == sizeof(elem_t))
		auto write(T elem) -> buffer&;

		template<class T>
		requires (sizeof(T) == sizeof(elem_t))
		auto write(const T* data, size_t count) -> buffer&;

		template<class T, size_t N>
		requires (sizeof(T) == sizeof(elem_t))
		auto write(const T (&data)[N], size_t count = unused) -> buffer&;

		template<size_t N>
		auto write(const elem_t (&data)[N], size_t count = unused) -> buffer&;

		template<class T>
		requires xmem::iterable_t<T, elem_t>
		auto write(const T&& data, size_t count = unused) -> buffer&;

		template<class T>
		requires xmem::iterable_t<T, elem_t>
		auto write(const T& data, size_t count = unused) -> buffer&;

		template <class iterator_t>
		requires xmem::valid_iterator_t<iterator_t, elem_t>
		auto write(iterator_t first, size_t count) -> buffer&;

		template <class iterator_t>
		requires xmem::valid_iterator_t<iterator_t, elem_t>
		auto write(iterator_t begin, iterator_t end) -> buffer&;

		template<class T>
		requires (sizeof(T) == sizeof(elem_t))
		auto write(size_t offset, T elem, bool overwrite = true) -> buffer&;
		
		auto write(size_t offset, elem_t elem, bool overwrite = true) -> buffer&;

		template<class T>
		requires (sizeof(T) == sizeof(elem_t))
		auto write(size_t offset, const T* elem, size_t count, bool overwrite = true) -> buffer&;

		auto write(size_t offset, const elem_t* elem, size_t count, bool overwrite = true) -> buffer&;

		template<class T, size_t N>
		requires (sizeof(T) == sizeof(elem_t))
		auto write(size_t offset, const T (&data)[N], size_t count, bool overwrite = true) -> buffer&;

		template<size_t N>
		auto write(size_t offset, const elem_t (&data)[N], size_t count, bool overwrite = true) -> buffer&;

		template<class T>
		requires xmem::iterable_t<T, elem_t>
		auto write(size_t offset, const T&& data, size_t count, bool overwrite = true) -> buffer&;

		template<class T>
		requires xmem::iterable_t<T, elem_t>
		auto write(size_t offset, const T& data, size_t count, bool overwrite = true) -> buffer&;

		template <class iterator_t>
		requires xmem::valid_iterator_t<iterator_t, elem_t>
		auto write(size_t offset, iterator_t first, size_t count, bool overwrite = true) -> buffer&;

		template<class T, size_t N>
		requires (sizeof(T) == sizeof(elem_t))
		auto write(size_t offset, const T (&data)[N], bool overwrite = true) -> buffer&;

		template<size_t N>
		auto write(size_t offset, const elem_t (&data)[N], bool overwrite = true) -> buffer&;

		template<class T>
		requires xmem::iterable_t<T, elem_t>
		auto write(size_t offset, const T&& data, bool overwrite = true) -> buffer&;

		template<class T>
		requires xmem::iterable_t<T, elem_t>
		auto write(size_t offset, const T& data, bool overwrite = true) -> buffer&;

		template <class iterator_t>
		requires xmem::valid_iterator_t<iterator_t, elem_t>
		auto write(size_t offset, iterator_t begin, iterator_t end, bool overwrite = true) -> buffer&;
		
		template<class T>
		requires (sizeof(T) == sizeof(elem_t))
		auto memset(size_t offset, T value, size_t count, bool overwrite = true) -> buffer&;

		auto memset(size_t offset, elem_t value, size_t count, bool overwrite = true) -> buffer&;

		auto written() -> std::span<elem_t>;

		auto recount(size_t count = 0) -> buffer&;

		auto resize(size_t size) -> buffer&;

		auto count() const -> size_t;

		auto size() const -> size_t;

		auto clear() -> buffer&;

		auto begin() -> iterator;

		auto end() -> iterator;

		auto data() -> elem_t*;

		auto copy() -> buffer;

		auto operator=(const buffer& other) -> buffer&;

		auto operator=(buffer&& other) noexcept -> buffer&;

		auto operator[](size_t index) -> elem_t;
	};

	template<class elem_t>
	inline buffer<elem_t>::buffer(size_t size)
	{
		this->m_Buffer.resize(size);
	}

	template<class elem_t>
	inline buffer<elem_t>::buffer(const buffer& other): m_Buffer(other.m_Buffer), m_Count(other.m_Count)
	{
	}

	template<class elem_t>
	inline buffer<elem_t>::buffer(buffer&& other) noexcept : m_Buffer(std::move(other.m_Buffer)), m_Count(other.m_Count)
	{
		other.m_Count = 0;
	}

	template<class elem_t>
	inline auto buffer<elem_t>::check(const size_t length) -> buffer&
	{

		size_t count{ this->m_Count + length };
		if (count > this->m_Buffer.size())
		{
			size_t size{ (count + 1023) & ~1023 };
			this->m_Buffer.resize(size);
		}
		return *this;
	}

	template<class elem_t>
	inline auto buffer<elem_t>::write(elem_t elem) -> buffer&
	{
		return this->write(std::span<elem_t>{ &elem, 1});
	}

	template<class elem_t>
	template<class T>
	requires (sizeof(T) == sizeof(elem_t))
	inline auto buffer<elem_t>::write(T elem) -> buffer&
	{
		return this->write(std::span<T>{ &elem, 1});
	}

	template<class elem_t>
	template<class T>
	requires (sizeof(T) == sizeof(elem_t))
	inline auto buffer<elem_t>::write(const T* data, size_t count) -> buffer&
	{
		return this->write(std::span<const T>{ data, count });
	}

	template<class elem_t>
	template<class T, size_t N>
	requires (sizeof(T) == sizeof(elem_t))
	inline auto buffer<elem_t>::write(const T(&data)[N], size_t count) -> buffer&
	{
		return this->write(std::span<T>{ const_cast<T*>(data), N }, count);
	}

	template<class elem_t>
	template<size_t N>
	inline auto buffer<elem_t>::write(const elem_t(&data)[N], size_t count) -> buffer&
	{
		return this->write(std::span<elem_t>{ const_cast<elem_t*>(data), N }, count);
	}

	template<class elem_t>
	template<class T>
	requires xmem::iterable_t<T, elem_t>
	inline auto buffer<elem_t>::write(const T&& data, size_t count) -> buffer&
	{
		return this->write(data, count);
	}

	template<class elem_t>
	template<class T>
	requires xmem::iterable_t<T, elem_t>
	inline auto buffer<elem_t>::write(const T& data, size_t count) -> buffer&
	{
		return this->write(this->m_Count, data, count);
	}

	template<class elem_t>
	template<class iterator_t>
	requires xmem::valid_iterator_t<iterator_t, elem_t>
	inline auto buffer<elem_t>::write(iterator_t first, size_t count) -> buffer&
	{
		return this->write(std::span<elem_t>{ first, first + count });
	}

	template<class elem_t>
	template<class iterator_t>
	requires xmem::valid_iterator_t<iterator_t, elem_t>
	inline auto buffer<elem_t>::write(iterator_t begin, iterator_t end) -> buffer&
	{
		return this->write(this->m_Count, begin, end);
	}

	template<class elem_t>
	template<class T>
	requires (sizeof(T) == sizeof(elem_t))
	inline auto buffer<elem_t>::write(size_t offset, T elem, bool overwrite) -> buffer&
	{
		return this->write(offset, std::span<T>{ &elem, 1 }, overwrite);
	}

	template<class elem_t>
	inline auto buffer<elem_t>::write(size_t offset, elem_t elem, bool overwrite) -> buffer&
	{
		return this->write(offset, std::span<elem_t>{ &elem, 1 }, overwrite);
	}

	template<class elem_t>
	template<class T>
	requires (sizeof(T) == sizeof(elem_t))
	inline auto buffer<elem_t>::write(size_t offset, const T* elem, size_t count, bool overwrite) -> buffer&
	{
		return this->write(offset, std::span<T>{ const_cast<T*>(elem), count }, overwrite);
	}

	template<class elem_t>
	inline auto buffer<elem_t>::write(size_t offset, const elem_t* elem, size_t count, bool overwrite) -> buffer&
	{
		return this->write(offset, std::span<elem_t>{ const_cast<elem_t*>(elem), count }, overwrite);
	}

	template<class elem_t>
	template<class T, size_t N>
	requires (sizeof(T) == sizeof(elem_t))
	inline auto buffer<elem_t>::write(size_t offset, const T (&data)[N], size_t count, bool overwrite) -> buffer&
	{
		return this->write(offset, std::span<T>{ const_cast<T*>(data), N }, count, overwrite);
	}

	template<class elem_t>
	template<size_t N>
	inline auto buffer<elem_t>::write(size_t offset, const elem_t(&data)[N], size_t count, bool overwrite) -> buffer&
	{
		return this->write(offset, std::span<elem_t>{ const_cast<elem_t*>(data), N }, count, overwrite);
	}

	template<class elem_t>
	template<class T>
	requires xmem::iterable_t<T, elem_t>
	inline auto buffer<elem_t>::write(size_t offset, const T&& data, size_t count, bool overwrite) -> buffer&
	{
		return this->write(offset, data, count, overwrite);
	}

	template<class elem_t>
	template<class T>
	requires xmem::iterable_t<T, elem_t>
	inline auto buffer<elem_t>::write(size_t offset, const T& data, size_t count, bool overwrite) -> buffer&
	{
		auto size{ static_cast<size_t>(data.end() - data.begin()) };
		if (count > size)
		{
			return this->write(offset, data.begin(), data.end(), overwrite);
		}
		else {
			return this->write(offset, data.begin(), data.begin() + count, overwrite);
		}
		return *this;
	}

	template<class elem_t>
	template<class iterator_t>
	requires xmem::valid_iterator_t<iterator_t, elem_t>
	inline auto buffer<elem_t>::write(size_t offset, iterator_t first, size_t count, bool overwrite) -> buffer&
	{
		return this->write(offset, std::span<elem_t>{ first, first + count }, overwrite);
	}

	template<class elem_t>
	template<class T, size_t N>
	requires (sizeof(T) == sizeof(elem_t))
	inline auto buffer<elem_t>::write(size_t offset, const T (&data)[N], bool overwrite) -> buffer&
	{
		return this->write(offset, std::span< T>{ const_cast<T*>(data), N }, overwrite);
	}

	template<class elem_t>
	template<size_t N>
	inline auto buffer<elem_t>::write(size_t offset, const elem_t (&data)[N], bool overwrite) -> buffer&
	{
		return this->write(offset, std::span<elem_t>{ const_cast<elem_t*>(data), N }, overwrite);
	}

	template<class elem_t>
	template<class T>
	requires xmem::iterable_t<T, elem_t>
	inline auto buffer<elem_t>::write(size_t offset, const T&& data, bool overwrite) -> buffer&
	{
		return this->write(offset, data, overwrite);
	}

	template<class elem_t>
	template<class T>
	requires xmem::iterable_t<T, elem_t>
	inline auto buffer<elem_t>::write(size_t offset, const T& data, bool overwrite) -> buffer&
	{
		return this->write(offset, data.begin(), data.end(), overwrite);
	}

	template<class elem_t>
	template<class iterator_t>
	requires xmem::valid_iterator_t<iterator_t, elem_t>
	inline auto buffer<elem_t>::write(size_t offset, iterator_t begin, iterator_t end, bool overwrite) -> buffer&
	{

		auto size{ static_cast<size_t>(end - begin) };

		if (offset > this->m_Count) 
		{
			size_t length = (offset - this->m_Count) + size;
			this->check(length);
			this->m_Count = length;
		}
		else if (overwrite)
		{
			size_t length = offset + size;
			if (length > this->m_Count) 
			{
				auto n_size{ length - this->m_Count };
				this->check(n_size);
				this->m_Count += n_size;
			}
		}
		else
		{
			this->check(size);
			auto first{ this->begin() + offset };
			auto last { first + size };
			std::copy(first, last, last);
			this->m_Count += size;
		}
		std::copy(begin, end, this->begin() + offset);
		return *this;
	}

	template<class elem_t>
	template<class T>
	requires (sizeof(T) == sizeof(elem_t))
	inline auto buffer<elem_t>::memset(size_t offset, T value, size_t count, bool overwrite) -> buffer&
	{
		return this->memset(offset, static_cast<elem_t>(value), count, overwrite);
	}

	template<class elem_t>
	inline auto buffer<elem_t>::memset(size_t offset, elem_t value, size_t count, bool overwrite) -> buffer&
	{
		if (offset > this->m_Count)
		{
			size_t length = (offset - this->m_Count) + count;
			this->check(length);
			this->m_Count = length;
		}
		else if (overwrite)
		{
			size_t length = offset + count;
			if (length > this->m_Count)
			{
				auto n_size{ length - this->m_Count };
				this->check(n_size);
			}
			this->m_Count = length;
		}
		else
		{
			this->check(count);
			auto first{ this->begin() + offset };
			auto last{ first + count };
			std::copy(first, last, last);
			this->m_Count += count;
		}

		std::span<elem_t> temp { this->data() + offset, count };
		for (elem_t& elem : temp) 
		{
			elem = value; 
		}

		return *this;
	}

	template<class elem_t>
	inline auto buffer<elem_t>::written() -> std::span<elem_t>
	{
		return std::span<elem_t>{ this->data(), this->m_Count };
	}

	template<class elem_t>
	inline auto buffer<elem_t>::recount(size_t count) -> buffer&
	{
		if (count > this->m_Buffer.size()) 
		{
			this->m_Buffer.resize(count);
		}
		this->m_Count = count;

		return *this;
	}

	template<class elem_t>
	inline auto buffer<elem_t>::resize(size_t size) -> buffer&
	{
		this->m_Buffer.resize(size);
		if (size < this->m_Count) 
		{
			this->m_Count = size;
		}
		return *this;
	}

	template<class elem_t>
	inline auto buffer<elem_t>::count() const -> size_t
	{
		return this->m_Count;
	}

	template<class elem_t>
	inline auto buffer<elem_t>::size() const -> size_t
	{
		return this->m_Buffer.size();
	}

	template<class elem_t>
	inline auto buffer<elem_t>::clear() -> buffer&
	{
		this->m_Buffer.clear();
		this->m_Count = 0;
		return *this;
	}

	template<class elem_t>
	inline auto buffer<elem_t>::begin() -> iterator
	{
		return this->m_Buffer.begin();
	}

	template<class elem_t>
	inline auto buffer<elem_t>::end() -> iterator
	{
		return this->m_Buffer.end();
	}

	template<class elem_t>
	inline auto buffer<elem_t>::data() -> elem_t*
	{
		return this->m_Buffer.data();
	}

	template<class elem_t>
	inline auto buffer<elem_t>::copy() -> buffer
	{
		return { *this };
	}

	template<class elem_t>
	inline auto buffer<elem_t>::operator=(const buffer& other) -> buffer&
	{
		if (this == &other) 
		{
			return *this;
		}

		this->m_Buffer = other.m_Buffer;
		this->m_Count = other.m_Count;

		return *this;
	}

	template<class elem_t>
	inline auto buffer<elem_t>::operator=(buffer&& other) noexcept -> buffer&
	{
		if (this == &other) 
		{
			return *this; 
		}

		this->m_Buffer = std::move(other.m_Buffer);
		this->m_Count = other.m_Count;

		other.m_Count = 0;

		return *this;
	}

	template<class elem_t>
	inline auto buffer<elem_t>::operator[](size_t index) -> elem_t
	{
		if (index >= this->m_Buffer.size())
		{
			throw std::exception
			{ 
				"buffer::operator[](size_t): index out of bounds!" 
			};
		}

		elem_t result = this->m_Buffer[index];
		return result;
	}

}