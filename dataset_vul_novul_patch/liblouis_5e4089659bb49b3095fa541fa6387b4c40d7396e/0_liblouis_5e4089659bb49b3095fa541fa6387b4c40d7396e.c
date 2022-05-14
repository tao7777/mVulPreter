matchCurrentInput(
 		const InString *input, int pos, const widechar *passInstructions, int passIC) {
 	int k;
 	int kk = pos;
	for (k = passIC + 2;
			((k < passIC + 2 + passInstructions[passIC + 1]) && (kk < input->length));
			k++)
 		if (input->chars[kk] == ENDSEGMENT || passInstructions[k] != input->chars[kk++])
 			return 0;
 	return 1;
}
