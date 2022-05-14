static INLINE BOOL zgfx_GetBits(ZGFX_CONTEXT* _zgfx, UINT32 _nbits)
{
	if (!_zgfx)
		return FALSE;

	while (_zgfx->cBitsCurrent < _nbits)
	{
		_zgfx->BitsCurrent <<= 8;

		if (_zgfx->pbInputCurrent < _zgfx->pbInputEnd)
			_zgfx->BitsCurrent += *(_zgfx->pbInputCurrent)++;

		_zgfx->cBitsCurrent += 8;
	}

	_zgfx->cBitsRemaining -= _nbits;
 	_zgfx->cBitsCurrent -= _nbits;
 	_zgfx->bits = _zgfx->BitsCurrent >> _zgfx->cBitsCurrent;
 	_zgfx->BitsCurrent &= ((1 << _zgfx->cBitsCurrent) - 1);
	return TRUE;
 }
