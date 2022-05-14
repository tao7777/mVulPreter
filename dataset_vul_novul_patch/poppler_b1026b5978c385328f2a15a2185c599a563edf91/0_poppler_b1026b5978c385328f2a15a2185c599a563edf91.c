int CCITTFaxStream::lookChar() {
  int code1, code2, code3;
  int b1i, blackPixels, i, bits;
  GBool gotEOL;

  if (buf != EOF) {
    return buf;
  }

  if (outputBits == 0) {

    if (eof) {
      return EOF;
    }

    err = gFalse;

    if (nextLine2D) {
       for (i = 0; i < columns && codingLine[i] < columns; ++i) {
 	refLine[i] = codingLine[i];
       }
      for (; i < columns + 2; ++i) {
	refLine[i] = columns;
      }
       codingLine[0] = 0;
       a0i = 0;
       b1i = 0;
      while (codingLine[a0i] < columns && !err) {
	code1 = getTwoDimCode();
	switch (code1) {
	case twoDimPass:
	  if (likely(b1i + 1 < columns + 2)) {
	    addPixels(refLine[b1i + 1], blackPixels);
	    if (refLine[b1i + 1] < columns) {
	      b1i += 2;
	    }
	  }
	  break;
	case twoDimHoriz:
	  code1 = code2 = 0;
	  if (blackPixels) {
	    do {
	      code1 += code3 = getBlackCode();
	    } while (code3 >= 64);
	    do {
	      code2 += code3 = getWhiteCode();
	    } while (code3 >= 64);
	  } else {
	    do {
	      code1 += code3 = getWhiteCode();
	    } while (code3 >= 64);
	    do {
	      code2 += code3 = getBlackCode();
	    } while (code3 >= 64);
	  }
	  addPixels(codingLine[a0i] + code1, blackPixels);
	  if (codingLine[a0i] < columns) {
	    addPixels(codingLine[a0i] + code2, blackPixels ^ 1);
	  }
	  while (refLine[b1i] <= codingLine[a0i] && refLine[b1i] < columns) {
	    b1i += 2;
	    if (unlikely(b1i > columns + 1)) {
	      error(errSyntaxError, getPos(),
		"Bad 2D code {0:04x} in CCITTFax stream", code1);
	      err = gTrue;
	      break;
	    }
	  }
	  break;
	case twoDimVertR3:
	  if (unlikely(b1i > columns + 1)) {
	    error(errSyntaxError, getPos(),
	      "Bad 2D code {0:04x} in CCITTFax stream", code1);
	    err = gTrue;
	    break;
	  }
	  addPixels(refLine[b1i] + 3, blackPixels);
	  blackPixels ^= 1;
	  if (codingLine[a0i] < columns) {
	    ++b1i;
	    while (refLine[b1i] <= codingLine[a0i] && refLine[b1i] < columns) {
	      b1i += 2;
	      if (unlikely(b1i > columns + 1)) {
		error(errSyntaxError, getPos(),
		  "Bad 2D code {0:04x} in CCITTFax stream", code1);
		err = gTrue;
		break;
	      }
	    }
	  }
	  break;
	case twoDimVertR2:
	  if (unlikely(b1i > columns + 1)) {
	    error(errSyntaxError, getPos(),
	      "Bad 2D code {0:04x} in CCITTFax stream", code1);
	    err = gTrue;
	    break;
	  }
	  addPixels(refLine[b1i] + 2, blackPixels);
	  blackPixels ^= 1;
	  if (codingLine[a0i] < columns) {
	    ++b1i;
	    while (refLine[b1i] <= codingLine[a0i] && refLine[b1i] < columns) {
	      b1i += 2;
	      if (unlikely(b1i > columns + 1)) {
		error(errSyntaxError, getPos(),
		  "Bad 2D code {0:04x} in CCITTFax stream", code1);
		err = gTrue;
		break;
	      }
	    }
	  }
	  break;
	case twoDimVertR1:
	  if (unlikely(b1i > columns + 1)) {
	    error(errSyntaxError, getPos(),
	      "Bad 2D code {0:04x} in CCITTFax stream", code1);
	    err = gTrue;
	    break;
	  }
	  addPixels(refLine[b1i] + 1, blackPixels);
	  blackPixels ^= 1;
	  if (codingLine[a0i] < columns) {
	    ++b1i;
	    while (refLine[b1i] <= codingLine[a0i] && refLine[b1i] < columns) {
	      b1i += 2;
	      if (unlikely(b1i > columns + 1)) {
		error(errSyntaxError, getPos(),
		  "Bad 2D code {0:04x} in CCITTFax stream", code1);
		err = gTrue;
		break;
	      }
	    }
	  }
	  break;
	case twoDimVert0:
	  if (unlikely(b1i > columns + 1)) {
	    error(errSyntaxError, getPos(),
	      "Bad 2D code {0:04x} in CCITTFax stream", code1);
	    err = gTrue;
	    break;
	  }
	  addPixels(refLine[b1i], blackPixels);
	  blackPixels ^= 1;
	  if (codingLine[a0i] < columns) {
	    ++b1i;
	    while (refLine[b1i] <= codingLine[a0i] && refLine[b1i] < columns) {
	      b1i += 2;
	      if (unlikely(b1i > columns + 1)) {
		error(errSyntaxError, getPos(),
		  "Bad 2D code {0:04x} in CCITTFax stream", code1);
		err = gTrue;
		break;
	      }
	    }
	  }
	  break;
	case twoDimVertL3:
	  if (unlikely(b1i > columns + 1)) {
	    error(errSyntaxError, getPos(),
	      "Bad 2D code {0:04x} in CCITTFax stream", code1);
	    err = gTrue;
	    break;
	  }
	  addPixelsNeg(refLine[b1i] - 3, blackPixels);
	  blackPixels ^= 1;
	  if (codingLine[a0i] < columns) {
	    if (b1i > 0) {
	      --b1i;
	    } else {
	      ++b1i;
	    }
	    while (refLine[b1i] <= codingLine[a0i] && refLine[b1i] < columns) {
	      b1i += 2;
	      if (unlikely(b1i > columns + 1)) {
		error(errSyntaxError, getPos(),
		  "Bad 2D code {0:04x} in CCITTFax stream", code1);
		err = gTrue;
		break;
	      }
	    }
	  }
	  break;
	case twoDimVertL2:
	  if (unlikely(b1i > columns + 1)) {
	    error(errSyntaxError, getPos(),
	      "Bad 2D code {0:04x} in CCITTFax stream", code1);
	    err = gTrue;
	    break;
	  }
	  addPixelsNeg(refLine[b1i] - 2, blackPixels);
	  blackPixels ^= 1;
	  if (codingLine[a0i] < columns) {
	    if (b1i > 0) {
	      --b1i;
	    } else {
	      ++b1i;
	    }
	    while (refLine[b1i] <= codingLine[a0i] && refLine[b1i] < columns) {
	      b1i += 2;
	      if (unlikely(b1i > columns + 1)) {
	        error(errSyntaxError, getPos(),
		  "Bad 2D code {0:04x} in CCITTFax stream", code1);
	        err = gTrue;
	        break;
	      }
	    }
	  }
	  break;
	case twoDimVertL1:
	  if (unlikely(b1i > columns + 1)) {
	    error(errSyntaxError, getPos(),
	      "Bad 2D code {0:04x} in CCITTFax stream", code1);
	    err = gTrue;
	    break;
	  }
	  addPixelsNeg(refLine[b1i] - 1, blackPixels);
	  blackPixels ^= 1;
	  if (codingLine[a0i] < columns) {
	    if (b1i > 0) {
	      --b1i;
	    } else {
	      ++b1i;
	    }
	    while (refLine[b1i] <= codingLine[a0i] && refLine[b1i] < columns) {
	      b1i += 2;
	      if (unlikely(b1i > columns + 1)) {
		error(errSyntaxError, getPos(),
		  "Bad 2D code {0:04x} in CCITTFax stream", code1);
		err = gTrue;
		break;
	      }
	    }
	  }
	  break;
	case EOF:
	  addPixels(columns, 0);
	  eof = gTrue;
	  break;
	default:
	  error(errSyntaxError, getPos(),
		"Bad 2D code {0:04x} in CCITTFax stream", code1);
	  addPixels(columns, 0);
	  err = gTrue;
	  break;
	}
      }

    } else {
      codingLine[0] = 0;
      a0i = 0;
      blackPixels = 0;
      while (codingLine[a0i] < columns) {
	code1 = 0;
	if (blackPixels) {
	  do {
	    code1 += code3 = getBlackCode();
	  } while (code3 >= 64);
	} else {
	  do {
	    code1 += code3 = getWhiteCode();
	  } while (code3 >= 64);
	}
	addPixels(codingLine[a0i] + code1, blackPixels);
	blackPixels ^= 1;
      }
    }

    gotEOL = gFalse;
    if (!endOfBlock && row == rows - 1) {
      eof = gTrue;
    } else if (endOfLine || !byteAlign) {
      code1 = lookBits(12);
      if (endOfLine) {
	while (code1 != EOF && code1 != 0x001) {
	  eatBits(1);
	  code1 = lookBits(12);
	}
      } else {
	while (code1 == 0) {
	  eatBits(1);
	  code1 = lookBits(12);
	}
      }
      if (code1 == 0x001) {
	eatBits(12);
	gotEOL = gTrue;
      }
    }

    if (byteAlign && !gotEOL) {
      inputBits &= ~7;
    }

    if (lookBits(1) == EOF) {
      eof = gTrue;
    }

    if (!eof && encoding > 0) {
      nextLine2D = !lookBits(1);
      eatBits(1);
    }

    if (endOfBlock && !endOfLine && byteAlign) {
      code1 = lookBits(24);
      if (code1 == 0x001001) {
	eatBits(12);
	gotEOL = gTrue;
      }
    }
    if (endOfBlock && gotEOL) {
      code1 = lookBits(12);
      if (code1 == 0x001) {
	eatBits(12);
	if (encoding > 0) {
	  lookBits(1);
	  eatBits(1);
	}
	if (encoding >= 0) {
	  for (i = 0; i < 4; ++i) {
	    code1 = lookBits(12);
	    if (code1 != 0x001) {
	      error(errSyntaxError, getPos(),
		    "Bad RTC code in CCITTFax stream");
	    }
	    eatBits(12);
	    if (encoding > 0) {
	      lookBits(1);
	      eatBits(1);
	    }
	  }
	}
	eof = gTrue;
      }

    } else if (err && endOfLine) {
      while (1) {
	code1 = lookBits(13);
	if (code1 == EOF) {
	  eof = gTrue;
	  return EOF;
	}
	if ((code1 >> 1) == 0x001) {
	  break;
	}
	eatBits(1);
      }
      eatBits(12); 
      if (encoding > 0) {
	eatBits(1);
	nextLine2D = !(code1 & 1);
      }
    }

    if (codingLine[0] > 0) {
      outputBits = codingLine[a0i = 0];
    } else {
      outputBits = codingLine[a0i = 1];
    }

    ++row;
  }

  if (outputBits >= 8) {
    buf = (a0i & 1) ? 0x00 : 0xff;
    outputBits -= 8;
    if (outputBits == 0 && codingLine[a0i] < columns) {
      ++a0i;
      outputBits = codingLine[a0i] - codingLine[a0i - 1];
    }
  } else {
    bits = 8;
    buf = 0;
    do {
      if (outputBits > bits) {
	buf <<= bits;
	if (!(a0i & 1)) {
	  buf |= 0xff >> (8 - bits);
	}
	outputBits -= bits;
	bits = 0;
      } else {
	buf <<= outputBits;
	if (!(a0i & 1)) {
	  buf |= 0xff >> (8 - outputBits);
	}
	bits -= outputBits;
	outputBits = 0;
	if (codingLine[a0i] < columns) {
	  ++a0i;
	  if (unlikely(a0i > columns)) {
	    error(errSyntaxError, getPos(),
	      "Bad bits {0:04x} in CCITTFax stream", bits);
	      err = gTrue;
	      break;
	  }
	  outputBits = codingLine[a0i] - codingLine[a0i - 1];
	} else if (bits > 0) {
	  buf <<= bits;
	  bits = 0;
	}
      }
    } while (bits);
  }
  if (black) {
    buf ^= 0xff;
  }
  return buf;
}
