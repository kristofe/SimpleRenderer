// redirectStreamBuf.cpp
#pragma once
//Feel free to modify target options here (for example #ifndef NDEBUG)
#ifdef _WIN32
#define ENABLE_MSVC_OUTPUT
#endif


#include <iostream>
#include <vector>
#include <sstream>

#ifdef ENABLE_MSVC_OUTPUT
#include <Windows.h>
#endif


#ifdef ENABLE_MSVC_OUTPUT
template<typename TChar, typename TTraits = std::char_traits<TChar>>
class OutputDebugStringBuf : public std::basic_stringbuf<TChar, TTraits>
{
public:
  typedef std::basic_stringbuf<TChar, TTraits> BaseClass;

  explicit OutputDebugStringBuf() : _buffer(256)
  {
    setg(nullptr, nullptr, nullptr);
    setp(_buffer.data(), _buffer.data(), _buffer.data() + _buffer.size());
  }

  static_assert(std::is_same<TChar, char>::value || std::is_same<TChar, wchar_t>::value, "OutputDebugStringBuf only supports char and wchar_t types");

  int sync() override try
  {
    MessageOutputer<TChar, TTraits>()(pbase(), pptr());
    setp(_buffer.data(), _buffer.data(), _buffer.data() + _buffer.size());
    return 0;
  }
  catch (...)
  {
    return -1;
  }

  typename BaseClass::int_type overflow(typename BaseClass::int_type c = TTraits::eof()) override
  {
    auto syncRet = sync();
    if (c != TTraits::eof())
    {
      _buffer[0] = c;
      setp(_buffer.data(), _buffer.data() + 1, _buffer.data() + _buffer.size());
    }
    return syncRet == -1 ? TTraits::eof() : 0;
  }


private:
  std::vector<TChar> _buffer;

  template<typename TChar, typename TTraits>
  struct MessageOutputer;

  template<>
  struct MessageOutputer<char, std::char_traits<char>>
  {
    template<typename TIterator>
    void operator()(TIterator begin, TIterator end) const
    {
      std::string s(begin, end);
      OutputDebugStringA(s.c_str());
    }
  };

  template<>
  struct MessageOutputer<wchar_t, std::char_traits<wchar_t>>
  {
    template<typename TIterator>
    void operator()(TIterator begin, TIterator end) const
    {
      std::wstring s(begin, end);
      OutputDebugStringW(s.c_str());
    }
  };
};
#endif


void RedirectStdoutToMSVC()
{
#ifdef ENABLE_MSVC_OUTPUT
  static OutputDebugStringBuf<char> outputDebugBufChar;
  static OutputDebugStringBuf<wchar_t> outputDebugBufWChar;

  std::cout.rdbuf(&outputDebugBufChar);
  std::cerr.rdbuf(&outputDebugBufChar);
  std::clog.rdbuf(&outputDebugBufChar);

  std::wcout.rdbuf(&outputDebugBufWChar);
  std::wcerr.rdbuf(&outputDebugBufWChar);
  std::wclog.rdbuf(&outputDebugBufWChar);
#endif
}