void SplitString(const std::wstring& str,
                 wchar_t c,
                 std::vector<std::wstring>* r) {
  SplitStringT(str, c, true, r);
}
