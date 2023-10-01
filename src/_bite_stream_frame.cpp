#include <iostream>
#include <vector>
#include <string>
#include <exception>
#include <cassert>
#include "_bite_stream_frame.h"
#include "_bite_reporting.h"
#include "_bite_defines.h"
#undef min
#undef max



__forceinline char *copymem(const char *src, size_t src_len)
{
	return (char *)memcpy(new char[src_len], src, src_len);
}

class memorybuf : public std::streambuf
{
public:
	inline memorybuf(const char *buf, const std::streamoff sz)
		: m_buf{ (const char *)memcpy(new char[sz], buf, sz) },
		m_sz{ sz },
		m_cur{ 0 }

	{
	}
private:
	inline int_type underflow()
	{
		if (m_cur == m_sz)
			return traits_type::eof();
		return traits_type::to_int_type(*(m_buf.get() + m_cur));
	}

	inline int_type uflow()
	{
		if (m_cur == m_sz)
			return traits_type::eof();
		return traits_type::to_int_type(*(m_buf.get() + m_cur++));
	}

	inline int_type pbackfail(int_type ch)
	{
		const char *cur = m_buf.get() + m_cur;
		if (!m_cur || (ch != traits_type::eof() && ch != cur[ -1 ]))
			return traits_type::eof();
		return traits_type::to_int_type(*(m_buf.get() + --m_cur));
	}

	inline std::streamsize showmanyc()
	{
		return m_sz - m_cur;
	}

	inline std::streampos seekoff(
		std::streamoff off,
		std::ios_base::seekdir way,
		std::ios_base::openmode wich = std::ios::in | std::ios::out)
	{
		switch (way)
		{
		case std::ios::beg:
			m_cur = off;
			break;
		case std::ios::cur:
			m_cur += off;
			break;
		case std::ios::end:
			m_cur = m_sz + off;
			break;
		default:
			return -1;
		}

		if (m_cur < 0 || m_cur > m_sz)
			return -1;
		return m_cur;
	}

	inline std::streampos seekpos(
		std::streampos pos,
		std::ios_base::openmode wich = std::ios::in | std::ios::out)
	{
		m_cur = pos;
		if (m_cur < 0 || m_cur > m_sz)
			return -1;
		return m_cur;
	}

	memorybuf(const memorybuf &) = delete;
	memorybuf &operator =(const memorybuf &) = delete;

private:
	const std::shared_ptr<const char[]> m_buf;
	const std::streamoff m_sz;
	std::streamoff m_cur;
};

// will copy buf
std::istream *bite::load_mem_input(bite::byte_t *buf, const size_t sz)
{
	return new std::istream(new memorybuf((char *)buf, sz));
}

// will copy buf
std::ostream *bite::load_mem_output(bite::byte_t *buf, const size_t sz)
{
	return new std::ostream(new memorybuf((char *)buf, sz));
}

namespace bite
{

	StreamWriter::StreamWriter()
		: StreamFrame()
	{
	}

	StreamWriter::StreamWriter(std::shared_ptr<stream_type> stream, EndianOrder order)
		: StreamFrame(stream, order)
	{
	}

	StreamWriter::StreamWriter(stream_type *stream, EndianOrder order)
		: StreamFrame(std::shared_ptr<stream_type>(stream), order)
	{
	}

	StreamWriter::StreamWriter(const std::string &path, EndianOrder order)
		: StreamFrame(std::shared_ptr<stream_type>( new std::ofstream(path, std::ios::binary) ), order)
	{
	}

	StreamWriter::~StreamWriter()
	{
		if (m_stream)
			close();
	}

	StreamReader::StreamReader()
		: StreamFrame()
	{
	}

	StreamReader::StreamReader(std::shared_ptr<stream_type> stream, EndianOrder order)
		: StreamFrame(stream, order)
	{
	}

	StreamReader::StreamReader(stream_type* stream, EndianOrder order)
		: StreamFrame(std::shared_ptr<stream_type>(stream), order)
	{
	}

	StreamReader::StreamReader(byte_t *buffer, const size_t buffer_sz, EndianOrder order)
		: StreamReader(load_mem_input(buffer, buffer_sz), order)
	{
	}

	StreamReader::StreamReader(const std::string &path, EndianOrder order)
		: StreamFrame(std::shared_ptr<stream_type>( new std::ifstream(path, std::ios::binary) ), order)
	{
	}

	


