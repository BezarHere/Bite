#include <iostream>
#include <vector>
#include <string>
#include <exception>
#include "_bite_StreamFrame.h"
#include "_bite_defines.h"


constexpr char16_t HexTable[ 256 ]
{
	'00', '10', '20', '30', '40', '50', '60', '70', '80', '90', 'a0', 'b0', 'c0', 'd0', 'e0', 'f0',
	'01', '11', '21', '31', '41', '51', '61', '71', '81', '91', 'a1', 'b1', 'c1', 'd1', 'e1', 'f1',
	'02', '12', '22', '32', '42', '52', '62', '72', '82', '92', 'a2', 'b2', 'c2', 'd2', 'e2', 'f2',
	'03', '13', '23', '33', '43', '53', '63', '73', '83', '93', 'a3', 'b3', 'c3', 'd3', 'e3', 'f3',
	'04', '14', '24', '34', '44', '54', '64', '74', '84', '94', 'a4', 'b4', 'c4', 'd4', 'e4', 'f4',
	'05', '15', '25', '35', '45', '55', '65', '75', '85', '95', 'a5', 'b5', 'c5', 'd5', 'e5', 'f5',
	'06', '16', '26', '36', '46', '56', '66', '76', '86', '96', 'a6', 'b6', 'c6', 'd6', 'e6', 'f6',
	'07', '17', '27', '37', '47', '57', '67', '77', '87', '97', 'a7', 'b7', 'c7', 'd7', 'e7', 'f7',
	'08', '18', '28', '38', '48', '58', '68', '78', '88', '98', 'a8', 'b8', 'c8', 'd8', 'e8', 'f8',
	'09', '19', '29', '39', '49', '59', '69', '79', '89', '99', 'a9', 'b9', 'c9', 'd9', 'e9', 'f9', 
	'0a', '1a', '2a', '3a', '4a', '5a', '6a', '7a', '8a', '9a', 'aa', 'ba', 'ca', 'da', 'ea', 'fa',
	'0b', '1b', '2b', '3b', '4b', '5b', '6b', '7b', '8b', '9b', 'ab', 'bb', 'cb', 'db', 'eb', 'fb',
	'0c', '1c', '2c', '3c', '4c', '5c', '6c', '7c', '8c', '9c', 'ac', 'bc', 'cc', 'dc', 'ec', 'fc',
	'0d', '1d', '2d', '3d', '4d', '5d', '6d', '7d', '8d', '9d', 'ad', 'bd', 'cd', 'dd', 'ed', 'fd',
	'0e', '1e', '2e', '3e', '4e', '5e', '6e', '7e', '8e', '9e', 'ae', 'be', 'ce', 'de', 'ee', 'fe',
	'0f', '1f', '2f', '3f', '4f', '5f', '6f', '7f', '8f', '9f', 'af', 'bf', 'cf', 'df', 'ef', 'ff'
};

__forceinline char *copymem(const char *src, size_t src_len)
{
	return (char *)memcpy(new char[src_len], src, src_len);
}

namespace bite
{

	template <typename STREAM>
	StreamFrame<STREAM>::StreamFrame()
		: m_stream{ nullptr }, m_order{ EndianOrder::Little }
	{
	}

	template<typename STREAM>
	StreamFrame<STREAM>::StreamFrame(std::shared_ptr<STREAM> stream, EndianOrder order)
		: m_stream{ stream }, m_order{ order }
	{
	}

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


	template <typename STREAM>
	void StreamFrame<STREAM>::setStream(STREAM *stream)
	{
		if (!stream)
			throw std::runtime_error("'stream' is nullptr.");

	}

	template <typename STREAM>
	void StreamFrame<STREAM>::setStream(std::shared_ptr<STREAM> stream)
	{
	}

	template <typename STREAM>
	std::shared_ptr<STREAM> StreamFrame<STREAM>::getStream()
	{
		return m_stream;
	}

	template <typename STREAM>
	const std::shared_ptr<const STREAM> StreamFrame<STREAM>::getStream() const
	{
		return (std::shared_ptr<const STREAM>)m_stream;
	}

	template<typename STREAM>
	EndianOrder StreamFrame<STREAM>::order() const
	{
		return m_order;
	}

	template<typename STREAM>
	std::ios::iostate StreamFrame<STREAM>::state() const
	{
		return m_stream ? m_stream->rdstate() : -1;
	}

	template<typename STREAM>
	bool StreamFrame<STREAM>::valid() const
	{
		return m_stream && m_stream->good();
	}

	template<typename STREAM>
	StreamFrame<STREAM>::operator bool() const
	{
		return valid();
	}

	template<typename STREAM>
	bool StreamFrame<STREAM>::operator!() const
	{
		return !valid();
	}

	void StreamReader::move(intptr_t offset)
	{
		// base-case
		if (!offset) return;

		intptr_t dst{ (intptr_t)cursor() + offset };

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
			((char16_t*)buffer)[ (i + 1) ] = HexTable[ ptr[ i ] ];
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
