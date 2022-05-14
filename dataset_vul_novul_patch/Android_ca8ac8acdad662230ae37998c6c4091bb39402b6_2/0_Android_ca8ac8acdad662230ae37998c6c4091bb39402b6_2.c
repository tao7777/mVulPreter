void SparseBitSet::initFromRanges(const uint32_t* ranges, size_t nRanges) {
 if (nRanges == 0) {
        mMaxVal = 0;
        mIndices.reset();
        mBitmaps.reset();
 return;
 }
    mMaxVal = ranges[nRanges * 2 - 1];
 size_t indexSize = (mMaxVal + kPageMask) >> kLogValuesPerPage;
    mIndices.reset(new uint32_t[indexSize]);
 uint32_t nPages = calcNumPages(ranges, nRanges);
    mBitmaps.reset(new element[nPages << (kLogValuesPerPage - kLogBitsPerEl)]);
    memset(mBitmaps.get(), 0, nPages << (kLogValuesPerPage - 3));
    mZeroPageIndex = noZeroPage;
 uint32_t nonzeroPageEnd = 0;
 uint32_t currentPage = 0;

     for (size_t i = 0; i < nRanges; i++) {
         uint32_t start = ranges[i * 2];
         uint32_t end = ranges[i * 2 + 1];
        LOG_ALWAYS_FATAL_IF(end < start);  // make sure range size is nonnegative
         uint32_t startPage = start >> kLogValuesPerPage;
         uint32_t endPage = (end - 1) >> kLogValuesPerPage;
         if (startPage >= nonzeroPageEnd) {
 if (startPage > nonzeroPageEnd) {
 if (mZeroPageIndex == noZeroPage) {
                    mZeroPageIndex = (currentPage++) << (kLogValuesPerPage - kLogBitsPerEl);
 }
 for (uint32_t j = nonzeroPageEnd; j < startPage; j++) {
                    mIndices[j] = mZeroPageIndex;
 }
 }
            mIndices[startPage] = (currentPage++) << (kLogValuesPerPage - kLogBitsPerEl);
 }

 size_t index = ((currentPage - 1) << (kLogValuesPerPage - kLogBitsPerEl)) +
 ((start & kPageMask) >> kLogBitsPerEl);
 size_t nElements = (end - (start & ~kElMask) + kElMask) >> kLogBitsPerEl;
 if (nElements == 1) {
            mBitmaps[index] |= (kElAllOnes >> (start & kElMask)) &
 (kElAllOnes << ((-end) & kElMask));
 } else {
            mBitmaps[index] |= kElAllOnes >> (start & kElMask);
 for (size_t j = 1; j < nElements - 1; j++) {
                mBitmaps[index + j] = kElAllOnes;
 }
            mBitmaps[index + nElements - 1] |= kElAllOnes << ((-end) & kElMask);
 }
 for (size_t j = startPage + 1; j < endPage + 1; j++) {
            mIndices[j] = (currentPage++) << (kLogValuesPerPage - kLogBitsPerEl);
 }
        nonzeroPageEnd = endPage + 1;
 }
}
