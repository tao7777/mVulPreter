void PostScript_MetaHandler::ParsePSFile()
{
	bool     found = false;
	IOBuffer ioBuf;

	XMP_IO* fileRef = this->parent->ioRef;

	XMP_AbortProc abortProc  = this->parent->abortProc;
	void *        abortArg   = this->parent->abortArg;
	const bool    checkAbort = (abortProc != 0);

	if ( ! PostScript_Support::IsValidPSFile(fileRef,this->fileformat) ) return ; 

	fileRef->Rewind();
	if ( ! CheckFileSpace ( fileRef, &ioBuf, 4 ) ) return ;
	XMP_Uns32 fileheader = GetUns32BE ( ioBuf.ptr );

	if ( fileheader == 0xC5D0D3C6 ) 
	{

		if ( ! CheckFileSpace ( fileRef, &ioBuf, 30 ) ) return ;

		XMP_Uns32 psOffset = GetUns32LE ( ioBuf.ptr+4 );	// PostScript offset.
		XMP_Uns32 psLength = GetUns32LE ( ioBuf.ptr+8 );	// PostScript length.

		setTokenInfo(kPS_EndPostScript,psOffset+psLength,0);
		MoveToOffset ( fileRef, psOffset, &ioBuf );

	}

	while ( true ) 
	{
		if ( checkAbort && abortProc(abortArg) ) {
			XMP_Throw ( "PostScript_MetaHandler::FindPostScriptHint - User abort", kXMPErr_UserAbort );
		}

		if ( ! CheckFileSpace ( fileRef, &ioBuf, kPSContainsForString.length() ) ) return ;

		if ( (CheckFileSpace ( fileRef, &ioBuf, kPSEndCommentString.length() )&& 
				CheckBytes ( ioBuf.ptr, Uns8Ptr(kPSEndCommentString.c_str()), kPSEndCommentString.length() )
				)|| *ioBuf.ptr!='%' || !(*(ioBuf.ptr+1)>32 && *(ioBuf.ptr+1)<=126 )) // implicit endcomment check
		{
			if (CheckBytes ( ioBuf.ptr, Uns8Ptr(kPSEndCommentString.c_str()), kPSEndCommentString.length() ))
			{
				setTokenInfo(kPS_EndComments,ioBuf.filePos+ioBuf.ptr-ioBuf.data,kPSEndCommentString.length());
				ioBuf.ptr+=kPSEndCommentString.length();
			}
			else
			{
				setTokenInfo(kPS_EndComments,ioBuf.filePos+ioBuf.ptr-ioBuf.data,0);
			}
			while(true)
			{
				if ( ! CheckFileSpace ( fileRef, &ioBuf, 1 ) ) return ;
				if (! IsWhitespace (*ioBuf.ptr)) break;
				++ioBuf.ptr;
			} 
			while(true)
			{
				if ( ! CheckFileSpace ( fileRef, &ioBuf, 5 ) ) return ;
				if (CheckBytes ( ioBuf.ptr, Uns8Ptr("/DOCI"), 5 ) 
					&& CheckFileSpace ( fileRef, &ioBuf, kPSContainsDocInfoString.length() )
					&&CheckBytes ( ioBuf.ptr, Uns8Ptr(kPSContainsDocInfoString.c_str()), kPSContainsDocInfoString.length() ))
					
				{
					
					ioBuf.ptr+=kPSContainsDocInfoString.length();
					ExtractDocInfoDict(ioBuf);
				}// DOCINFO Not found in document
				else if(CheckBytes ( ioBuf.ptr, Uns8Ptr("%%Beg"), 5 ))
				{//possibly one of %%BeginProlog %%BeginSetup %%BeginBinary %%BeginData 
					XMP_Int64 begStartpos=ioBuf.filePos+ioBuf.ptr-ioBuf.data;
					ioBuf.ptr+=5;
					if (!CheckFileSpace ( fileRef, &ioBuf, 6 )) return;
					if(CheckBytes ( ioBuf.ptr, Uns8Ptr("inProl"), 6 ))
					{//%%BeginProlog
						ioBuf.ptr+=6;
						if (!CheckFileSpace ( fileRef, &ioBuf, 2 ))return;
						if(CheckBytes ( ioBuf.ptr, Uns8Ptr("og"), 2 ))
						{
							ioBuf.ptr+=2;
							setTokenInfo(kPS_BeginProlog,begStartpos,13);
						}
					}
					else if (CheckBytes ( ioBuf.ptr, Uns8Ptr("inSetu"), 6 ))
					{//%%BeginSetup 
						ioBuf.ptr+=6;
						if (!CheckFileSpace ( fileRef, &ioBuf, 1 ))return;
						if(CheckBytes ( ioBuf.ptr, Uns8Ptr("p"), 1 ))
						{
							ioBuf.ptr+=1;
							setTokenInfo(kPS_BeginSetup,begStartpos,12);
						}
					}
					else if (CheckBytes ( ioBuf.ptr, Uns8Ptr("inBina"), 6 ))
					{//%%BeginBinary
						ioBuf.ptr+=6;
						if (!CheckFileSpace ( fileRef, &ioBuf, 3 ))return;
						if(CheckBytes ( ioBuf.ptr, Uns8Ptr("ry"), 3 ))
						{
							ioBuf.ptr+=3;
							while(true)
							{
								if (!CheckFileSpace ( fileRef, &ioBuf, 12 ))return;
								if (CheckBytes ( ioBuf.ptr, Uns8Ptr("%%EndBinary"), 11 ))
								{
									ioBuf.ptr+=11;
									if (IsWhitespace(*ioBuf.ptr))
									{
										ioBuf.ptr++;
										break;
									}
								}
								++ioBuf.ptr;
							}
						}
					}
					else if (CheckBytes ( ioBuf.ptr, Uns8Ptr("inData"), 6 ))
					{//%%BeginData
						ioBuf.ptr+=6;
						if (!CheckFileSpace ( fileRef, &ioBuf, 1 ))return;
						if(CheckBytes ( ioBuf.ptr, Uns8Ptr(":"), 1 ))
						{
							while(true)
							{
								if (!CheckFileSpace ( fileRef, &ioBuf, 10 ))return;
								if (CheckBytes ( ioBuf.ptr, Uns8Ptr("%%EndData"), 9 ))
								{
									ioBuf.ptr+=9;
									if (IsWhitespace(*ioBuf.ptr))
									{
										ioBuf.ptr++;
										break;
									}
								}
								++ioBuf.ptr;
							}
						}
					}
					else if (CheckBytes ( ioBuf.ptr, Uns8Ptr("inDocu"), 6 ))
					{// %%BeginDocument
						ioBuf.ptr+=6;
						if (!CheckFileSpace ( fileRef, &ioBuf, 5 ))return;
						if(CheckBytes ( ioBuf.ptr, Uns8Ptr("ment:"), 5 ))
						{
							ioBuf.ptr+=5;
							while(true)
							{
								if (!CheckFileSpace ( fileRef, &ioBuf, 14 ))return;
								if (CheckBytes ( ioBuf.ptr, Uns8Ptr("%%EndDocument"), 13 ))
								{
									ioBuf.ptr+=13;
									if (IsWhitespace(*ioBuf.ptr))
									{
										ioBuf.ptr++;
										break;
									}
								}
								++ioBuf.ptr;
							}
						}
					}
					else if (CheckBytes ( ioBuf.ptr, Uns8Ptr("inPage"), 6 ))
					{// %%BeginPageSetup
						ioBuf.ptr+=6;
						if (!CheckFileSpace ( fileRef, &ioBuf, 5 ))return;
						if(CheckBytes ( ioBuf.ptr, Uns8Ptr("Setup"), 5 ))
						{
							ioBuf.ptr+=5;
							setTokenInfo(kPS_BeginPageSetup,begStartpos,16);
						}
					}
				}
				else if(CheckBytes ( ioBuf.ptr, Uns8Ptr("%%End"), 5 ))
				{//possibly %%EndProlog %%EndSetup %%EndPageSetup %%EndPageComments
					XMP_Int64 begStartpos=ioBuf.filePos+ioBuf.ptr-ioBuf.data;
					ioBuf.ptr+=5;
					if ( ! CheckFileSpace ( fileRef, &ioBuf, 5 ) ) return ;
					if (CheckBytes ( ioBuf.ptr, Uns8Ptr("Prolo"), 5 ))
					{// %%EndProlog
						ioBuf.ptr+=5;
						if ( ! CheckFileSpace ( fileRef, &ioBuf, 1 ) ) return ;
						if (CheckBytes ( ioBuf.ptr, Uns8Ptr("g"), 1 ))
						{
							ioBuf.ptr+=1;
							setTokenInfo(kPS_EndProlog,begStartpos,11);
						}
					}
					else if (CheckBytes ( ioBuf.ptr, Uns8Ptr("Setup"), 5 ))
					{//%%EndSetup
						ioBuf.ptr+=5;
						setTokenInfo(kPS_EndSetup,begStartpos,10);
					}
					else if (CheckBytes ( ioBuf.ptr, Uns8Ptr("PageS"), 5 ))
					{//%%EndPageSetup
						ioBuf.ptr+=5;
						if ( ! CheckFileSpace ( fileRef, &ioBuf, 4 ) ) return ;
						if (CheckBytes ( ioBuf.ptr, Uns8Ptr("etup"), 4 ))
						{
							ioBuf.ptr+=4;
							setTokenInfo(kPS_EndPageSetup,begStartpos,14);
						}
					}
					else if (CheckBytes ( ioBuf.ptr, Uns8Ptr("PageC"), 5 ))
					{//%%EndPageComments
						ioBuf.ptr+=5;
						if ( ! CheckFileSpace ( fileRef, &ioBuf, 7 ) ) return ;
						if (CheckBytes ( ioBuf.ptr, Uns8Ptr("omments"), 7 ))
						{
							ioBuf.ptr+=7;
							setTokenInfo(kPS_EndPageComments,begStartpos,17);
						}
					}
				}
				else if(CheckBytes ( ioBuf.ptr, Uns8Ptr("%%Pag"), 5 ))
				{
					XMP_Int64 begStartpos=ioBuf.filePos+ioBuf.ptr-ioBuf.data;
					ioBuf.ptr+=5;
					if ( ! CheckFileSpace ( fileRef, &ioBuf, 2 ) ) return ;
					if (CheckBytes ( ioBuf.ptr, Uns8Ptr(":"), 2 ))
					{
						ioBuf.ptr+=2;
						while(!IsNewline(*ioBuf.ptr))
						{
							if ( ! CheckFileSpace ( fileRef, &ioBuf, 1 ) ) return ;
							++ioBuf.ptr;
						}
						setTokenInfo(kPS_Page,begStartpos,ioBuf.filePos+ioBuf.ptr-ioBuf.data-begStartpos);
					}

				}
				else if(CheckBytes ( ioBuf.ptr, Uns8Ptr("%%Tra"), 5 ))
				{
					XMP_Int64 begStartpos=ioBuf.filePos+ioBuf.ptr-ioBuf.data;
					ioBuf.ptr+=5;
					if ( ! CheckFileSpace ( fileRef, &ioBuf, 4 ) ) return ;
 					if (CheckBytes ( ioBuf.ptr, Uns8Ptr("iler"), 4 ))
 					{
 						ioBuf.ptr+=4;
						while(ioBuf.ptr < ioBuf.limit &&
                                                      !IsNewline(*ioBuf.ptr))
                                                    ++ioBuf.ptr;
 						setTokenInfo(kPS_Trailer,begStartpos,ioBuf.filePos+ioBuf.ptr-ioBuf.data-begStartpos);
 					}
 				}
					ioBuf.ptr+=5;
					setTokenInfo(kPS_EOF,ioBuf.filePos+ioBuf.ptr-ioBuf.data,5);
				}
				if ( ! CheckFileSpace ( fileRef, &ioBuf, 1 ) ) return ;
				++ioBuf.ptr;
			}
			return;

		}else if (!(kPS_Creator & dscFlags) && 
			 CheckFileSpace ( fileRef, &ioBuf, kPSContainsForString.length() )&&
			CheckBytes ( ioBuf.ptr, Uns8Ptr(kPSContainsForString.c_str()), kPSContainsForString.length() ))
		{
			ioBuf.ptr+=kPSContainsForString.length();
			if ( ! ExtractDSCCommentValue(ioBuf,kPS_dscFor) ) return ;
		}
		else if (!(kPS_CreatorTool & dscFlags) &&
			 CheckFileSpace ( fileRef, &ioBuf, kPSContainsCreatorString.length() )&&
			 CheckBytes ( ioBuf.ptr, Uns8Ptr(kPSContainsCreatorString.c_str()), kPSContainsCreatorString.length() ))
		{
			ioBuf.ptr+=kPSContainsCreatorString.length();
			if ( ! ExtractDSCCommentValue(ioBuf,kPS_dscCreator) ) return ;
		}
		else if (!(kPS_CreateDate & dscFlags) &&
			 CheckFileSpace ( fileRef, &ioBuf, kPSContainsCreateDateString.length() )&&
			CheckBytes ( ioBuf.ptr, Uns8Ptr(kPSContainsCreateDateString.c_str()), kPSContainsCreateDateString.length() ))
		{
			
			ioBuf.ptr+=kPSContainsCreateDateString.length();
			if ( ! ExtractDSCCommentValue(ioBuf,kPS_dscCreateDate) ) return ;
		}
		else if (!(kPS_Title & dscFlags) &&
			 CheckFileSpace ( fileRef, &ioBuf, kPSContainsTitleString.length() )&&
			CheckBytes ( ioBuf.ptr, Uns8Ptr(kPSContainsTitleString.c_str()), kPSContainsTitleString.length() ))
		{
						
			ioBuf.ptr+=kPSContainsTitleString.length();
			if ( ! ExtractDSCCommentValue(ioBuf,kPS_dscTitle) ) return ;
		}
		else if( CheckFileSpace ( fileRef, &ioBuf, kPSContainsXMPString.length() )&&
			 (  CheckBytes ( ioBuf.ptr, Uns8Ptr(kPSContainsXMPString.c_str()), kPSContainsXMPString.length()    ) )) {

			
			XMP_Int64 containsXMPStartpos=ioBuf.filePos+ioBuf.ptr-ioBuf.data;
			ioBuf.ptr += kPSContainsXMPString.length();
			ExtractContainsXMPHint(ioBuf,containsXMPStartpos);

		}	// Found "%ADO_ContainsXMP:".
		if ( ! PostScript_Support::SkipUntilNewline(fileRef,ioBuf) ) return ;

	}	// Outer marker loop.
