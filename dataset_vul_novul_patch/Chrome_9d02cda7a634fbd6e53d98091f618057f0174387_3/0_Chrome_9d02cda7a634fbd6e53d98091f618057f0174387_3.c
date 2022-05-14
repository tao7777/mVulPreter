 std::string::size_type CountTrailingChars(
    const std::string& input,
     const std::string::value_type trailing_chars[]) {
   const size_t last_good_char = input.find_last_not_of(trailing_chars);
   return (last_good_char == std::string::npos) ?
      input.length() : (input.length() - last_good_char - 1);
}
