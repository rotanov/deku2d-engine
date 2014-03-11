#include "2de_Storage.h"

#include "2de_Log.h"
#include "2de_Environment.h"

namespace Deku2D
{
	//////////////////////////////////////////////////////////////////////////
	// CStorage

	CStorage::CStorage() : READ_BUFFER_DEFAULT_SIZE(256)
	{
	}

	bool CStorage::ReadByte(byte *Buffer)
	{
		return Read(Buffer, sizeof(byte));
	}

	bool CStorage::WriteByte(byte *Data)
	{
		return Write(Data, sizeof(byte));
	}

	bool CStorage::WriteByte(byte Data)
	{
		return WriteByte(&Data);
	}

	/**
	* CStorage::ReadString - reads a null-terminated string from a storage with maximum length of ASize bytes and stores it in memory at address Buffer.
	*
	* \todo It woulde be better to take out this logic somewhere, or stop using such ways of storing strings at all.
	*/

	bool CStorage::ReadString(char *Buffer, int ASize)
	{
		if (!Good() || Buffer == NULL || ASize == 0)
			return false;
		
		byte b;
		int i = 0;

		do
		{
			if (!ReadByte(&b))
				return false;

			Buffer[i] = b;
			i++;
		} while (b != 0 && i < ASize);

		return true;
	}

	/**
	* CStorage::ReadString - reads a null-terminated string from a storage and stores it in Buffer.
	* Unlike the version with char* doesn't require to specify size explicitly.
	*
	* \todo It woulde be better to take out this logic somewhere, or stop using such ways of storing strings at all.
	*/

	bool CStorage::ReadString(string &Buffer)
	{
		if (!Good())
			return false;
		
		Buffer.clear();

		byte b;

		if (!ReadByte(&b))
			return false;

		// theoretically (not tested) makes processing somewhat faster on average strings, but consumes more memory..
		Buffer.reserve(READ_BUFFER_DEFAULT_SIZE);

		while (b != 0)
		{
			Buffer += b;

			if (!ReadByte(&b))
				return false;
		}

		return true;
	}

	/**
	* CStorage::WriteString - writes Data string to a storage, terminating it with null byte.
	*
	* \todo It woulde be better to take out this logic somewhere, or stop using such ways of storing strings at all.
	*/

	bool CStorage::WriteString(const char *Data)
	{
		if (Data == NULL)
			return false;

		return Write(Data, strlen(Data) + 1);
	}

	/** \copydoc WriteString(const char *) */

	bool CStorage::WriteString(const string &Data)
	{
		return Write(Data.c_str(), Data.length() + 1);
	}

	/**
	* CStorage::WriteText - writes text from Data to a storage without terminating it with anything.
	*
	* Unlike WriteString this function is good and useful.
	*/

	bool CStorage::WriteText(const char *Data)
	{
		if (Data == NULL)
			return false;
		
		return Write(Data, strlen(Data));
	}

	/** \copydoc WriteText(const char *) */

	bool CStorage::WriteText(const string &Data)
	{
		return Write(Data.c_str(), Data.length());
	}

	/**
	* CStorage::ReadLineS - reads a string from a storage until newline or EOF symbol, whichever comes first, and stores it in Buffer.
	*/

	bool CStorage::ReadLineS(string &Buffer)
	{
		if (!Good())
			return false;
		
		Buffer.clear();

		byte b = 0;

		// theoretically (not tested) makes processing somewhat faster on average strings, but consumes more memory..
		Buffer.reserve(READ_BUFFER_DEFAULT_SIZE);

		while (!Eof())
		{
			if (!ReadByte(&b))
				return false;

			if (b == 0)	// that's needed, because std::string is buggy in handling \0 in strings...
				break;

			Buffer += b;

			if (b == 10)
				break;
		}

		return true;
	}

	/**
	* CStorage::WriteLine - writes Data string in a storage, terminated by newline symbols, according to current platform.
	*/

	bool CStorage::WriteLine(const char *Data)
	{
		if (Data == NULL)
			return false;

		return WriteLine(Data, strlen(Data));
	}

	/*! \copydoc WriteLine(const char *) */

	bool CStorage::WriteLine(const string &Data)
	{
		return WriteLine(Data.c_str(), Data.length());
	}

	/**
	* CStorage::SetContent - writes Data string in a storage from its beginning.
	*
	* Cursor position is set to the end of write, so, obviously, affects other writing operations.
	*/

	bool CStorage::SetContent(const string &Data)
	{
		Rewind();
		return WriteText(Data);
	}

	bool CStorage::Rewind()
	{
		return Seek(0L, SEEK_ORIGIN_BEGINNING);
	}

	bool CStorage::Flush()
	{
		if (!Good())
			return false;

		return true;
	}

	/**
	* CStorage::WriteLine - internal version of WriteLine to take out common code from versions with std::string and null-terrminated string.
	*/

