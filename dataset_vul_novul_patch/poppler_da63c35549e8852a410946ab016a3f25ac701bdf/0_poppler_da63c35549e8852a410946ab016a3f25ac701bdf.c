void FoFiType1C::convertToType0(char *psName, int *codeMap, int nCodes,
				FoFiOutputFunc outputFunc,
				void *outputStream) {
  int *cidMap;
  Type1CIndex subrIdx;
  Type1CIndexVal val;
  int nCIDs;
  GooString *buf;
  Type1CEexecBuf eb;
  GBool ok;
  int fd, i, j, k;

  if (codeMap) {
    nCIDs = nCodes;
    cidMap = (int *)gmallocn(nCIDs, sizeof(int));
    for (i = 0; i < nCodes; ++i) {
      if (codeMap[i] >= 0 && codeMap[i] < nGlyphs) {
	cidMap[i] = codeMap[i];
      } else {
	cidMap[i] = -1;
      }
    }
  } else if (topDict.firstOp == 0x0c1e) {
    nCIDs = 0;
    for (i = 0; i < nGlyphs && i < charsetLength; ++i) {
      if (charset[i] >= nCIDs) {
	nCIDs = charset[i] + 1;
      }
    }
    cidMap = (int *)gmallocn(nCIDs, sizeof(int));
    for (i = 0; i < nCIDs; ++i) {
      cidMap[i] = -1;
    }
    for (i = 0; i < nGlyphs && i < charsetLength; ++i) {
      cidMap[charset[i]] = i;
    }
  } else {
    nCIDs = nGlyphs;
    cidMap = (int *)gmallocn(nCIDs, sizeof(int));
    for (i = 0; i < nCIDs; ++i) {
      cidMap[i] = i;
    }
  }

  if (privateDicts) {
    for (i = 0; i < nCIDs; i += 256) {

      fd = 0;
      if (fdSelect) {
	for (j = i==0 ? 1 : 0; j < 256 && i+j < nCIDs; ++j) {
	  if (cidMap[i+j] >= 0) {
	    fd = fdSelect[cidMap[i+j]];
	    break;
	  }
 	}
       }
 
      if (fd >= nFDs)
	continue;

       (*outputFunc)(outputStream, "16 dict begin\n", 14);
       (*outputFunc)(outputStream, "/FontName /", 11);
      delete buf;
      (*outputFunc)(outputStream, "/FontType 1 def\n", 16);
      if (privateDicts[fd].hasFontMatrix) {
	buf = GooString::format("/FontMatrix [{0:.8g} {1:.8g} {2:.8g} {3:.8g} {4:.8g} {5:.8g}] def\n",
			      privateDicts[fd].fontMatrix[0],
			      privateDicts[fd].fontMatrix[1],
			      privateDicts[fd].fontMatrix[2],
			      privateDicts[fd].fontMatrix[3],
			      privateDicts[fd].fontMatrix[4],
			      privateDicts[fd].fontMatrix[5]);
	(*outputFunc)(outputStream, buf->getCString(), buf->getLength());
	delete buf;
      } else if (topDict.hasFontMatrix) {
	(*outputFunc)(outputStream, "/FontMatrix [1 0 0 1 0 0] def\n", 30);
      } else {
	(*outputFunc)(outputStream,
		      "/FontMatrix [0.001 0 0 0.001 0 0] def\n", 38);
      }
      buf = GooString::format("/FontBBox [{0:.4g} {1:.4g} {2:.4g} {3:.4g}] def\n",
			    topDict.fontBBox[0], topDict.fontBBox[1],
			    topDict.fontBBox[2], topDict.fontBBox[3]);
      (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
      delete buf;
      buf = GooString::format("/PaintType {0:d} def\n", topDict.paintType);
      (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
      delete buf;
      if (topDict.paintType != 0) {
	buf = GooString::format("/StrokeWidth {0:.4g} def\n", topDict.strokeWidth);
	(*outputFunc)(outputStream, buf->getCString(), buf->getLength());
	delete buf;
      }
      (*outputFunc)(outputStream, "/Encoding 256 array\n", 20);
      for (j = 0; j < 256 && i+j < nCIDs; ++j) {
	buf = GooString::format("dup {0:d} /c{1:02x} put\n", j, j);
	(*outputFunc)(outputStream, buf->getCString(), buf->getLength());
	delete buf;
      }
      if (j < 256) {
	buf = GooString::format("{0:d} 1 255 {{ 1 index exch /.notdef put }} for\n",
			      j);
	(*outputFunc)(outputStream, buf->getCString(), buf->getLength());
	delete buf;
      }
      (*outputFunc)(outputStream, "readonly def\n", 13);
      (*outputFunc)(outputStream, "currentdict end\n", 16);

      (*outputFunc)(outputStream, "currentfile eexec\n", 18);
      eb.outputFunc = outputFunc;
      eb.outputStream = outputStream;
      eb.ascii = gTrue;
      eb.r1 = 55665;
      eb.line = 0;

      eexecWrite(&eb, "\x83\xca\x73\xd5");
      eexecWrite(&eb, "dup /Private 32 dict dup begin\n");
      eexecWrite(&eb, "/RD {string currentfile exch readstring pop}"
		" executeonly def\n");
      eexecWrite(&eb, "/ND {noaccess def} executeonly def\n");
      eexecWrite(&eb, "/NP {noaccess put} executeonly def\n");
      eexecWrite(&eb, "/MinFeature {16 16} def\n");
      eexecWrite(&eb, "/password 5839 def\n");
      if (privateDicts[fd].nBlueValues) {
	eexecWrite(&eb, "/BlueValues [");
	for (k = 0; k < privateDicts[fd].nBlueValues; ++k) {
	  buf = GooString::format("{0:s}{1:d}",
				k > 0 ? " " : "",
				privateDicts[fd].blueValues[k]);
	  eexecWrite(&eb, buf->getCString());
	  delete buf;
	}
	eexecWrite(&eb, "] def\n");
      }
      if (privateDicts[fd].nOtherBlues) {
	eexecWrite(&eb, "/OtherBlues [");
	for (k = 0; k < privateDicts[fd].nOtherBlues; ++k) {
	  buf = GooString::format("{0:s}{1:d}",
				k > 0 ? " " : "",
				privateDicts[fd].otherBlues[k]);
	  eexecWrite(&eb, buf->getCString());
	  delete buf;
	}
	eexecWrite(&eb, "] def\n");
      }
      if (privateDicts[fd].nFamilyBlues) {
	eexecWrite(&eb, "/FamilyBlues [");
	for (k = 0; k < privateDicts[fd].nFamilyBlues; ++k) {
	  buf = GooString::format("{0:s}{1:d}", k > 0 ? " " : "",
				privateDicts[fd].familyBlues[k]);
	  eexecWrite(&eb, buf->getCString());
	  delete buf;
	}
	eexecWrite(&eb, "] def\n");
      }
      if (privateDicts[fd].nFamilyOtherBlues) {
	eexecWrite(&eb, "/FamilyOtherBlues [");
	for (k = 0; k < privateDicts[fd].nFamilyOtherBlues; ++k) {
	  buf = GooString::format("{0:s}{1:d}", k > 0 ? " " : "",
				privateDicts[fd].familyOtherBlues[k]);
	  eexecWrite(&eb, buf->getCString());
	  delete buf;
	}
	eexecWrite(&eb, "] def\n");
      }
      if (privateDicts[fd].blueScale != 0.039625) {
	buf = GooString::format("/BlueScale {0:.4g} def\n",
			      privateDicts[fd].blueScale);
	eexecWrite(&eb, buf->getCString());
	delete buf;
      }
      if (privateDicts[fd].blueShift != 7) {
	buf = GooString::format("/BlueShift {0:d} def\n",
			      privateDicts[fd].blueShift);
	eexecWrite(&eb, buf->getCString());
	delete buf;
      }
      if (privateDicts[fd].blueFuzz != 1) {
	buf = GooString::format("/BlueFuzz {0:d} def\n",
			      privateDicts[fd].blueFuzz);
	eexecWrite(&eb, buf->getCString());
	delete buf;
      }
      if (privateDicts[fd].hasStdHW) {
	buf = GooString::format("/StdHW [{0:.4g}] def\n", privateDicts[fd].stdHW);
	eexecWrite(&eb, buf->getCString());
	delete buf;
      }
      if (privateDicts[fd].hasStdVW) {
	buf = GooString::format("/StdVW [{0:.4g}] def\n", privateDicts[fd].stdVW);
	eexecWrite(&eb, buf->getCString());
	delete buf;
      }
      if (privateDicts[fd].nStemSnapH) {
	eexecWrite(&eb, "/StemSnapH [");
	for (k = 0; k < privateDicts[fd].nStemSnapH; ++k) {
	  buf = GooString::format("{0:s}{1:.4g}",
				k > 0 ? " " : "", privateDicts[fd].stemSnapH[k]);
	  eexecWrite(&eb, buf->getCString());
	  delete buf;
	}
	eexecWrite(&eb, "] def\n");
      }
      if (privateDicts[fd].nStemSnapV) {
	eexecWrite(&eb, "/StemSnapV [");
	for (k = 0; k < privateDicts[fd].nStemSnapV; ++k) {
	  buf = GooString::format("{0:s}{1:.4g}",
				k > 0 ? " " : "", privateDicts[fd].stemSnapV[k]);
	  eexecWrite(&eb, buf->getCString());
	  delete buf;
	}
	eexecWrite(&eb, "] def\n");
      }
      if (privateDicts[fd].hasForceBold) {
	buf = GooString::format("/ForceBold {0:s} def\n",
			      privateDicts[fd].forceBold ? "true" : "false");
	eexecWrite(&eb, buf->getCString());
	delete buf;
      }
      if (privateDicts[fd].forceBoldThreshold != 0) {
	buf = GooString::format("/ForceBoldThreshold {0:.4g} def\n",
			      privateDicts[fd].forceBoldThreshold);
	eexecWrite(&eb, buf->getCString());
	delete buf;
      }
      if (privateDicts[fd].languageGroup != 0) {
	buf = GooString::format("/LanguageGroup {0:d} def\n",
			      privateDicts[fd].languageGroup);
	eexecWrite(&eb, buf->getCString());
	delete buf;
      }
      if (privateDicts[fd].expansionFactor != 0.06) {
	buf = GooString::format("/ExpansionFactor {0:.4g} def\n",
			      privateDicts[fd].expansionFactor);
	eexecWrite(&eb, buf->getCString());
	delete buf;
      }

      ok = gTrue;
      getIndex(privateDicts[fd].subrsOffset, &subrIdx, &ok);
      if (!ok) {
	subrIdx.pos = -1;
      }

      eexecWrite(&eb, "2 index /CharStrings 256 dict dup begin\n");

      ok = gTrue;
      getIndexVal(&charStringsIdx, 0, &val, &ok);
      if (ok) {
	eexecCvtGlyph(&eb, ".notdef", val.pos, val.len,
		      &subrIdx, &privateDicts[fd]);
      }

      for (j = 0; j < 256 && i+j < nCIDs; ++j) {
	if (cidMap[i+j] >= 0) {
	  ok = gTrue;
	  getIndexVal(&charStringsIdx, cidMap[i+j], &val, &ok);
	  if (ok) {
	    buf = GooString::format("c{0:02x}", j);
	    eexecCvtGlyph(&eb, buf->getCString(), val.pos, val.len,
			  &subrIdx, &privateDicts[fd]);
	    delete buf;
	  }
	}
      }
      eexecWrite(&eb, "end\n");
      eexecWrite(&eb, "end\n");
      eexecWrite(&eb, "readonly put\n");
      eexecWrite(&eb, "noaccess put\n");
      eexecWrite(&eb, "dup /FontName get exch definefont pop\n");
      eexecWrite(&eb, "mark currentfile closefile\n");

      if (eb.line > 0) {
	(*outputFunc)(outputStream, "\n", 1);
      }
      for (j = 0; j < 8; ++j) {
	(*outputFunc)(outputStream, "0000000000000000000000000000000000000000000000000000000000000000\n", 65);
      }
      (*outputFunc)(outputStream, "cleartomark\n", 12);
    }
  } else {
    error(errSyntaxError, -1, "FoFiType1C::convertToType0 without privateDicts");
  }

  (*outputFunc)(outputStream, "16 dict begin\n", 14);
  (*outputFunc)(outputStream, "/FontName /", 11);
  (*outputFunc)(outputStream, psName, strlen(psName));
  (*outputFunc)(outputStream, " def\n", 5);
  (*outputFunc)(outputStream, "/FontType 0 def\n", 16);
  if (topDict.hasFontMatrix) {
    buf = GooString::format("/FontMatrix [{0:.8g} {1:.8g} {2:.8g} {3:.8g} {4:.8g} {5:.8g}] def\n",
			  topDict.fontMatrix[0], topDict.fontMatrix[1],
			  topDict.fontMatrix[2], topDict.fontMatrix[3],
			  topDict.fontMatrix[4], topDict.fontMatrix[5]);
    (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
    delete buf;
  } else {
    (*outputFunc)(outputStream, "/FontMatrix [1 0 0 1 0 0] def\n", 30);
  }
  (*outputFunc)(outputStream, "/FMapType 2 def\n", 16);
  (*outputFunc)(outputStream, "/Encoding [\n", 12);
  for (i = 0; i < nCIDs; i += 256) {
    buf = GooString::format("{0:d}\n", i >> 8);
    (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
    delete buf;
  }
  (*outputFunc)(outputStream, "] def\n", 6);
  (*outputFunc)(outputStream, "/FDepVector [\n", 14);
  for (i = 0; i < nCIDs; i += 256) {
    (*outputFunc)(outputStream, "/", 1);
    (*outputFunc)(outputStream, psName, strlen(psName));
    buf = GooString::format("_{0:02x} findfont\n", i >> 8);
    (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
    delete buf;
  }
  (*outputFunc)(outputStream, "] def\n", 6);
  (*outputFunc)(outputStream, "FontName currentdict end definefont pop\n", 40);

  gfree(cidMap);
}
