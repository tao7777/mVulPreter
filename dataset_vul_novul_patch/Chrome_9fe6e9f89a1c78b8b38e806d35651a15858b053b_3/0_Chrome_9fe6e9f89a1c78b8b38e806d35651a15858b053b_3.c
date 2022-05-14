 void ParamTraits<SkBitmap>::Write(base::Pickle* m, const SkBitmap& p) {
  WriteParam(m, p.info());
   size_t pixel_size = p.computeByteSize();
   m->WriteData(reinterpret_cast<const char*>(p.getPixels()),
                static_cast<int>(pixel_size));
}
