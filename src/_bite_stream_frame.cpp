#include <iostream>
#include <vector>
#include <string>
#include <exception>
#include "_bite_stream_frame.h"
#include "_bite_defines.h"




__forceinline char *copymem(const char *src, size_t src_len)
{
	return (char *)memcpy(new char[src_len], src, src_len);
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

	void StreamWriter::write(const char *buffer, size_t length)
	{
		m_stream->write(buffer, length);
	}

	void StreamWriter::writeCstr(const char *buffer, size_t length)
	{
		if (length < 2)
			return;
		write(buffer, length - 1);
	}

	void StreamWriter::insert(const char *buffer, size_t length)
	{
		if (!length)
			return;
		write(buffer, length);
		move(-(intptr_t)length);
	}

	void StreamWriter::insertCstr(const char *buffer, size_t length)
	{
		if (length < 2)
			return;
		write(buffer, length - 1);
		move(1 - (intptr_t)length);
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
