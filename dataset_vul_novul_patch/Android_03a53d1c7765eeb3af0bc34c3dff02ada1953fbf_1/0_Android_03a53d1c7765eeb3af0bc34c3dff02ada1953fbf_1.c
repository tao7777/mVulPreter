 bool InputWindowInfo::frameContainsPoint(int32_t x, int32_t y) const {
    return x >= frameLeft && x < frameRight
            && y >= frameTop && y < frameBottom;
 }
