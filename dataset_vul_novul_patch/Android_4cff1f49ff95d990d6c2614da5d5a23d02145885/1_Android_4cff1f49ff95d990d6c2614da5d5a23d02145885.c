static jlong Region_createFromParcel(JNIEnv* env, jobject clazz, jobject parcel)
{
 if (parcel == NULL) {
 return NULL;
 }

 
     android::Parcel* p = android::parcelForJavaObject(env, parcel);
 
     SkRegion* region = new SkRegion;
    size_t size = p->readInt32();
    region->readFromMemory(p->readInplace(size), size);
 
     return reinterpret_cast<jlong>(region);
 }
