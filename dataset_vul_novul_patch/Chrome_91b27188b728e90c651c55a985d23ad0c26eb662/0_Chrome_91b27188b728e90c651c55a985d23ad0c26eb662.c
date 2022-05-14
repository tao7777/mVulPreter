static inline bool base64DecodeInternal(const T* data, unsigned length, Vector<char>& out, CharacterMatchFunctionPtr shouldIgnoreCharacter, Base64DecodePolicy policy)
{
    out.clear();
    if (!length)
        return true;

    out.grow(length);
 
     unsigned equalsSignCount = 0;
     unsigned outLength = 0;
    bool hadError = false;
     for (unsigned idx = 0; idx < length; ++idx) {
         unsigned ch = data[idx];
         if (ch == '=') {
             ++equalsSignCount;
            if (policy == Base64ValidatePadding && equalsSignCount > 2) {
                hadError = true;
                break;
            }
         } else if (('0' <= ch && ch <= '9') || ('A' <= ch && ch <= 'Z') || ('a' <= ch && ch <= 'z') || ch == '+' || ch == '/') {
            if (equalsSignCount) {
                hadError = true;
                break;
            }
             out[outLength++] = base64DecMap[ch];
         } else if (!shouldIgnoreCharacter || !shouldIgnoreCharacter(ch)) {
            hadError = true;
            break;
         }
     }
 
    if (outLength < out.size())
        out.shrink(outLength);

    if (hadError)
        return false;

     if (!outLength)
         return !equalsSignCount;
 
    if (policy == Base64ValidatePadding && equalsSignCount && (outLength + equalsSignCount) % 4)
        return false;

    if ((outLength % 4) == 1)
        return false;

    outLength -= (outLength + 3) / 4;
    if (!outLength)
        return false;

    unsigned sidx = 0;
    unsigned didx = 0;
    if (outLength > 1) {
        while (didx < outLength - 2) {
            out[didx] = (((out[sidx] << 2) & 255) | ((out[sidx + 1] >> 4) & 003));
            out[didx + 1] = (((out[sidx + 1] << 4) & 255) | ((out[sidx + 2] >> 2) & 017));
            out[didx + 2] = (((out[sidx + 2] << 6) & 255) | (out[sidx + 3] & 077));
            sidx += 4;
            didx += 3;
        }
    }

    if (didx < outLength)
        out[didx] = (((out[sidx] << 2) & 255) | ((out[sidx + 1] >> 4) & 003));

    if (++didx < outLength)
        out[didx] = (((out[sidx + 1] << 4) & 255) | ((out[sidx + 2] >> 2) & 017));

    if (outLength < out.size())
        out.shrink(outLength);

    return true;
}