	bool CStorage::WriteLine(const char *Data, size_t Size)
	{
		if (!Good() || Data == NULL)
			return false;

		Write(Data, Size);
		WriteText(Environment::GetLineTerminator());

		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	// CFile

	CFile::CFile() : File(NULL)
	{
	}

	CFile::CFile(const string &AFilename, EOpenMode AMode) : File(NULL)
	{
		Open(AFilename, AMode);
	}

	CFile::~CFile()
	{
		Close();
	}

	bool CFile::Open(const string &AFilename, EOpenMode AMode)
	{
		if (Good())
		{
			Log("ERROR", "Can't open file '%s': another file is already opened", AFilename.c_str());
			return false;
		}

		if (AFilename.empty())
		{
			Log("ERROR", "Can't open file with empty filename");
			return false;
		}

		Filename = AFilename;

		switch (AMode)
		{
		case OPEN_MODE_READ:
			File = fopen(Filename.c_str(), "rb");
			break;
		case OPEN_MODE_WRITE:
			File = fopen(Filename.c_str(), "wb");
			break;
		}

		if (!Good())
		{
			Log("ERROR", "Can't open file '%s'", Filename.c_str());
			return false;
		}

		return true;
	}

	bool CFile::Close()
	{
		if (!Good())
			return false;

		fclose(File);
		File = NULL;

		Filename.clear();

		return true;
	}

	/**
	* CFile::Read - reads BytesCount bytes ElementsCount times from a file and stores them in memory at address Buffer.
	*/

	bool CFile::Read(void *Buffer, size_t BytesCount, size_t ElementsCount /*= 1*/)
	{
		if (!Good() || Buffer == NULL || BytesCount == 0 || ElementsCount == 0)
			return false;

		if (fread(Buffer, BytesCount, ElementsCount, File) != ElementsCount && !Eof())
		{
			Log("ERROR", "File IO error occured in attempt to read data from '%s'", Filename.c_str());
			return false;
		}

		return true;
	}

	/**
	* CFile::Write - writes ElementsCount elements size of BytesCount bytes each from array at address Data to a file.
	*/

	bool CFile::Write(const void *Data, size_t BytesCount, size_t ElementsCount /*= 1*/)
	{
		if (!Good() || Data == NULL || BytesCount == 0 || ElementsCount == 0)
			return false;

		if (fwrite(Data, BytesCount, ElementsCount, File) != ElementsCount)
		{
			Log("ERROR", "File IO error occured in attempt to write data to '%s'", Filename.c_str());
			return false;
		}

		return true;
	}

	/**
	* CFile::ReadLine - reads a string terminated by newline symbol and stores it in memory at address Buffer.
	* Reading finishes at reaching ASize symbol, newline or EOF symbol, whichvever comes first.
	* 
	* \param[out] Buffer pointer to memory, where a string will be stored
	* \param ASize maximum number of symbols to read
	*/

	bool CFile::ReadLine(char *Buffer, int ASize)
	{
		if (!Good() || Buffer == NULL || ASize == 0)
			return false;

		if (fgets(Buffer, ASize, File) == NULL)
		{
			Log("ERROR", "File IO error occurend in attempt to read line from '%s'", Filename.c_str());
			return false;
		}

		return true;
	}

	/**
	* CFile::GetContent - returns all file content in string.
	*
	* Doesn't affect cursor position for other reading operations.
	*/

	string CFile::GetContent()
	{
		if (!Good())
			return "";

		string result;
		if (result.max_size() < Size())
		{
			Log("ERROR", "File '%s' is too large to be stored in single string", Filename.c_str());
			return "";
		}

		fpos_t OldPos;
		fgetpos(File, &OldPos);
		rewind(File);

		string tmp;

		while (!Eof())
		{
			ReadLineS(tmp);
			result += tmp;
		}

		fsetpos(File, &OldPos);

		return result;
	}


	/**
	* CFile::Seek - moves the internal current position pointer to specified place.
	*
	* \param Offset offset in bytes
	* \param Origin shows from what place offset is calculated
	*/

	bool CFile::Seek(long Offset, ESeekOrigin Origin)
	{
		if (!Good())
			return false;

		int origin_const;

		switch (Origin)
		{
		case SEEK_ORIGIN_BEGINNING:
			origin_const = SEEK_SET;
			break;
		case SEEK_ORIGIN_CURRENT:
			origin_const = SEEK_CUR;
			break;
		case SEEK_ORIGIN_END:
			origin_const = SEEK_END;
			break;
		}

		return (fseek(File, Offset, origin_const) == 0);
	}

	bool CFile::Flush()
	{
		if (!Good())
			return false;

		return (fflush(File) == 0);
	}

	bool CFile::Eof() const
	{
		if (!Good())
			return true;

		return (feof(File) != 0);
	}

	bool CFile::Good() const
	{
		return (File != NULL);
	}

	unsigned long CFile::Size() const
	{
		struct stat FileStat;

		if (stat(Filename.c_str(), &FileStat))
		{
			Log("ERROR", "Can't get size of '%s'", Filename.c_str());
			return 0;
		}

		return FileStat.st_size;
	}

	//////////////////////////////////////////////////////////////////////////
	// CMemory

	CMemory::CMemory() : BeginningPointer(NULL), CurrentPointer(NULL), Length(0)
	{
	}

	CMemory::CMemory(byte *ABeginningPointer, long ALength, EOpenMode AMode) : BeginningPointer(NULL), CurrentPointer(NULL), Length(0)
	{
		Open(ABeginningPointer, ALength, AMode);
	}

	CMemory::~CMemory()
	{
		Close();
	}

	bool CMemory::Open(byte *ABeginningPointer, long ALength, EOpenMode AMode)
	{
		if (Good())
		{
			Log("ERROR", "Can't open memory storage at address '%p': another storage is already opened at address '%p'", ABeginningPointer, BeginningPointer);
			return false;
		}

		if (ABeginningPointer == NULL)
		{
			Log("ERROR", "Can't open memory storage at null address");
			return false;
		}

		if (ALength == 0)
		{
			Log("ERROR", "Can't open memory storage with zero length");
			return false;
		}

		BeginningPointer = ABeginningPointer;
		CurrentPointer = BeginningPointer;
		Length = ALength;
		Mode = AMode;

		return true;
	}

	bool CMemory::Close()
	{
		if (!Good())
			return false;

		BeginningPointer = NULL;
		CurrentPointer = NULL;
		Length = 0;

		return true;
	}

	/**
	* CMemory::Read - reads BytesCount bytes ElementsCount times from a memory storage and stores them in memory at address Buffer.
	*/

	bool CMemory::Read(void *Buffer, size_t BytesCount, size_t ElementsCount /*= 1*/)
	{
		if (!Good() || Buffer == NULL || BytesCount == 0 || ElementsCount == 0)
			return false;
		
		int n = min(Length - (CurrentPointer - BeginningPointer), static_cast<unsigned long>(BytesCount * ElementsCount));
		/*if (n == 0)
			return false;*/

		memcpy(Buffer, CurrentPointer, n);

		CurrentPointer += n;

		return true;
	}

	/**
	* CMemory::Write - writes ElementsCount elements size of BytesCount bytes each from array at address Data to a memory storage.
	*/

	bool CMemory::Write(const void *Data, size_t BytesCount, size_t ElementsCount /*= 1*/)
	{
		if (!Good() || Data == NULL || BytesCount == 0 || ElementsCount == 0)
			return false;

		// maybe add mode check..

		int n = min(Length - (CurrentPointer - BeginningPointer), static_cast<unsigned long>(BytesCount * ElementsCount));

		memcpy(CurrentPointer, Data, n);

		CurrentPointer += n;

		return true;
	}

	/**
	* CMemory::ReadLine - reads a string terminated by newline symbol and stores it in memory at address Buffer.
	* Reading finishes at reaching ASize symbol, newline or EOF symbol, whichvever comes first.
	* 
	* \param[out] Buffer pointer to memory, where a string will be stored
	* \param ASize maximum number of symbols to read
	*/

	bool CMemory::ReadLine(char *Buffer, int ASize)
	{
		if (!Good() || Buffer == NULL || ASize == 0)
			return false;
		
		int ReadCount = 0;

		while (!Eof() && (ReadCount < ASize))
		{
			Buffer[ReadCount] = *CurrentPointer;

			ReadCount++;
			CurrentPointer++;

			if (*(CurrentPointer - 1) == '\n')
				break;
		}

		Buffer[ReadCount] = 0;

		return true;
	}

	/**
	* CMemory::GetContent - returns all memory storage content in string.
	*
	* Doesn't affect cursor position for other reading operations.
	*/

	string CMemory::GetContent()
	{
		if (!Good())
			return "";

		string result;
		if (result.max_size() < Size())
		{
			Log("ERROR", "Memory storage at address '%p' is too large to be stored in single string", BeginningPointer);
			return "";
		}

		byte *OldPos = CurrentPointer;

		CurrentPointer = BeginningPointer;

		string tmp;

		while (!Eof())
		{
			ReadLineS(tmp);
			result += tmp;
		}

		CurrentPointer = OldPos;

		return result;
	}


	/**
	* CMemory::Seek - moves the internal current position pointer to specified place.
	*
	* \param Offset offset in bytes
	* \param Origin shows from what place offset is calculated
	*/

	bool CMemory::Seek(long Offset, ESeekOrigin Origin)
	{
		if (!Good())
			return false;

		switch (Origin)
		{
		case SEEK_ORIGIN_BEGINNING:
			CurrentPointer = BeginningPointer + Offset;
			break;
		case SEEK_ORIGIN_CURRENT:
			CurrentPointer += Offset;
			break;
		case SEEK_ORIGIN_END:
			// maybe disallow this...
			CurrentPointer = BeginningPointer + Length + Offset;
			break;
		}

		return true;
	}

	bool CMemory::Eof() const
	{
		if (!Good())
			return true;

		return (CurrentPointer >= (BeginningPointer + Length));
	}

	bool CMemory::Good() const
	{
		return (BeginningPointer != NULL);
	}

	unsigned long CMemory::Size() const
	{
		return Length;
	}

};	// namespace Deku2D
