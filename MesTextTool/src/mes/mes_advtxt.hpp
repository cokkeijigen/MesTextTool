#pragma once
#include <span>
#include <vector>
#include <algorithm>

namespace mes
{
	class advtxt_info
	{
		static std::vector<advtxt_info> advtxt_infos;
		static const char* const advtxt_supports[];

	public:
		
		const std::string name;
		const std::vector<uint8_t> encstrs; // the opcode for encrypted strings in scene text

		inline auto is_encstrs(uint8_t value) const noexcept -> bool;

		static auto   get(const std::string_view name) -> const advtxt_info*;
		static auto  make(std::string_view name, std::vector<uint8_t>&& encstrs) -> const advtxt_info*;

		static auto infos() -> const std::vector<advtxt_info>&;
		static auto supports() -> const std::span<const char* const>;
	};

	class advtxt_view
	{
	public:

		struct token
		{
			const uint8_t*      data{};
			int32_t offset{}, length{};

		    #pragma pack(push, 1)
			struct value_t { uint8_t opcode, data[]; };
			#pragma pack(pop)

			inline auto operator->() const noexcept -> const value_t* 
			{
				return reinterpret_cast<const value_t*>(this->data);
			}

			inline auto value() const noexcept -> const value_t*
			{
				return reinterpret_cast<const value_t*>(this->data);
			}
		};

		#pragma pack(push, 1)
		struct header_t
		{
			uint8_t magic[8]; // #ADV_TXT
			struct
			{
				int32_t   count;
				uint16_t data[];
			} entries;
		};
		#pragma pack(pop)

		template<class T>
		class view_t : public std::span<T>
		{
			friend advtxt_view;
			int32_t m_offset{};

		public:
			using std::span<T>::span;
			using std::span<T>::operator=;
			inline static constexpr auto nops{ static_cast<size_t>(-1) };

			inline view_t(const std::span<T>& data, int32_t offset) noexcept :
				std::span<T>::span{ data }, m_offset{ offset }
			{
			}

			inline view_t(view_t&& other) noexcept
			{
				this->operator=(std::move(other));
			}

			inline auto operator=(view_t&& other) noexcept -> view_t&
			{
				if (this != &other)
				{
					std::span<T>::operator=(std::move(other));
					this->m_offset = other.m_offset;
				}
				return *this;
			}

			inline auto offset() const noexcept -> int32_t
			{
				return this->m_offset;
			}

		protected:

			inline auto find(size_t start, const std::span<const T> target) -> size_t
			{
				const auto it{ std::search(this->begin() + start, this->end(), target.begin(), target.end()) };
				if (it == this->end())
				{
					return view_t<T>::nops;
				}
				return std::distance(this->begin(), it);
			}
		};

		advtxt_view() noexcept = default;
		advtxt_view(const std::span<uint8_t> raw, const advtxt_info* info) noexcept;

		inline auto is_parsed() const noexcept -> bool;

		inline auto raw   () const noexcept -> const view_t<uint8_t>&;
		inline auto asmbin() const noexcept -> const view_t<uint8_t>&;
		inline auto tokens() const noexcept -> const std::vector<token>&;
		inline auto header() const noexcept -> const header_t*;
		inline auto info  () const noexcept -> const advtxt_info*;
	protected:
		const   advtxt_info*       m_info{};
		mutable view_t<uint8_t>    m_raw {};
		mutable view_t<uint8_t>    m_asmbin{};
		mutable std::vector<token> m_tokens{};
		auto token_parse() noexcept -> void;
	};

	inline auto advtxt_view::is_parsed() const noexcept -> bool
	{
		return !this->m_tokens.empty();
	}

	inline auto advtxt_view::raw() const noexcept -> const view_t<uint8_t>&
	{
		return this->m_raw;
	}

	inline auto advtxt_view::asmbin() const noexcept -> const view_t<uint8_t>&
	{
		return this->m_asmbin;
	}

	inline auto advtxt_view::tokens() const noexcept -> const std::vector<token>&
	{
		return this->m_tokens;
	}

	inline auto advtxt_view::header() const noexcept -> const header_t*
	{
		return reinterpret_cast<const header_t*>(this->m_raw.data());
	}

	inline auto advtxt_view::info() const noexcept -> const advtxt_info*
	{
		return this->m_info;
	}

	inline auto advtxt_info::is_encstrs(uint8_t value) const noexcept -> bool
	{
		return std::find(this->encstrs.begin(), this->encstrs.end(), value) != this->encstrs.end();
	}

	auto is_advtxt(const std::span<const uint8_t> data) -> bool;

	namespace advtxt 
	{
		using view = advtxt_view;
		using info = advtxt_info;
		inline static constexpr const uint8_t endtoken[2]{ 0x0A, 0x0D };
		static inline constexpr const uint8_t magic   [8]{ '#', 'A','D','V','_','T','X', 'T' };

		auto decrypt_string(const std::span<const uint8_t> str) -> std::string;

		auto string_parse(const view::token& token) -> std::string;
	}
}