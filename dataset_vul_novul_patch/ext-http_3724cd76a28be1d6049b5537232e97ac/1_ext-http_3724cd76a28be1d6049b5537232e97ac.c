static const char *parse_scheme(struct parse_state *state)
{
	size_t mb;
	const char *tmp = state->ptr;

	do {
		switch (*state->ptr) {
		case ':':
			/* scheme delimiter */
			state->url.scheme = &state->buffer[0];
			state->buffer[state->offset++] = 0;
			return ++state->ptr;

		case '0': case '1': case '2': case '3': case '4': case '5': case '6':
 		case '7': case '8': case '9':
 		case '+': case '-': case '.':
 			if (state->ptr == tmp) {
				return tmp;
 			}
 			/* no break */
 		case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
		case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
		case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
		case 'V': case 'W': case 'X': case 'Y': case 'Z':
		case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
		case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
		case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
		case 'v': case 'w': case 'x': case 'y': case 'z':
			/* scheme part */
			state->buffer[state->offset++] = *state->ptr;
			break;
 
 		default:
 			if (!(mb = parse_mb(state, PARSE_SCHEME, state->ptr, state->end, tmp, 1))) {
				/* soft fail; parse path next */
				return tmp;
 			}
 			state->ptr += mb - 1;
 		}
 	} while (++state->ptr != state->end);
 
 	return state->ptr = tmp;
 }
