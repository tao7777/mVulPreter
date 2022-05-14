 size_t OpenMP4SourceUDTA(char *filename)
 {
	mp4object *mp4 = (mp4object *)malloc(sizeof(mp4object));
	if (mp4 == NULL) return 0;

	memset(mp4, 0, sizeof(mp4object));

#ifdef _WINDOWS
	fopen_s(&mp4->mediafp, filename, "rb");
#else
	mp4->mediafp = fopen(filename, "rb");
#endif
 
 	if (mp4->mediafp)
 	{
		uint32_t qttag, qtsize32;
		size_t len;
 		int32_t nest = 0;
 		uint64_t nestsize[MAX_NEST_LEVEL] = { 0 };
 		uint64_t lastsize = 0, qtsize;

		do
		{
			len = fread(&qtsize32, 1, 4, mp4->mediafp);
			len += fread(&qttag, 1, 4, mp4->mediafp);
			if (len == 8)
 			{
 				if (!GPMF_VALID_FOURCC(qttag))
 				{
					LongSeek(mp4, lastsize - 8 - 8);
 
 					NESTSIZE(lastsize - 8);
 					continue;
				}

				qtsize32 = BYTESWAP32(qtsize32);

				if (qtsize32 == 1) // 64-bit Atom
				{
					fread(&qtsize, 1, 8, mp4->mediafp);
					qtsize = BYTESWAP64(qtsize) - 8;
				}
				else
					qtsize = qtsize32;

				nest++;

				if (qtsize < 8) break;
				if (nest >= MAX_NEST_LEVEL) break;

				nestsize[nest] = qtsize;
				lastsize = qtsize;

 				if (qttag == MAKEID('m', 'd', 'a', 't') ||
 					qttag == MAKEID('f', 't', 'y', 'p'))
 				{
					LongSeek(mp4, qtsize - 8);
 					NESTSIZE(qtsize);
 					continue;
 				}

				if (qttag == MAKEID('G', 'P', 'M', 'F'))
				{
					mp4->videolength += 1.0;
					mp4->metadatalength += 1.0;

					mp4->indexcount = (int)mp4->metadatalength;

					mp4->metasizes = (uint32_t *)malloc(mp4->indexcount * 4 + 4);  memset(mp4->metasizes, 0, mp4->indexcount * 4 + 4);
					mp4->metaoffsets = (uint64_t *)malloc(mp4->indexcount * 8 + 8);  memset(mp4->metaoffsets, 0, mp4->indexcount * 8 + 8);

					mp4->metasizes[0] = (int)qtsize - 8;
					mp4->metaoffsets[0] = ftell(mp4->mediafp);
					mp4->metasize_count = 1;

					return (size_t)mp4;  // not an MP4, RAW GPMF which has not inherent timing, assigning a during of 1second.
				}
 				if (qttag != MAKEID('m', 'o', 'o', 'v') && //skip over all but these atoms
 					qttag != MAKEID('u', 'd', 't', 'a'))
 				{
					LongSeek(mp4, qtsize - 8);
 					NESTSIZE(qtsize);
 					continue;
 				}
				else
				{
					NESTSIZE(8);
				}
			}
		} while (len > 0);
	}
	return (size_t)mp4;
 }
