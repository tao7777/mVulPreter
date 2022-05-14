bool TradQT_Manager::ParseCachedBoxes ( const MOOV_Manager & moovMgr )
{
	MOOV_Manager::BoxInfo udtaInfo;
	MOOV_Manager::BoxRef  udtaRef = moovMgr.GetBox ( "moov/udta", &udtaInfo );
	if ( udtaRef == 0 ) return false;

	for ( XMP_Uns32 i = 0; i < udtaInfo.childCount; ++i ) {

		MOOV_Manager::BoxInfo currInfo;
		MOOV_Manager::BoxRef  currRef = moovMgr.GetNthChild ( udtaRef, i, &currInfo );
		if ( currRef == 0 ) break;	// Sanity check, should not happen.
		if ( (currInfo.boxType >> 24) != 0xA9 ) continue;
		if ( currInfo.contentSize < 2+2+1 ) continue;	// Want enough for a non-empty value.
		
		InfoMapPos newInfo = this->parsedBoxes.insert ( this->parsedBoxes.end(),
														InfoMap::value_type ( currInfo.boxType, ParsedBoxInfo ( currInfo.boxType ) ) );
		std::vector<ValueInfo> * newValues = &newInfo->second.values;
		
		XMP_Uns8 * boxPtr = (XMP_Uns8*) currInfo.content;
		XMP_Uns8 * boxEnd = boxPtr + currInfo.contentSize;
		XMP_Uns16 miniLen, macLang;

		for ( ; boxPtr < boxEnd-4; boxPtr += miniLen ) {
 
 			miniLen = 4 + GetUns16BE ( boxPtr );	// ! Include header in local miniLen.
 			macLang  = GetUns16BE ( boxPtr+2);
			if ( (miniLen <= 4) || (miniLen > (boxEnd - boxPtr)) )
				break;	// Ignore bad or empty values.
 			
 			XMP_StringPtr valuePtr = (char*)(boxPtr+4);
 			size_t valueLen = miniLen - 4;
			newValues->push_back ( ValueInfo() );
			ValueInfo * newValue = &newValues->back();
			
			
			newValue->macLang = macLang;
			if ( IsMacLangKnown ( macLang ) ) newValue->xmpLang = GetXMPLang ( macLang );
			newValue->macValue.assign ( valuePtr, valueLen );

		}

	}
	
	return (! this->parsedBoxes.empty());

}	// TradQT_Manager::ParseCachedBoxes
