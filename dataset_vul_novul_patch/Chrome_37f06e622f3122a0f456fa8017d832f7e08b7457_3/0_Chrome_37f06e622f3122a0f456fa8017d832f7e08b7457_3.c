 HpackDecoder::HpackDecoder(const HpackHuffmanTable& table)
     : max_string_literal_size_(kDefaultMaxStringLiteralSize),
      regular_header_seen_(false),
       huffman_table_(table) {}
