#ifndef _MINIZ_CPP_GZIP_HPP_
#define _MINIZ_CPP_GZIP_HPP_

#if defined (_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined (_MSC_VER) && (_MSC_VER >= 1200)

#include <array>
#include <istream>
#include <ostream>

namespace miniz {

  class gnu_zip {
    public:
      using CharT = unsigned char;
      static constexpr size_t INPUT_BUFFER_SIZE = 1024 * 512;
      // must be a power of 2 and >= TINFL_LZ_DICT_SIZE
      static constexpr size_t OUTPUT_BUFFER_SIZE = 1024 * 512;
 
      void decompress(std::istream& input, std::ostream& output);

    private:
      std::array<CharT,INPUT_BUFFER_SIZE> input_buffer_;
      std::array<CharT,OUTPUT_BUFFER_SIZE> output_buffer_;
  }; // class gnu_zip

  using gzip = gnu_zip;

} // namespace miniz

#endif // _MINIZ_CPP_GZIP_HPP_
