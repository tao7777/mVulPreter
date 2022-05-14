TabGroupData::TabGroupData() {
  static int next_placeholder_title_number = 1;
  title_ = base::ASCIIToUTF16(
      "Group " + base::NumberToString(next_placeholder_title_number));
   ++next_placeholder_title_number;
 
   static SkRandom rand;
  color_ = rand.nextU() | 0xff000000;
 }
