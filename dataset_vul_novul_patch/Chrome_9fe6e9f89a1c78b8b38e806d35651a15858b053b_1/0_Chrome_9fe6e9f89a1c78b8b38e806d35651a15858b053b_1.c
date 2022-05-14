 void ParamTraits<SkBitmap>::Log(const SkBitmap& p, std::string* l) {
   l->append("<SkBitmap>");
  LogParam(p.info(), l);
 }
