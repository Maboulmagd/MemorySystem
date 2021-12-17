//-----------------------------------------------------------------------------
// Copyright Ed Keenan 2019
//----------------------------------------------------------------------------- 
// FILE_IO  
//    v.2.1.0
//    v.2.8   - warning 5039 windows.h
//    v.2.9   - fence
//    v.2.10  - msc_ver 1914, win 10.0.17
//    v.2.11  - warning fixes 
//    v.2.12  - mutex on all trace::out
//    v.2.13  - fixed warnings on mutex and default copy const / assignment
//    v.2.14  - FileIO remove new... so you can overload new/delete
//    v.2.15  - number tests pass reformat
//    v.2.16  - fixing static analysis issues
//    v.2.17  - missing deleted copy constructor and assignment operator in macro
//    v.2.18  - teardown issue
//    v.2.19  - WinSDK, Compiler, Framework version reporting
//    v.2.20  - WinSDK, Compiler, Framework version reporting in Release mode
//    v.2.21  - x64 info header
//----------------------------------------------------------------------------- 

#ifndef FILE_IO_H
#define FILE_IO_H

class FileIO
{
public:
	static void Open(const char * const firstName, const char * const lastName) noexcept;
	static void Close() noexcept;
	static FILE *GetHandle() noexcept;

private:
	// big six
	constexpr FileIO() noexcept;
	~FileIO();
	FileIO(FileIO &&) = delete;
	FileIO(const FileIO &) = delete;
	FileIO & operator=(const FileIO &) = delete;
	FileIO & operator=(FileIO &&) = delete;

	// methods
	static FileIO	*privGetInstance() noexcept;
	void			privClose() noexcept;
	void			privOpen(const char * const firstName, const char * const lastName) noexcept;
	FILE			*privGetHandle() noexcept;

	// data
	FILE *pFileHandle;
};

#endif 

// ---  End of File ---------------
