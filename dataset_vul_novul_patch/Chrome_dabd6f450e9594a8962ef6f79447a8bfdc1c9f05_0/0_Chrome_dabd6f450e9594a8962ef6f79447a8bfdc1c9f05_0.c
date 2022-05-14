 void Label::SizeToFit(int max_width) {
   DCHECK(is_multi_line_);
 
  std::vector<string16> lines;
  base::SplitString(text_, '\n', &lines);
 
   int label_width = 0;
  for (std::vector<string16>::const_iterator iter = lines.begin();
        iter != lines.end(); ++iter) {
    label_width = std::max(label_width, font_.GetStringWidth(*iter));
   }
 
   label_width += GetInsets().width();

  if (max_width > 0)
    label_width = std::min(label_width, max_width);

  SetBounds(x(), y(), label_width, 0);
  SizeToPreferredSize();
}
