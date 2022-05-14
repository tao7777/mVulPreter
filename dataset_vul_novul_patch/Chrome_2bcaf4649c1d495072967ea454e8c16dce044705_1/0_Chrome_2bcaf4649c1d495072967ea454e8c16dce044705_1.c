std::string HttpUtil::AssembleRawHeaders(const char* input_begin,
                                         int input_len) {
  std::string raw_headers;
  raw_headers.reserve(input_len);

  const char* input_end = input_begin + input_len;

  int status_begin_offset = LocateStartOfStatusLine(input_begin, input_len);
  if (status_begin_offset != -1)
    input_begin += status_begin_offset;

  const char* status_line_end = FindStatusLineEnd(input_begin, input_end);
  raw_headers.append(input_begin, status_line_end);


  CStringTokenizer lines(status_line_end, input_end, "\r\n");

  bool prev_line_continuable = false;

  while (lines.GetNext()) {
    const char* line_begin = lines.token_begin();
    const char* line_end = lines.token_end();

    if (prev_line_continuable && IsLWS(*line_begin)) {
      raw_headers.push_back(' ');
       raw_headers.append(FindFirstNonLWS(line_begin, line_end), line_end);
     } else {
      raw_headers.push_back('\n');
 
       raw_headers.append(line_begin, line_end);

      prev_line_continuable = IsLineSegmentContinuable(line_begin, line_end);
     }
   }
 
  raw_headers.append("\n\n", 2);

  // Use '\0' as the canonical line terminator. If the input already contained
  // any embeded '\0' characters we will strip them first to avoid interpreting
  // them as line breaks.
  raw_headers.erase(std::remove(raw_headers.begin(), raw_headers.end(), '\0'),
                    raw_headers.end());
  std::replace(raw_headers.begin(), raw_headers.end(), '\n', '\0');

   return raw_headers;
 }
