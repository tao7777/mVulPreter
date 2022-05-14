static void ProcessExifDir(unsigned char * DirStart, unsigned char * OffsetBase,
 unsigned ExifLength, int NestingLevel)
{
 int de;
 int a;
 int NumDirEntries;
 unsigned ThumbnailOffset = 0;
 unsigned ThumbnailSize = 0;
 char IndentString[25];

    printf("ProcessExifDir");
 if (NestingLevel > 4){
 ErrNonfatal("Maximum directory nesting exceeded (corrupt exif header)", 0,0);
 return;
 }

    memset(IndentString, ' ', 25);
 IndentString[NestingLevel * 4] = '\0';


 NumDirEntries = Get16u(DirStart);
 #define DIR_ENTRY_ADDR(Start, Entry) (Start+2+12*(Entry))

 {
 unsigned char * DirEnd;
 DirEnd = DIR_ENTRY_ADDR(DirStart, NumDirEntries);
 if (DirEnd+4 > (OffsetBase+ExifLength)){
 if (DirEnd+2 == OffsetBase+ExifLength || DirEnd == OffsetBase+ExifLength){
 }else{
 ErrNonfatal("Illegally sized exif subdirectory (%d entries)",NumDirEntries,0);
 return;
 }
 }
 if (DumpExifMap){
            printf("Map: %05d-%05d: Directory\n",(int)(DirStart-OffsetBase), (int)(DirEnd+4-OffsetBase));
 }


 }

 if (ShowTags){
        printf("(dir has %d entries)\n",NumDirEntries);
 }

 for (de=0;de<NumDirEntries;de++){
 int Tag, Format, Components;
 unsigned char * ValuePtr;
 int ByteCount;
 unsigned char * DirEntry;
 DirEntry = DIR_ENTRY_ADDR(DirStart, de);

 Tag = Get16u(DirEntry);
 Format = Get16u(DirEntry+2);
 Components = Get32u(DirEntry+4);

 if ((Format-1) >= NUM_FORMATS) {
 ErrNonfatal("Illegal number format %d for tag %04x", Format, Tag);
 continue;
 }

 if ((unsigned)Components > 0x10000){
 ErrNonfatal("Illegal number of components %d for tag %04x", Components, Tag);
 continue;
 }

 ByteCount = Components * BytesPerFormat[Format];

 if (ByteCount > 4){

             unsigned OffsetVal;
             OffsetVal = Get32u(DirEntry+8);
            if (OffsetVal > UINT32_MAX - ByteCount || OffsetVal+ByteCount > ExifLength){
                 ErrNonfatal("Illegal value pointer for tag %04x", Tag,0);
                 continue;
 }
 ValuePtr = OffsetBase+OffsetVal;

 if (OffsetVal > ImageInfo.LargestExifOffset){
 ImageInfo.LargestExifOffset = OffsetVal;
 }

 if (DumpExifMap){
                printf("Map: %05d-%05d:   Data for tag %04x\n",OffsetVal, OffsetVal+ByteCount, Tag);
 }
 }else{
 ValuePtr = DirEntry+8;
 }

 if (Tag == TAG_MAKER_NOTE){
 if (ShowTags){
                printf("%s    Maker note: ",IndentString);
 }
 ProcessMakerNote(ValuePtr, ByteCount, OffsetBase, ExifLength);
 continue;
 }

 if (ShowTags){
 for (a=0;;a++){
 if (a >= (int)TAG_TABLE_SIZE){
                    printf("%s", IndentString);
                    printf("    Unknown Tag %04x Value = ", Tag);
 break;
 }
 if (TagTable[a].Tag == Tag){
                    printf("%s", IndentString);
                    printf("    %s = ",TagTable[a].Desc);
 break;
 }
 }

 switch(Format){
 case FMT_BYTE:
 if(ByteCount>1){
                        printf("%.*ls\n", ByteCount/2, (wchar_t *)ValuePtr);
 }else{
 PrintFormatNumber(ValuePtr, Format, ByteCount);
                        printf("\n");
 }
 break;

 case FMT_UNDEFINED:

 case FMT_STRING:
 {
                          printf("\"%s\"", ValuePtr);
 }
 break;

 default:
 PrintFormatNumber(ValuePtr, Format, ByteCount);
                    printf("\n");
 }
 }

 switch(Tag){

 case TAG_MAKE:
                strncpy(ImageInfo.CameraMake, (char *)ValuePtr, ByteCount < 31 ? ByteCount : 31);
 break;

 case TAG_MODEL:
                strncpy(ImageInfo.CameraModel, (char *)ValuePtr, ByteCount < 39 ? ByteCount : 39);
 break;

 case TAG_SUBSEC_TIME:
                strlcpy(ImageInfo.SubSecTime, (char *)ValuePtr, sizeof(ImageInfo.SubSecTime));
 break;

 case TAG_SUBSEC_TIME_ORIG:
                strlcpy(ImageInfo.SubSecTimeOrig, (char *)ValuePtr,
 sizeof(ImageInfo.SubSecTimeOrig));
 break;

 case TAG_SUBSEC_TIME_DIG:
                strlcpy(ImageInfo.SubSecTimeDig, (char *)ValuePtr,
 sizeof(ImageInfo.SubSecTimeDig));
 break;

 case TAG_DATETIME_DIGITIZED:
                strlcpy(ImageInfo.DigitizedTime, (char *)ValuePtr,
 sizeof(ImageInfo.DigitizedTime));

 if (ImageInfo.numDateTimeTags >= MAX_DATE_COPIES){
 ErrNonfatal("More than %d date fields!  This is nuts", MAX_DATE_COPIES, 0);
 break;
 }
 ImageInfo.DateTimeOffsets[ImageInfo.numDateTimeTags++] =
 (char *)ValuePtr - (char *)OffsetBase;
 break;

 case TAG_DATETIME_ORIGINAL:
                strncpy(ImageInfo.DateTime, (char *)ValuePtr, 19);

 case TAG_DATETIME:
 if (!isdigit(ImageInfo.DateTime[0])){
                    strncpy(ImageInfo.DateTime, (char *)ValuePtr, 19);
 }

 if (ImageInfo.numDateTimeTags >= MAX_DATE_COPIES){
 ErrNonfatal("More than %d date fields!  This is nuts", MAX_DATE_COPIES, 0);
 break;
 }
 ImageInfo.DateTimeOffsets[ImageInfo.numDateTimeTags++] =
 (char *)ValuePtr - (char *)OffsetBase;
 break;

 case TAG_WINXP_COMMENT:
 if (ImageInfo.Comments[0]){ // We already have a jpeg comment.
 if (ShowTags) printf("Windows XP commend and other comment in header\n");
 break; // Already have a windows comment, skip this one.
 }

 if (ByteCount > 1){
 if (ByteCount > MAX_COMMENT_SIZE) ByteCount = MAX_COMMENT_SIZE;
                    memcpy(ImageInfo.Comments, ValuePtr, ByteCount);
 ImageInfo.CommentWidchars = ByteCount/2;
 }
 break;

 case TAG_USERCOMMENT:
 if (ImageInfo.Comments[0]){ // We already have a jpeg comment.
 if (ShowTags) printf("Multiple comments in exif header\n");
 break; // Already have a windows comment, skip this one.
 }

 for (a=ByteCount;;){
                    a--;
 if ((ValuePtr)[a] == ' '){
 (ValuePtr)[a] = '\0';
 }else{
 break;
 }
 if (a == 0) break;
 }

 {
 int msiz = ExifLength - (ValuePtr-OffsetBase);
 if (msiz > ByteCount) msiz = ByteCount;
 if (msiz > MAX_COMMENT_SIZE - 1) msiz = MAX_COMMENT_SIZE - 1;
 if (msiz > 5 && memcmp(ValuePtr, "ASCII", 5) == 0) {
 for (a = 5; a < 10 && a < msiz; a++) {
 int c = (ValuePtr)[a];
 if (c != '\0' && c != ' ') {
                                strncpy(ImageInfo.Comments,
 (char *)ValuePtr + a, msiz - a);
 break;
 }
 }
 } else {
                        strncpy(ImageInfo.Comments, (char *)ValuePtr, msiz);
 }
 }
 break;

 case TAG_FNUMBER:
 ImageInfo.ApertureFNumber = (float)ConvertAnyFormat(ValuePtr, Format);
 break;

 case TAG_APERTURE:
 case TAG_MAXAPERTURE:
 if (ImageInfo.ApertureFNumber == 0){
 ImageInfo.ApertureFNumber
 = (float)exp(ConvertAnyFormat(ValuePtr, Format)*log(2)*0.5);
 }
 break;

 case TAG_FOCALLENGTH:
 ImageInfo.FocalLength.num = Get32u(ValuePtr);
 ImageInfo.FocalLength.denom = Get32u(4+(char *)ValuePtr);
 break;

 case TAG_SUBJECT_DISTANCE:
 ImageInfo.Distance = (float)ConvertAnyFormat(ValuePtr, Format);
 break;

 case TAG_EXPOSURETIME:
 ImageInfo.ExposureTime = (float)ConvertAnyFormat(ValuePtr, Format);
 break;

 case TAG_SHUTTERSPEED:
 if (ImageInfo.ExposureTime == 0){
 ImageInfo.ExposureTime
 = (float)(1/exp(ConvertAnyFormat(ValuePtr, Format)*log(2)));
 }
 break;


 case TAG_FLASH:
 ImageInfo.FlashUsed=(int)ConvertAnyFormat(ValuePtr, Format);
 break;

 case TAG_ORIENTATION:
 if (NumOrientations >= 2){
 ErrNonfatal("More than two orientation tags!",0,0);
 break;
 }
 OrientationPtr[NumOrientations] = ValuePtr;
 OrientationNumFormat[NumOrientations] = Format;
 if (NumOrientations == 0){
 ImageInfo.Orientation = (int)ConvertAnyFormat(ValuePtr, Format);
 }
 if (ImageInfo.Orientation < 0 || ImageInfo.Orientation > 8){
 ErrNonfatal("Undefined rotation value %d", ImageInfo.Orientation, 0);
 ImageInfo.Orientation = 0;
 }
 NumOrientations += 1;
 break;

 case TAG_EXIF_IMAGELENGTH:
 case TAG_EXIF_IMAGEWIDTH:
                a = (int)ConvertAnyFormat(ValuePtr, Format);
 if (ExifImageWidth < a) ExifImageWidth = a;
 break;

 case TAG_FOCAL_PLANE_XRES:
 FocalplaneXRes = ConvertAnyFormat(ValuePtr, Format);
 break;

 case TAG_FOCAL_PLANE_UNITS:
 switch((int)ConvertAnyFormat(ValuePtr, Format)){
 case 1: FocalplaneUnits = 25.4; break; // inch
 case 2:
 FocalplaneUnits = 25.4;
 break;

 case 3: FocalplaneUnits = 10; break; // centimeter
 case 4: FocalplaneUnits = 1; break; // millimeter
 case 5: FocalplaneUnits = .001; break; // micrometer
 }
 break;

 case TAG_EXPOSURE_BIAS:
 ImageInfo.ExposureBias = (float)ConvertAnyFormat(ValuePtr, Format);
 break;

 case TAG_WHITEBALANCE:
 ImageInfo.Whitebalance = (int)ConvertAnyFormat(ValuePtr, Format);
 break;

 case TAG_LIGHT_SOURCE:
 ImageInfo.LightSource = (int)ConvertAnyFormat(ValuePtr, Format);
 break;

 case TAG_METERING_MODE:
 ImageInfo.MeteringMode = (int)ConvertAnyFormat(ValuePtr, Format);
 break;

 case TAG_EXPOSURE_PROGRAM:
 ImageInfo.ExposureProgram = (int)ConvertAnyFormat(ValuePtr, Format);
 break;

 case TAG_EXPOSURE_INDEX:
 if (ImageInfo.ISOequivalent == 0){
 ImageInfo.ISOequivalent = (int)ConvertAnyFormat(ValuePtr, Format);
 }
 break;

 case TAG_EXPOSURE_MODE:
 ImageInfo.ExposureMode = (int)ConvertAnyFormat(ValuePtr, Format);
 break;

 case TAG_ISO_EQUIVALENT:
 ImageInfo.ISOequivalent = (int)ConvertAnyFormat(ValuePtr, Format);
 break;

 case TAG_DIGITALZOOMRATIO:
 ImageInfo.DigitalZoomRatio = (float)ConvertAnyFormat(ValuePtr, Format);
 break;

 case TAG_THUMBNAIL_OFFSET:
 ThumbnailOffset = (unsigned)ConvertAnyFormat(ValuePtr, Format);
 DirWithThumbnailPtrs = DirStart;
 break;

 case TAG_THUMBNAIL_LENGTH:
 ThumbnailSize = (unsigned)ConvertAnyFormat(ValuePtr, Format);
 ImageInfo.ThumbnailSizeOffset = ValuePtr-OffsetBase;
 break;

 case TAG_EXIF_OFFSET:
 if (ShowTags) printf("%s    Exif Dir:",IndentString);

 case TAG_INTEROP_OFFSET:
 if (Tag == TAG_INTEROP_OFFSET && ShowTags) printf("%s    Interop Dir:",IndentString);
 {
 unsigned char * SubdirStart;
 SubdirStart = OffsetBase + Get32u(ValuePtr);
 if (SubdirStart < OffsetBase || SubdirStart > OffsetBase+ExifLength){
 ErrNonfatal("Illegal exif or interop ofset directory link",0,0);
 }else{
 ProcessExifDir(SubdirStart, OffsetBase, ExifLength, NestingLevel+1);
 }
 continue;
 }
 break;

 case TAG_GPSINFO:
 if (ShowTags) printf("%s    GPS info dir:",IndentString);
 {
 unsigned char * SubdirStart;
 SubdirStart = OffsetBase + Get32u(ValuePtr);
 if (SubdirStart < OffsetBase || SubdirStart > OffsetBase+ExifLength){
 ErrNonfatal("Illegal GPS directory link",0,0);
 }else{
 ProcessGpsInfo(SubdirStart, ByteCount, OffsetBase, ExifLength);
 }
 continue;
 }
 break;

 case TAG_FOCALLENGTH_35MM:
 ImageInfo.FocalLength35mmEquiv = (unsigned)ConvertAnyFormat(ValuePtr, Format);
 break;

 case TAG_DISTANCE_RANGE:
 ImageInfo.DistanceRange = (int)ConvertAnyFormat(ValuePtr, Format);
 break;
 }
 }


 {
 unsigned char * SubdirStart;
 unsigned Offset;

 if (DIR_ENTRY_ADDR(DirStart, NumDirEntries) + 4 <= OffsetBase+ExifLength){
            printf("DirStart %p offset from dirstart %d", DirStart, 2+12*NumDirEntries);
 Offset = Get32u(DirStart+2+12*NumDirEntries);
 if (Offset){
 SubdirStart = OffsetBase + Offset;
 if (SubdirStart > OffsetBase+ExifLength || SubdirStart < OffsetBase){
                    printf("SubdirStart %p OffsetBase %p ExifLength %d Offset %d",
 SubdirStart, OffsetBase, ExifLength, Offset);
 if (SubdirStart > OffsetBase && SubdirStart < OffsetBase+ExifLength+20){
 if (ShowTags) printf("Thumbnail removed with Jhead 1.3 or earlier\n");
 }else{
 ErrNonfatal("Illegal subdirectory link",0,0);
 }
 }else{
 if (SubdirStart <= OffsetBase+ExifLength){
 if (ShowTags) printf("%s    Continued directory ",IndentString);
 ProcessExifDir(SubdirStart, OffsetBase, ExifLength, NestingLevel+1);
 }
 }
 if (Offset > ImageInfo.LargestExifOffset){
 ImageInfo.LargestExifOffset = Offset;
 }
 }
 }else{
 }
 }

 if (ThumbnailOffset){
 ImageInfo.ThumbnailAtEnd = FALSE;

 if (DumpExifMap){
            printf("Map: %05d-%05d: Thumbnail\n",ThumbnailOffset, ThumbnailOffset+ThumbnailSize);
 }

 if (ThumbnailOffset <= ExifLength){
 if (ThumbnailSize > ExifLength-ThumbnailOffset){
 ThumbnailSize = ExifLength-ThumbnailOffset;
 if (ShowTags) printf("Thumbnail incorrectly placed in header\n");

 }
 ImageInfo.ThumbnailOffset = ThumbnailOffset;
 ImageInfo.ThumbnailSize = ThumbnailSize;

 if (ShowTags){
                printf("Thumbnail size: %d bytes\n",ThumbnailSize);
 }
 }
 }
    printf("returning from ProcessExifDir");
}
