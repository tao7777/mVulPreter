static jlong Region_createFromParcel(JNIEnv* env, jobject clazz, jobject parcel)
{
 if (parcel == NULL) {
 return NULL;
 }

    android::Parcel* p = android::parcelForJavaObject(env, parcel);

 const size_t size = p->readInt32();
 const void* regionData = p->readInplace(size);
 if (regionData == NULL) {

         return NULL;
     }
     SkRegion* region = new SkRegion;
    size_t actualSize = region->readFromMemory(regionData, size);

    if (size != actualSize) {
        delete region;
        return NULL;
    }
 
     return reinterpret_cast<jlong>(region);
 }
