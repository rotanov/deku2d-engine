#ifndef _2DE_STORAGE_H_
#define _2DE_STORAGE_H_

namespace Deku2D
{
	/**
	* CStorage - base class for CFile and CMemory classes, that describes their interface.
	*/

	class CStorage
	{
	public:
		enum EOpenMode
		{
			OPEN_MODE_READ,
			OPEN_MODE_WRITE,
		};
		
		enum ESeekOrigin
		{
			SEEK_ORIGIN_BEGINNING,
			SEEK_ORIGIN_CURRENT,
			SEEK_ORIGIN_END,
		};

		CStorage();

		virtual bool Close() = 0;

		virtual bool Read(void *Buffer, size_t BytesCount, size_t ElementsCount = 1) = 0;
		virtual bool Write(const void *Data, size_t BytesCount, size_t ElementsCount = 1) = 0;
		
		// kinda-type-and-size-safe versions of Read and Write.. don't work with arrays, though..
		template<typename T>
		bool Read(T *Buffer)
		{
			return Read(Buffer, sizeof(T));
		}

		template<typename T>
		bool Write(const T *Data)
		{
			return Write(Data, sizeof(T));
		}

		bool ReadByte(byte *Buffer);
		bool WriteByte(byte *Data);
		bool WriteByte(byte Data);

		// i think, that CFile should not provide posibility to read 0-terminated strings from files..
		// formats should be developed in such way, that does NOT include reading variable-length strings from binary files.. it's bad design of data structures to do so..
		// BUT even if someone ever need to store string in binary file, they should develop structure for storing string size too, together with string.. and then deal with it in higher level of abstraction..
		// YES OFCOURSE YOU"RE FUCKIING RIGHT> LET"S DO IT

		bool ReadString(char *Buffer, int ASize);
		bool ReadString(string &Buffer);
		bool WriteString(const char *Data);
		bool WriteString(const string &Data);

		bool WriteText(const char *Data);
		bool WriteText(const string &Data);

		virtual bool ReadLine(char *Buffer, int ASize) = 0;
		virtual bool ReadLineS(string &Buffer);
		bool WriteLine(const char *Data);
		bool WriteLine(const string &Data);

		virtual string GetContent() = 0;
		bool SetContent(const string &Data);

		virtual bool Seek(long Offset, ESeekOrigin Origin) = 0;
		bool Rewind();
		virtual bool Flush();

		virtual bool Eof() const = 0;
		virtual bool Good() const = 0;
		virtual unsigned long Size() const = 0;

	protected:
		bool WriteLine(const char *Data, size_t Size);

		EOpenMode Mode;

		const unsigned int READ_BUFFER_DEFAULT_SIZE;

	private:
		// CStorage can't be copied or assigned.
		CStorage(const CStorage &Source);
		CStorage& operator=(const CStorage &Source);
	};

	/**
	* CFile - a class that provides interface to reading and writing of files.
	*
	* Each instance of the class represents a single file, implementing RAII principle.
	* This class is a front-end for stdio. Also extends it with some lazy-functions, like get file contents into a string.
	* Can't be copied or assigned.
	*/

	class CFile : public CStorage
	{
	public:
		CFile();
		CFile(const string &AFilename, EOpenMode AMode);
		~CFile();

		bool Open(const string &AFilename, EOpenMode AMode);
		bool Close();

		bool Read(void *Buffer, size_t BytesCount, size_t ElementsCount = 1);
		bool Write(const void *Data, size_t BytesCount, size_t ElementsCount = 1);
		
		bool ReadLine(char *Buffer, int ASize);

		string GetContent();

		bool Seek(long Offset, ESeekOrigin Origin);
		bool Flush();

		bool Eof() const;
		bool Good() const;
		unsigned long Size() const;

	private:
		FILE *File;
		string Filename;
	};

	class CMemory : public CStorage
	{
	public:
		CMemory();
		CMemory(byte *ABeginningPointer, long ALength, EOpenMode AMode);
		~CMemory();

		bool Open(byte *ABeginningPointer, long ALength, EOpenMode AMode);
		bool Close();

		bool Read(void *Buffer, size_t BytesCount, size_t ElementsCount = 1);
		bool Write(const void *Data, size_t BytesCount, size_t ElementsCount = 1);

		bool ReadLine(char *Buffer, int ASize);

		string GetContent();

		bool Seek(long Offset, ESeekOrigin Origin);

		bool Eof() const;
		bool Good() const;
		unsigned long Size() const;

	private:
		byte *BeginningPointer;
		byte *CurrentPointer;
		unsigned long Length;
	};

};

#endif	//	_2DE_STORAGE_H_
