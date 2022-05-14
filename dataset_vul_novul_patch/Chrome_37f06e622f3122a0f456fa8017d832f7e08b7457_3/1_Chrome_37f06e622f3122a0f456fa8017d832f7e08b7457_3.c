 HpackDecoder::HpackDecoder(const HpackHuffmanTable& table)
     : max_string_literal_size_(kDefaultMaxStringLiteralSize),
       huffman_table_(table) {}
