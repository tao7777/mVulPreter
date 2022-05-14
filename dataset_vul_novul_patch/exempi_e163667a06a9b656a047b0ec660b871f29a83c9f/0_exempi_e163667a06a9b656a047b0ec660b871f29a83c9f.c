void TIFF_MetaHandler::ProcessXMP()
{

	this->processedXMP = true;	// Make sure we only come through here once.



	bool found;
	bool readOnly = ((this->parent->openFlags & kXMPFiles_OpenForUpdate) == 0);

	if ( readOnly ) {
		this->psirMgr = new PSIR_MemoryReader();
		this->iptcMgr = new IPTC_Reader();
	} else {
		this->psirMgr = new PSIR_FileWriter();
		this->iptcMgr = new IPTC_Writer();	// ! Parse it later.
	}

	TIFF_Manager & tiff = this->tiffMgr;	// Give the compiler help in recognizing non-aliases.
	PSIR_Manager & psir = *this->psirMgr;
	IPTC_Manager & iptc = *this->iptcMgr;

	TIFF_Manager::TagInfo psirInfo;
	bool havePSIR = tiff.GetTag ( kTIFF_PrimaryIFD, kTIFF_PSIR, &psirInfo );

	if ( havePSIR ) {	// ! Do the Photoshop 6 integration before other legacy analysis.
		psir.ParseMemoryResources ( psirInfo.dataPtr, psirInfo.dataLen );
		PSIR_Manager::ImgRsrcInfo buriedExif;
		found = psir.GetImgRsrc ( kPSIR_Exif, &buriedExif );
		if ( found ) {
			tiff.IntegrateFromPShop6 ( buriedExif.dataPtr, buriedExif.dataLen );
			if ( ! readOnly ) psir.DeleteImgRsrc ( kPSIR_Exif );
		}
	}

	TIFF_Manager::TagInfo iptcInfo;
	bool haveIPTC = tiff.GetTag ( kTIFF_PrimaryIFD, kTIFF_IPTC, &iptcInfo );	// The TIFF IPTC tag.
	int iptcDigestState = kDigestMatches;

	if ( haveIPTC ) {

		bool haveDigest = false;
		PSIR_Manager::ImgRsrcInfo digestInfo;
		if ( havePSIR ) haveDigest = psir.GetImgRsrc ( kPSIR_IPTCDigest, &digestInfo );
		if ( digestInfo.dataLen != 16 ) haveDigest = false;

		if ( ! haveDigest ) {

			iptcDigestState = kDigestMissing;

		} else {

 
 			iptcDigestState = PhotoDataUtils::CheckIPTCDigest ( iptcInfo.dataPtr, iptcInfo.dataLen, digestInfo.dataPtr );
			// See bug https://bugs.freedesktop.org/show_bug.cgi?id=105205
			// if iptcInfo.dataLen is 0, then there is no digest.
			if ( (iptcDigestState == kDigestDiffers) && (kTIFF_TypeSizes[iptcInfo.type] > 1) && iptcInfo.dataLen > 0 ) {
 				XMP_Uns8 * endPtr = (XMP_Uns8*)iptcInfo.dataPtr + iptcInfo.dataLen - 1;
 				XMP_Uns8 * minPtr = endPtr - kTIFF_TypeSizes[iptcInfo.type] + 1;
 				while ( (endPtr >= minPtr) && (*endPtr == 0) ) --endPtr;
				iptcDigestState = PhotoDataUtils::CheckIPTCDigest ( iptcInfo.dataPtr, unpaddedLen, digestInfo.dataPtr );
			}

		}

	}

	XMP_OptionBits options = k2XMP_FileHadExif;	// TIFF files are presumed to have Exif legacy.
	if ( haveIPTC ) options |= k2XMP_FileHadIPTC;
	if ( this->containsXMP ) options |= k2XMP_FileHadXMP;


	bool haveXMP = false;

	if ( ! this->xmpPacket.empty() ) {
		XMP_Assert ( this->containsXMP );
		XMP_StringPtr packetStr = this->xmpPacket.c_str();
		XMP_StringLen packetLen = (XMP_StringLen)this->xmpPacket.size();
		try {
			this->xmpObj.ParseFromBuffer ( packetStr, packetLen );
		} catch ( ... ) { /* Ignore parsing failures, someday we hope to get partial XMP back. */ }
		haveXMP = true;
	}


	if ( haveIPTC && (! haveXMP) && (iptcDigestState == kDigestMatches) ) iptcDigestState = kDigestMissing;
	bool parseIPTC = (iptcDigestState != kDigestMatches) || (! readOnly);
	if ( parseIPTC ) iptc.ParseMemoryDataSets ( iptcInfo.dataPtr, iptcInfo.dataLen );
	ImportPhotoData ( tiff, iptc, psir, iptcDigestState, &this->xmpObj, options );

	this->containsXMP = true;	// Assume we now have something in the XMP.

}	// TIFF_MetaHandler::ProcessXMP
