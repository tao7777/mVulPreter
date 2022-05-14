 void Label::SizeToFit(int max_width) {
   DCHECK(is_multi_line_);
 
  std::vector<std::wstring> lines;
  base::SplitString(UTF16ToWideHack(text_), L'\n', &lines);
 
   int label_width = 0;
  for (std::vector<std::wstring>::const_iterator iter = lines.begin();
        iter != lines.end(); ++iter) {
    label_width = std::max(label_width,
                           font_.GetStringWidth(WideToUTF16Hack(*iter)));
   }
 
   label_width += GetInsets().width();

  if (max_width > 0)
    label_width = std::min(label_width, max_width);

  SetBounds(x(), y(), label_width, 0);
  SizeToPreferredSize();
}
