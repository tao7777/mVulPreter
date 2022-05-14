static jobject Bitmap_createFromParcel(JNIEnv* env, jobject, jobject parcel) {
 if (parcel == NULL) {
 SkDebugf("-------- unparcel parcel is NULL\n");
 return NULL;
 }

    android::Parcel* p = android::parcelForJavaObject(env, parcel);

 const bool        isMutable = p->readInt32() != 0;
 const SkColorType colorType = (SkColorType)p->readInt32();
 const SkAlphaType alphaType = (SkAlphaType)p->readInt32();
 const int         width = p->readInt32();
 const int         height = p->readInt32();
 const int         rowBytes = p->readInt32();
 const int         density = p->readInt32();

 if (kN32_SkColorType != colorType &&
            kRGB_565_SkColorType != colorType &&
            kARGB_4444_SkColorType != colorType &&
            kIndex_8_SkColorType != colorType &&
            kAlpha_8_SkColorType != colorType) {
 SkDebugf("Bitmap_createFromParcel unknown colortype: %d\n", colorType);

         return NULL;
     }
 
    SkBitmap* bitmap = new SkBitmap;
 
    bitmap->setInfo(SkImageInfo::Make(width, height, colorType, alphaType), rowBytes);
 
     SkColorTable* ctable = NULL;
     if (colorType == kIndex_8_SkColorType) {
         int count = p->readInt32();
         if (count > 0) {
             size_t size = count * sizeof(SkPMColor);
             const SkPMColor* src = (const SkPMColor*)p->readInplace(size);
             ctable = new SkColorTable(src, count);
         }
     }
 
    jbyteArray buffer = GraphicsJNI::allocateJavaPixelRef(env, bitmap, ctable);
     if (NULL == buffer) {
         SkSafeUnref(ctable);
        delete bitmap;
         return NULL;
     }
 
 SkSafeUnref(ctable);

 size_t size = bitmap->getSize();

    android::Parcel::ReadableBlob blob;

     android::status_t status = p->readBlob(size, &blob);
     if (status) {
         doThrowRE(env, "Could not read bitmap from parcel blob.");
        delete bitmap;
         return NULL;
     }
 
    bitmap->lockPixels();
    memcpy(bitmap->getPixels(), blob.data(), size);
    bitmap->unlockPixels();

 
     blob.release();
 
    return GraphicsJNI::createBitmap(env, bitmap, buffer, getPremulBitmapCreateFlags(isMutable),
            NULL, NULL, density);
 }