	void StreamReader::move(intptr_t offset)
	{
		// base-case
		if (!offset) return;

		intptr_t dst{ ((intptr_t)cursor()) + offset };

		if (dst < 0) halt("move distenation underflow");
		if (dst >= (intptr_t)size()) halt("move distenation overflow");

		m_stream->seekg((std::streampos)dst);
	}

	size_t StreamReader::cursor() const
	{
		return (size_t)m_stream->tellg();
	}

	size_t StreamReader::size() const
	{
		std::streamoff cur{ m_stream->tellg() };
		m_stream->seekg(0, std::ios::end);
		size_t size{ (size_t)m_stream->tellg() };
		m_stream->seekg(cur);
		return size;
	}

	void StreamReader::load(char *buffer, size_t length, bool do_endianeness)
	{
		m_stream->read(buffer, length);
		if (do_endianeness)
		{
			std::unique_ptr<char[]> temp{new char[length]};
			memcpy(temp.get(), buffer, length);
			M_EndianOrder(buffer, (const char *)temp.get(), length, order());
		}
	}


	BufferSmartPtr_t StreamReader::read(size_t length)
	{
		byte_t *buffer = new byte_t[ length ];
		load((char *)buffer, length);
		return BufferSmartPtr_t( buffer );
	}

	BufferSmartPtr_t StreamReader::peek(size_t length)
	{
		BufferSmartPtr_t buffer = read(length);
		move(-(intptr_t)length);
		return buffer;
	}

	BufferSmartPtr_t StreamReader::readCstr(size_t length)
	{
		byte_t *buffer = new byte_t[ length + 1 ];
		load((char *)buffer, length);
		buffer[ length ] = '\0';
		return BufferSmartPtr_t( buffer );
	}


	void StreamWriter::move(intptr_t offset)
	{
		if (!offset) return;

		intptr_t dst{ (intptr_t)cursor() + offset };

		if (dst < 0) halt("move distenation underflow");
		if (dst >= (intptr_t)size()) halt("move distenation overflow");

		m_stream->seekp((std::streampos)dst);
	}

	size_t StreamWriter::cursor() const
	{
		return (size_t)m_stream->tellp();
	}

	size_t StreamWriter::size() const
	{
		std::streamoff cur{ m_stream->tellp() };
		m_stream->seekp(0, std::ios::end);
		size_t size{ (size_t)m_stream->tellp() };
		m_stream->seekp(cur);
		return size;
	}

	void StreamWriter::flush()
	{
		m_stream->flush();
	}

	void StreamWriter::close()
	{
		flush();
		m_stream.reset();
	}

	void StreamWriter::write(const void *buffer, size_t length)
	{
		m_stream->write((const char *)buffer, length);
	}
	void StreamWriter::insert(const void *buffer, size_t length)
	{
		if (!length)
			return;
		write(buffer, length);
		move(-(intptr_t)length);
	}

	void M_EndianOrder(char *dst, const char *src, size_t length, EndianOrder order)
	{
		if (M_EndianNativeToMemory(order))
		{
			memcpy(dst, src, length);
		}
		else
		{
			for (size_t i{ 0 }; i < length; i++)
			{
				dst[ i ] = src[ length - i - 1 ];
			}
		}
	}

	BufferSmartPtr_t M_Hexfy(uint8_t *ptr, size_t length)
	{
		byte_t *buffer = new byte_t[ (length * 2) + 3 ];
		buffer[ 0 ] = '0';
		buffer[ 1 ] = 'x';
		buffer[ (length * 2) + 2 ] = '\0';

		// why?
		for (size_t i{0}; i < length; i++)
		{
			((char16_t*)buffer)[ (i + 1) ] = F_HexTable[ ptr[ i ] ];
		}
		return BufferSmartPtr_t( buffer );
	}

	BufferVector_t M_SplitBuffer(const char *src, size_t src_len, size_t block_len, size_t* last_block_len)
	{
		BufferVector_t result{};

		if (src_len <= block_len)
		{
			result.emplace_back((byte_t *)memcpy(new char[block_len] {}, src, src_len));
			return result;
		}

		result.reserve(src_len / block_len);

		size_t i{};
		size_t len{};
		for (intptr_t bytes_left{ (intptr_t)src_len }; bytes_left > 0; bytes_left -= block_len)
		{
			len = std::min((size_t)bytes_left, block_len);
			result.emplace_back((byte_t *)memcpy(new char[block_len] {}, src + (block_len * i++), len));
		}

		if (last_block_len)
			*last_block_len = len;

		return result;
	}

}
