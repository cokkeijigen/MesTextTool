#pragma once
#include <vector>

namespace utils::xstr 
{
	template<class T>
	struct extract_char_type 
	{
		using type = std::remove_cvref_t<T>;
	};

	template<class T>
	struct extract_char_type<std::basic_string<T>>
	{
		using type = T; 
	};

	template<class T>
	struct extract_char_type<std::basic_string_view<T>> 
	{
		using type = T; 
	};

	template<class T>
	struct extract_char_type<T*>
	{
		using type = std::remove_cv_t<T>; 
	};

	template<class T, size_t N>
	struct extract_char_type<T[N]> 
	{ 
		using type = std::remove_cv_t<T>; 
	};

	template<class T>
	using char_type_v = typename extract_char_type<std::remove_cvref_t<T>>::type;

	template<class char_type>
	class string_builder
	{
		std::vector<std::basic_string_view<char_type>> m_views{};

	public:

		using char_t = char_type;
		
		inline auto build() const noexcept -> std::basic_string<char_type>;
		inline auto operator*() const noexcept -> std::basic_string<char_type>;

		template<class... V>
		requires (std::convertible_to<V, std::basic_string_view<char_type>> && ...)
		inline string_builder(V&&... args) noexcept;

		template<class... V>
		requires (std::convertible_to<V, std::basic_string_view<char_type>> && ...)
		inline auto append(V&&... args) noexcept -> string_builder&;

		template<class V>
		requires std::convertible_to<V, std::basic_string_view<char_type>>
		inline auto operator+=(V&& arg) noexcept -> string_builder&;
		
	};

	template<class char_type>
	inline auto string_builder<char_type>::build() const noexcept -> std::basic_string<char_type>
	{
		return this->m_views | std::views::join | std::ranges::to<std::basic_string<char_type>>();
	}

	template<class char_type>
	inline auto string_builder<char_type>::operator*() const noexcept -> std::basic_string<char_type>
	{
		return this->build();
	}

	template<class char_type>
	template<class... V>
	requires (std::convertible_to<V, std::basic_string_view<char_type>> && ...)
	inline string_builder<char_type>::string_builder(V&&... args) noexcept
	{
		this->m_views.reserve(sizeof...(args));
		(
			this->m_views.emplace_back(std::forward<V>(args)),
			...
		);
	}

	template<class char_type>
	template<class ...V>
	requires (std::convertible_to<V, std::basic_string_view<char_type>> && ...)
	inline auto string_builder<char_type>::append(V && ...args) noexcept -> string_builder&
	{
		this->m_views.reserve(m_views.size() + sizeof...(args));
		(
			this->m_views.emplace_back(std::forward<V>(args)), 
			...
		);
		return *this;
	}

	template<class char_type>
	template<class V>
	requires std::convertible_to<V, std::basic_string_view<char_type>>
	inline auto string_builder<char_type>::operator+=(V&& arg) noexcept -> string_builder&
	{
		this->m_views.emplace_back(std::forward<V>(arg));
		return *this;
	}

	template<class first, class... rest>
	string_builder(first&&, rest&&...) -> string_builder<char_type_v<first>>;

}