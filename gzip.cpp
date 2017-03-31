#include "gzip.hpp"

#include <stdexcept>
#include <limits>
#include <functional>
#include <vector>
#include <iterator>
#include <cstring>

// unused stuff
#define MINIZ_NO_STDIO
#define MINIZ_NO_ARCHIVE_APIS
#define MINIZ_NO_TIME
#define MINIZ_NO_MALLOC
#include "miniz.h"

namespace miniz {

  void gnu_zip::decompress(std::istream& input,  std::ostream& output) {
    input.seekg(0, std::ios_base::end);
    auto input_size = input.tellg();
    input.seekg(0);

    // TODO: Must improve the error management: error_code + better exceptions
    if (input_size < 0 || input_size > std::numeric_limits<int>::max()) {
      throw std::runtime_error("input file is to large");
    }

    // TODO: must be relplace with some buffered read.
    std::vector<CharT> input_buffer(input_size);
    input.read(reinterpret_cast<char*>(&input_buffer[0]), input_size);


    // TODO: no gz file spec checks. Should be implemented
    size_t skip = 10;
    if (input_buffer[3] & 0x4) {
      skip+=2;
    }
    // TODO: original file name
    if (input_buffer[3] & 0x8) {
      while(input_buffer[skip] != '\0') {
        skip++;
      }
      skip++;
    }
    if (input_buffer[3] & 0x10) {
      while(input_buffer[skip] != '\0') {
        skip++;
      }
      skip++;
    }
    if (input_buffer[3] & 0x2) {
      skip+=2;
    }

    size_t inbuf_size = input_size;
    inbuf_size -= skip;

    //TODO: This object must be wrapped
    z_stream stream_data;
    std::memset(&stream_data, 0, sizeof(z_stream));

    inflateInit2(&stream_data, -MZ_DEFAULT_WINDOW_BITS);
    stream_data.avail_out = output_buffer_.size();
    stream_data.avail_in = inbuf_size;
    stream_data.next_in = &(input_buffer[skip]);
    stream_data.next_out = output_buffer_.data();

    while(true) {
      // TODO: should be using some low level calls instead?
      auto ret = mz_inflate(&stream_data, MZ_SYNC_FLUSH);
      if (stream_data.avail_out == 0 && stream_data.avail_in != 0) {
        output.write(reinterpret_cast<char*>(output_buffer_.data()), output_buffer_.size());
        stream_data.next_out = output_buffer_.data();
        stream_data.avail_out = output_buffer_.size();
      }
      if (ret == MZ_BUF_ERROR) {
        throw std::runtime_error("MZ_BUF_ERROR");
      } else if (ret == MZ_PARAM_ERROR) {
        throw std::runtime_error("MZ_PARAM_ERROR");
      } else if (ret == MZ_DATA_ERROR) {
        throw std::runtime_error("MZ_DATA_ERROR");
      } else if (ret == MZ_STREAM_END) {
        output.write(reinterpret_cast<char*>(output_buffer_.data()), output_buffer_.size() - stream_data.avail_out);
        break;
      }
    }
    if (inflateEnd(&stream_data) != Z_OK) {
        throw std::runtime_error("failed to close the stream");
    }
  }

} // namespace miniz;
