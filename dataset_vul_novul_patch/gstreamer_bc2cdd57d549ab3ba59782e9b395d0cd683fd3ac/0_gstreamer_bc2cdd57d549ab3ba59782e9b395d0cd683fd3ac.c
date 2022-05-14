UINT CSoundFile::ReadSample(MODINSTRUMENT *pIns, UINT nFlags, LPCSTR lpMemFile, DWORD dwMemLength)
 {
 	UINT len = 0, mem = pIns->nLength+6;
 
	if ((!pIns) || ((int)pIns->nLength < 4) || (!lpMemFile)) return 0;
 	if (pIns->nLength > MAX_SAMPLE_LENGTH) pIns->nLength = MAX_SAMPLE_LENGTH;
 	pIns->uFlags &= ~(CHN_16BIT|CHN_STEREO);
 	if (nFlags & RSF_16BIT)
	{
		mem *= 2;
		pIns->uFlags |= CHN_16BIT;
	}
	if (nFlags & RSF_STEREO)
	{
		mem *= 2;
		pIns->uFlags |= CHN_STEREO;
	}
	if ((pIns->pSample = AllocateSample(mem)) == NULL)
	{
		pIns->nLength = 0;
		return 0;
	}
	switch(nFlags)
	{
	case RS_PCM8U:
		{
			len = pIns->nLength;
			if (len > dwMemLength) len = pIns->nLength = dwMemLength;
			signed char *pSample = pIns->pSample;
			for (UINT j=0; j<len; j++) pSample[j] = (signed char)(lpMemFile[j] - 0x80);
		}
		break;

	case RS_PCM8D:
		{
			len = pIns->nLength;
			if (len > dwMemLength) break;
			signed char *pSample = pIns->pSample;
			const signed char *p = (const signed char *)lpMemFile;
			int delta = 0;

			for (UINT j=0; j<len; j++)
			{
				delta += p[j];
				*pSample++ = (signed char)delta;
			}
		}
		break;

	case RS_ADPCM4:
		{
			len = (pIns->nLength + 1) / 2;
			if (len > dwMemLength - 16) break;
			memcpy(CompressionTable, lpMemFile, 16);
			lpMemFile += 16;
			signed char *pSample = pIns->pSample;
			signed char delta = 0;
			for (UINT j=0; j<len; j++)
			{
				BYTE b0 = (BYTE)lpMemFile[j];
				BYTE b1 = (BYTE)(lpMemFile[j] >> 4);
				delta = (signed char)GetDeltaValue((int)delta, b0);
				pSample[0] = delta;
				delta = (signed char)GetDeltaValue((int)delta, b1);
				pSample[1] = delta;
				pSample += 2;
			}
			len += 16;
		}
		break;

	case RS_PCM16D:
		{
			len = pIns->nLength * 2;
			if (len > dwMemLength) break;
			short int *pSample = (short int *)pIns->pSample;
			short int *p = (short int *)lpMemFile;
			int delta16 = 0;
			for (UINT j=0; j<len; j+=2)
			{
				delta16 += bswapLE16(*p++);
				*pSample++ = (short int)delta16;
			}
		}
		break;

	case RS_PCM16S:
	        {
		len = pIns->nLength * 2;
		if (len <= dwMemLength) memcpy(pIns->pSample, lpMemFile, len);
			short int *pSample = (short int *)pIns->pSample;
			for (UINT j=0; j<len; j+=2)
			{
				short int s = bswapLE16(*pSample);
			        *pSample++ = s;
			}
		}
		break;

	case RS_PCM16M:
		len = pIns->nLength * 2;
		if (len > dwMemLength) len = dwMemLength & ~1;
		if (len > 1)
		{
			signed char *pSample = (signed char *)pIns->pSample;
			signed char *pSrc = (signed char *)lpMemFile;
			for (UINT j=0; j<len; j+=2)
			{
			        *((unsigned short *)(pSample+j)) = bswapBE16(*((unsigned short *)(pSrc+j)));
			}
		}
		break;

	case RS_PCM16U:
		{
			len = pIns->nLength * 2;
			if (len > dwMemLength) break;
			short int *pSample = (short int *)pIns->pSample;
			short int *pSrc = (short int *)lpMemFile;
			for (UINT j=0; j<len; j+=2) *pSample++ = bswapLE16(*(pSrc++)) - 0x8000;
		}
		break;

	case RS_STPCM16M:
		len = pIns->nLength * 2;
		if (len*2 <= dwMemLength)
		{
			signed char *pSample = (signed char *)pIns->pSample;
			signed char *pSrc = (signed char *)lpMemFile;
			for (UINT j=0; j<len; j+=2)
			{
			        *((unsigned short *)(pSample+j*2)) = bswapBE16(*((unsigned short *)(pSrc+j)));
				*((unsigned short *)(pSample+j*2+2)) = bswapBE16(*((unsigned short *)(pSrc+j+len)));
			}
			len *= 2;
		}
		break;

	case RS_STPCM8S:
	case RS_STPCM8U:
	case RS_STPCM8D:
		{
			int iadd_l = 0, iadd_r = 0;
			if (nFlags == RS_STPCM8U) { iadd_l = iadd_r = -128; }
			len = pIns->nLength;
			signed char *psrc = (signed char *)lpMemFile;
			signed char *pSample = (signed char *)pIns->pSample;
			if (len*2 > dwMemLength) break;
			for (UINT j=0; j<len; j++)
			{
				pSample[j*2] = (signed char)(psrc[0] + iadd_l);
				pSample[j*2+1] = (signed char)(psrc[len] + iadd_r);
				psrc++;
				if (nFlags == RS_STPCM8D)
				{
					iadd_l = pSample[j*2];
					iadd_r = pSample[j*2+1];
				}
			}
			len *= 2;
		}
		break;

	case RS_STPCM16S:
	case RS_STPCM16U:
	case RS_STPCM16D:
		{
			int iadd_l = 0, iadd_r = 0;
			if (nFlags == RS_STPCM16U) { iadd_l = iadd_r = -0x8000; }
			len = pIns->nLength;
			short int *psrc = (short int *)lpMemFile;
			short int *pSample = (short int *)pIns->pSample;
			if (len*4 > dwMemLength) break;
			for (UINT j=0; j<len; j++)
			{
				pSample[j*2] = (short int) (bswapLE16(psrc[0]) + iadd_l);
				pSample[j*2+1] = (short int) (bswapLE16(psrc[len]) + iadd_r);
				psrc++;
				if (nFlags == RS_STPCM16D)
				{
					iadd_l = pSample[j*2];
					iadd_r = pSample[j*2+1];
				}
			}
			len *= 4;
		}
		break;

	case RS_IT2148:
	case RS_IT21416:
	case RS_IT2158:
	case RS_IT21516:
		len = dwMemLength;
		if (len < 4) break;
		if ((nFlags == RS_IT2148) || (nFlags == RS_IT2158))
			ITUnpack8Bit(pIns->pSample, pIns->nLength, (LPBYTE)lpMemFile, dwMemLength, (nFlags == RS_IT2158));
		else
			ITUnpack16Bit(pIns->pSample, pIns->nLength, (LPBYTE)lpMemFile, dwMemLength, (nFlags == RS_IT21516));
		break;

#ifndef MODPLUG_BASIC_SUPPORT
#ifndef FASTSOUNDLIB
	case RS_STIPCM8S:
	case RS_STIPCM8U:
		{
			int iadd = 0;
			if (nFlags == RS_STIPCM8U) { iadd = -0x80; }
			len = pIns->nLength;
			if (len*2 > dwMemLength) len = dwMemLength >> 1;
			LPBYTE psrc = (LPBYTE)lpMemFile;
			LPBYTE pSample = (LPBYTE)pIns->pSample;
			for (UINT j=0; j<len; j++)
			{
				pSample[j*2] = (signed char)(psrc[0] + iadd);
				pSample[j*2+1] = (signed char)(psrc[1] + iadd);
				psrc+=2;
			}
			len *= 2;
		}
		break;

	case RS_STIPCM16S:
	case RS_STIPCM16U:
		{
			int iadd = 0;
			if (nFlags == RS_STIPCM16U) iadd = -32768;
			len = pIns->nLength;
			if (len*4 > dwMemLength) len = dwMemLength >> 2;
			short int *psrc = (short int *)lpMemFile;
			short int *pSample = (short int *)pIns->pSample;
			for (UINT j=0; j<len; j++)
			{
				pSample[j*2] = (short int)(bswapLE16(psrc[0]) + iadd);
				pSample[j*2+1] = (short int)(bswapLE16(psrc[1]) + iadd);
				psrc += 2;
			}
			len *= 4;
		}
		break;

	case RS_AMS8:
	case RS_AMS16:
		len = 9;
		if (dwMemLength > 9)
		{
			const char *psrc = lpMemFile;
			char packcharacter = lpMemFile[8], *pdest = (char *)pIns->pSample;
			len += bswapLE32(*((LPDWORD)(lpMemFile+4)));
			if (len > dwMemLength) len = dwMemLength;
			UINT dmax = pIns->nLength;
			if (pIns->uFlags & CHN_16BIT) dmax <<= 1;
			AMSUnpack(psrc+9, len-9, pdest, dmax, packcharacter);
		}
		break;

	case RS_PTM8DTO16:
		{
			len = pIns->nLength * 2;
			if (len > dwMemLength) break;
			signed char *pSample = (signed char *)pIns->pSample;
			signed char delta8 = 0;
			for (UINT j=0; j<len; j++)
			{
				delta8 += lpMemFile[j];
				*pSample++ = delta8;
			}
			WORD *pSampleW = (WORD *)pIns->pSample;
			for (UINT j=0; j<len; j+=2)   // swaparoni!
			{
				WORD s = bswapLE16(*pSampleW);
			        *pSampleW++ = s;
			}
		}
		break;

	case RS_MDL8:
	case RS_MDL16:
		len = dwMemLength;
		if (len >= 4)
		{
			LPBYTE pSample = (LPBYTE)pIns->pSample;
			LPBYTE ibuf = (LPBYTE)lpMemFile;
			DWORD bitbuf = bswapLE32(*((DWORD *)ibuf));
			UINT bitnum = 32;
			BYTE dlt = 0, lowbyte = 0;
			ibuf += 4;
			for (UINT j=0; j<pIns->nLength; j++)
			{
				BYTE hibyte;
				BYTE sign;
				if (nFlags == RS_MDL16) lowbyte = (BYTE)MDLReadBits(bitbuf, bitnum, ibuf, 8);
				sign = (BYTE)MDLReadBits(bitbuf, bitnum, ibuf, 1);
				if (MDLReadBits(bitbuf, bitnum, ibuf, 1))
				{
					hibyte = (BYTE)MDLReadBits(bitbuf, bitnum, ibuf, 3);
				} else
				{
					hibyte = 8;
					while (!MDLReadBits(bitbuf, bitnum, ibuf, 1)) hibyte += 0x10;
					hibyte += MDLReadBits(bitbuf, bitnum, ibuf, 4);
				}
				if (sign) hibyte = ~hibyte;
				dlt += hibyte;
				if (nFlags != RS_MDL16)
					pSample[j] = dlt;
				else
				{
					pSample[j<<1] = lowbyte;
					pSample[(j<<1)+1] = dlt;
				}
			}
		}
		break;

	case RS_DMF8:
	case RS_DMF16:
		len = dwMemLength;
		if (len >= 4)
		{
			UINT maxlen = pIns->nLength;
			if (pIns->uFlags & CHN_16BIT) maxlen <<= 1;
			LPBYTE ibuf = (LPBYTE)lpMemFile, ibufmax = (LPBYTE)(lpMemFile+dwMemLength);
			len = DMFUnpack((LPBYTE)pIns->pSample, ibuf, ibufmax, maxlen);
		}
		break;

#ifdef MODPLUG_TRACKER
	case RS_PCM24S:
	case RS_PCM32S:
		len = pIns->nLength * 3;
		if (nFlags == RS_PCM32S) len += pIns->nLength;
		if (len > dwMemLength) break;
		if (len > 4*8)
		{
			UINT slsize = (nFlags == RS_PCM32S) ? 4 : 3;
			LPBYTE pSrc = (LPBYTE)lpMemFile;
			LONG max = 255;
			if (nFlags == RS_PCM32S) pSrc++;
			for (UINT j=0; j<len; j+=slsize)
			{
				LONG l = ((((pSrc[j+2] << 8) + pSrc[j+1]) << 8) + pSrc[j]) << 8;
				l /= 256;
				if (l > max) max = l;
				if (-l > max) max = -l;
			}
			max = (max / 128) + 1;
			signed short *pDest = (signed short *)pIns->pSample;
			for (UINT k=0; k<len; k+=slsize)
			{
				LONG l = ((((pSrc[k+2] << 8) + pSrc[k+1]) << 8) + pSrc[k]) << 8;
				*pDest++ = (signed short)(l / max);
			}
		}
		break;

	case RS_STIPCM24S:
	case RS_STIPCM32S:
		len = pIns->nLength * 6;
		if (nFlags == RS_STIPCM32S) len += pIns->nLength * 2;
		if (len > dwMemLength) break;
		if (len > 8*8)
		{
			UINT slsize = (nFlags == RS_STIPCM32S) ? 4 : 3;
			LPBYTE pSrc = (LPBYTE)lpMemFile;
			LONG max = 255;
			if (nFlags == RS_STIPCM32S) pSrc++;
			for (UINT j=0; j<len; j+=slsize)
			{
				LONG l = ((((pSrc[j+2] << 8) + pSrc[j+1]) << 8) + pSrc[j]) << 8;
				l /= 256;
				if (l > max) max = l;
				if (-l > max) max = -l;
			}
			max = (max / 128) + 1;
			signed short *pDest = (signed short *)pIns->pSample;
			for (UINT k=0; k<len; k+=slsize)
			{
				LONG lr = ((((pSrc[k+2] << 8) + pSrc[k+1]) << 8) + pSrc[k]) << 8;
				k += slsize;
				LONG ll = ((((pSrc[k+2] << 8) + pSrc[k+1]) << 8) + pSrc[k]) << 8;
				pDest[0] = (signed short)ll;
				pDest[1] = (signed short)lr;
				pDest += 2;
			}
		}
		break;

	case RS_STIPCM16M:
		{
			len = pIns->nLength;
			if (len*4 > dwMemLength) len = dwMemLength >> 2;
			LPCBYTE psrc = (LPCBYTE)lpMemFile;
			short int *pSample = (short int *)pIns->pSample;
			for (UINT j=0; j<len; j++)
			{
				pSample[j*2] = (signed short)(((UINT)psrc[0] << 8) | (psrc[1]));
				pSample[j*2+1] = (signed short)(((UINT)psrc[2] << 8) | (psrc[3]));
				psrc += 4;
			}
			len *= 4;
		}
		break;

#endif // MODPLUG_TRACKER
#endif // !FASTSOUNDLIB
#endif // !MODPLUG_BASIC_SUPPORT

	default:
		len = pIns->nLength;
		if (len > dwMemLength) len = pIns->nLength = dwMemLength;
		memcpy(pIns->pSample, lpMemFile, len);
	}
	if (len > dwMemLength)
	{
		if (pIns->pSample)
		{
			pIns->nLength = 0;
			FreeSample(pIns->pSample);
			pIns->pSample = NULL;
		}
		return 0;
	}
	AdjustSampleLoop(pIns);
	return len;
}
