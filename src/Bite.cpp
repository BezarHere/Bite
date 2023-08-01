#include <iostream>
#include <vector>
#include <string>
#include <exception>
#include <Windows.h>
#include "Bite.h"

#define invalidcall throw std::runtime_error(std::string("Invalid call to un implemented function ") + __FUNCTION__ + "@" + __LINE__ + '.')

#define halt(msg) throw std::runtime_error(std::string(msg) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + ")")
//#define halt(msg) std::cout << std::endl << (std::string(msg) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + ")" + "\n[ press enter to abort ]") << std::endl; std::cin.get(); abort()
#define show(x) bite::cmddye::M_PutTheme({ bite::cmddye::CMDColor::LightYellow, bite::cmddye::CMDColor::Black }); std::cout << #x << ": " << (x) << std::endl; bite::cmddye::M_PutTheme({ bite::cmddye::CMDColor::White, bite::cmddye::CMDColor::Black })

#define BS namespace bite {
#define CDS namespace cmddye {
#define END }

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

BS

#pragma region(CTors & DTors)

File::File(const std::string& path, const OpenMode mode)
	: m_path(path), m_mode(mode),
	m_stream(path, std::ios::binary | ((mode & 1) ? std::ios::in : (std::ios::out | std::ios::trunc)))
{
	if (fail())
	{
		std::cout << "Failed to open file to path: \"" << path << "\" with state: " << state() << std::endl;
		return;
	}
	recalculate();
}

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

#pragma endregion


bool File::isForReading() const
{
	return m_mode & 1;
}

bool File::isForWriting() const
{
	return !(m_mode & 1);
}

const std::string& File::getPath() const
{
	return m_path;
}

const size_t &File::size() const
{
	return m_size;
}

const size_t &File::space() const
{
	return m_space;
}

const size_t& File::tell(bool update)
{
	if (update)
		recalculate();
	return m_cursor;
}

const size_t& File::tell() const
{
	return m_cursor;
}

void File::seek(size_t index)
{
	if (isForReading())
	{
		m_stream.seekg(index);
	}
	else
	{
		m_stream.seekp(index);
	}
	recalculate();
}

File::OpenMode File::mode() const
{
	return m_mode;
}

bool File::isAscii() const
{
	return mode() == ReadAscii || mode() == WriteAscii;
}

bool File::isBinary() const
{
	return mode() == ReadBinary || mode() == WriteBinary;
}

bool File::good() const
{
	return m_stream.good();
}

bool File::fail() const
{
	return m_stream.fail();
}

std::ios::iostate File::state() const
{
	return m_stream.rdstate();
}

void File::write(const char* const buffer, size_t from, size_t to)
{
	if (isForReading() || m_stream.fail())
	{
		return;
	}

	size_t b_length = sizeof(buffer);
	size_t w_length = to > from ? to - from : b_length - from;

	// remove terminating null
	if (m_mode == WriteAscii)
	{
		if (--w_length <= 0) return;
	}

	m_stream.write(buffer, w_length);
	recalculate();
}

BufferSmartPtr_t File::read(size_t len)
{
	if (isForWriting() || m_stream.fail())
	{
		return nullptr;
	}
	size_t reading_length = len > 0 ? len : m_space;

	// not enough reading space
	if (space() < len)
	{
		throw std::runtime_error(
			"Not enough data for buffer of size " + std::to_string(reading_length) + ", there is only " + std::to_string(space()) + " bytes for reading: returning 'nullptr'");
		return nullptr;
	}

	size_t value_length = reading_length;

	// creating space for terminating null
	if (m_mode == ReadAscii)
	{
		value_length++;
	}
	char* buffer = new char[ value_length ];

	load(buffer, reading_length);

	// add terminating null
	if (m_mode == ReadAscii)
	{
		buffer[ reading_length ] = '\0';
	}

	recalculate();
	return BufferSmartPtr_t(buffer);
}

void File::load(char* buffer, size_t len)
{
	if (isForWriting() || m_stream.fail())
	{
		throw std::runtime_error("failed");
	}
	if (space() < len)
	{
		throw std::runtime_error(
			"Not enough data to load char[" + std::to_string(len) + "], there is only char[" + std::to_string(space()) + "] left for reading.");
		return;
	}
	m_stream.read(buffer, len);
	recalculate();
}

void File::flush()
{
	m_stream.flush();
}

void File::close()
{
	m_stream.close();
}

bool File::valid() const
{
	return !fail() && m_stream.is_open();
}

bool File::canRead() const
{
	return isForReading() && !fail() && m_stream.is_open();
}

bool File::canWrite() const
{
	return isForWriting() && !fail() && m_stream.is_open();
}

File::operator bool() const
{
	return valid();
}

