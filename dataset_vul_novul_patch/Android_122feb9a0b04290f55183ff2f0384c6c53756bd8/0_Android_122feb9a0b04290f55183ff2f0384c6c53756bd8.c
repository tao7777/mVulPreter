static jboolean enableNative(JNIEnv* env, jobject obj) {
static jboolean enableNative(JNIEnv* env, jobject obj, jboolean isGuest) {
     ALOGV("%s:",__FUNCTION__);
 
     jboolean result = JNI_FALSE;
     if (!sBluetoothInterface) return result;
    int ret = sBluetoothInterface->enable(isGuest == JNI_TRUE ? 1 : 0);
     result = (ret == BT_STATUS_SUCCESS || ret == BT_STATUS_DONE) ? JNI_TRUE : JNI_FALSE;
     return result;
 }
