 int SoundPool::load(int fd, int64_t offset, int64_t length, int priority __unused)
 {
     ALOGV("load: fd=%d, offset=%" PRId64 ", length=%" PRId64 ", priority=%d",
             fd, offset, length, priority);