void File::recalculate()
{
	// why?
	if (isForReading())
	{
		std::streampos current_cur = m_stream.tellg();
		m_stream.seekg(0, std::ios::end);
		size_t end_cur = (size_t)m_stream.tellg();
		m_size = (size_t)end_cur;
		m_space = (size_t)(end_cur - current_cur);
		m_stream.seekg(current_cur);
		m_cursor = (size_t)(current_cur);
	}
	else
	{
		std::streampos current_cur = m_stream.tellp();
		m_stream.seekp(0, std::ios::end);
		size_t end_cur = (size_t)m_stream.tellp();
		m_size = (size_t)end_cur;
		m_space = (size_t)(end_cur - current_cur);
		m_stream.seekp(current_cur);
		m_cursor = (size_t)current_cur;
	}
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

#pragma region(Reading and Peeking func types)

BufferSmartPtr_t StreamReader::read(size_t length)
{
	char *buffer = new char[ length ];
	load(buffer, length);
	return BufferSmartPtr_t(buffer);
}

BufferSmartPtr_t StreamReader::peek(size_t length)
{
	BufferSmartPtr_t buffer = read(length);
	move(-(intptr_t)length);
	return buffer;
}

BufferSmartPtr_t StreamReader::readCstr(size_t length)
{
	char *buffer = new char[ length + 1 ];
	load(buffer, length);
	buffer[ length ] = '\0';
	return BufferSmartPtr_t(buffer);
}

#pragma endregion

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

#pragma region(Global Functions)


void M_Dye(const std::string &text, cmddye::CMDTheme colors, std::ostream &stream)
{
	cmddye::CMDTheme def_t = cmddye::M_GetTheme();
	cmddye::M_PutTheme(colors);
	stream << text;
	cmddye::M_PutTheme(def_t);
}

void M_Dye(const char *text, cmddye::CMDTheme colors, std::ostream &stream)
{
	cmddye::CMDTheme def_t = cmddye::M_GetTheme();
	cmddye::M_PutTheme(colors);
	stream << text;
	cmddye::M_PutTheme(def_t);
}

void M_Dye(const std::exception &exc, cmddye::CMDTheme colors, std::ostream &stream)
{
	cmddye::CMDTheme def_t = cmddye::M_GetTheme();
	cmddye::M_PutTheme(colors);
	stream << exc.what() << std::endl;
	cmddye::M_PutTheme(def_t);
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
	char* buffer = new char[ (length * 2) + 3 ];
	buffer[ 0 ] = '0';
	buffer[ 1 ] = 'x';
	buffer[ (length * 2) + 2 ] = '\0';

	// why?
	for (size_t i{0}; i < length; i++)
	{
		((char16_t*)buffer)[ (i + 1) ] = HexTable[ ptr[ i ] ];
	}
	return BufferSmartPtr_t(buffer);
}

BufferVector_t M_SplitBuffer(const char *src, size_t src_len, size_t block_len, size_t* last_block_len)
{
	BufferVector_t result{};

	if (src_len <= block_len)
	{
		result.emplace_back((char *)memcpy(new char[block_len] {}, src, src_len));
		return result;
	}

	result.reserve(src_len / block_len);
	
	size_t i{};
	size_t len{};
	for (intptr_t bytes_left{ (intptr_t)src_len }; bytes_left > 0; bytes_left -= block_len)
	{
		len = min((size_t)bytes_left, block_len);
		result.emplace_back((char *)memcpy(new char[block_len] {}, src + (block_len * i++), len));
	}

	if (last_block_len)
		*last_block_len = len;

	return result;
}

#pragma endregion

namespace cmddye
{
		static const CMDTheme F_DefaultTheme{bite::cmddye::M_GetTheme()};

	CMDTheme bite::cmddye::M_GetTheme()
	{
		return CMDTheme{};
	}

	CMDTheme M_GetDefaultTheme()
	{
		return F_DefaultTheme;
	}

	void bite::cmddye::M_PutTheme(CMDTheme theme)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD)M_UnpackTheme(theme));
	}

	void bite::cmddye::M_ClearTheme()
	{
		M_PutTheme(F_DefaultTheme);
	}

	constexpr uint8_t M_UnpackTheme(CMDTheme theme)
	{
		return (uint8_t)theme.fg + ((uint8_t)theme.bg << 4);
	}

	constexpr CMDTheme M_PackTheme(uint8_t theme)
	{
		return { (CMDColor)(theme & 0xf), (CMDColor)(theme >> 4) };
	}

} // cmddye


END // bite

#if 0
int main()
{
	std::cout << std::boolalpha;
	{
		bite::StreamReader reader{ std::shared_ptr<std::ifstream>{ new std::ifstream("test", std::ios::binary) }, bite::EndianOrder::Big };
		show(reader.size());
		show(reader.cursor());
		show(bite::M_EndianNativeToMemory(reader.order()));
		//show(reader.order());
		show(bite::M_Hexfy((uint8_t *)reader.peek(4).get(), 4));
		show(bite::M_Hexfy((uint8_t *)reader.read(4).get(), 4));
		printf("advancing by 8\n");
		reader.move(8);
		show(reader.cursor());
		show(bite::M_Hexfy((uint8_t *)reader.read(4).get(), 4));
		printf("advancing by -8\n");
		reader.move(-8);
		show(bite::M_Hexfy((uint8_t *)reader.read(4).get(), 4));
		show(reader.read(4));
		printf("advancing by -cursor\n");
		reader.move(-(intptr_t)reader.cursor());
		show((reader.peek<uint32_t, false>()));
		show((reader.peek<uint32_t, true>()));

		size_t p_ls;
		std::vector<char *> p = bite::M_SplitBuffer(reader.read(100).release(), 100, 32, &p_ls);

		show(p.size());

		try
		{
			reader.move(-32);
		}
		catch (std::exception e)
		{
			bite::M_Dye(e);
		}

		show((int)bite::cmddye::M_UnpackTheme(bite::cmddye::M_GetTheme()));
		bite::StreamWriter writer{
			"E:\\Assets\\visual studio\\ByteBlender\\output.bin", bite::EndianOrder::Little
		};

		//std::ofstream *writer = new std::ofstream("E:\\Assets\\visual studio\\ByteBlender\\output.bin", std::ios::binary);

		writer.write<int32_t>(32);
		writer.write<int32_t>(-32);
		writer.write<int32_t>(32<<2);
		writer.write<int32_t>(32>>2);
	}

	std::cin.get();
}
#endif

