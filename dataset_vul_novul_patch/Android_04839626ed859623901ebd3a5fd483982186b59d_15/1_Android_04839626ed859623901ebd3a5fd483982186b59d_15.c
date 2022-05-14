int Track::Info::Copy(Info& dst) const
{
    if (&dst == this)
        return 0;
 
    dst.type = type;
    dst.number = number;
    dst.defaultDuration = defaultDuration;
    dst.codecDelay = codecDelay;
    dst.seekPreRoll = seekPreRoll;
    dst.uid = uid;
    dst.lacing = lacing;
    dst.settings = settings;
 
 
    if (int status = CopyStr(&Info::nameAsUTF8, dst))
        return status;
 
    if (int status = CopyStr(&Info::language, dst))
        return status;
 
    if (int status = CopyStr(&Info::codecId, dst))
        return status;
 
    if (int status = CopyStr(&Info::codecNameAsUTF8, dst))
        return status;
 
    if (codecPrivateSize > 0)
    {
        if (codecPrivate == NULL)
            return -1;
 
        if (dst.codecPrivate)
            return -1;
 
        if (dst.codecPrivateSize != 0)
            return -1;
 
        dst.codecPrivate = new (std::nothrow) unsigned char[codecPrivateSize];
 
        if (dst.codecPrivate == NULL)
            return -1;
 
        memcpy(dst.codecPrivate, codecPrivate, codecPrivateSize);
        dst.codecPrivateSize = codecPrivateSize;
     }
 
    return 0;
}
