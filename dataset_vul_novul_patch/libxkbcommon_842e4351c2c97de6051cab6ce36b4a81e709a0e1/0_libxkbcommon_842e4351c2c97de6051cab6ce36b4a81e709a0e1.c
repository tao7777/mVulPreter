lex(struct scanner *s, union lvalue *val)
{
skip_more_whitespace_and_comments:
    /* Skip spaces. */
    while (is_space(peek(s)))
        if (next(s) == '\n')
            return TOK_END_OF_LINE;

    /* Skip comments. */
    if (chr(s, '#')) {
        skip_to_eol(s);
        goto skip_more_whitespace_and_comments;
    }

    /* See if we're done. */
    if (eof(s)) return TOK_END_OF_FILE;

    /* New token. */
    s->token_line = s->line;
    s->token_column = s->column;
    s->buf_pos = 0;
 
     /* LHS Keysym. */
     if (chr(s, '<')) {
        while (peek(s) != '>' && !eol(s) && !eof(s))
             buf_append(s, next(s));
         if (!chr(s, '>')) {
             scanner_err(s, "unterminated keysym literal");
            return TOK_ERROR;
        }
        if (!buf_append(s, '\0')) {
            scanner_err(s, "keysym literal is too long");
            return TOK_ERROR;
        }
        val->string.str = s->buf;
        val->string.len = s->buf_pos;
        return TOK_LHS_KEYSYM;
    }

    /* Colon. */
    if (chr(s, ':'))
        return TOK_COLON;
    if (chr(s, '!'))
        return TOK_BANG;
    if (chr(s, '~'))
        return TOK_TILDE;

    /* String literal. */
    if (chr(s, '\"')) {
        while (!eof(s) && !eol(s) && peek(s) != '\"') {
            if (chr(s, '\\')) {
                uint8_t o;
                if (chr(s, '\\')) {
                    buf_append(s, '\\');
                }
                else if (chr(s, '"')) {
                    buf_append(s, '"');
                }
                else if (chr(s, 'x') || chr(s, 'X')) {
                    if (hex(s, &o))
                        buf_append(s, (char) o);
                    else
                        scanner_warn(s, "illegal hexadecimal escape sequence in string literal");
                }
                else if (oct(s, &o)) {
                    buf_append(s, (char) o);
                }
                else {
                    scanner_warn(s, "unknown escape sequence (%c) in string literal", peek(s));
                    /* Ignore. */
                }
            } else {
                buf_append(s, next(s));
            }
        }
        if (!chr(s, '\"')) {
            scanner_err(s, "unterminated string literal");
            return TOK_ERROR;
        }
        if (!buf_append(s, '\0')) {
            scanner_err(s, "string literal is too long");
            return TOK_ERROR;
        }
        if (!is_valid_utf8(s->buf, s->buf_pos - 1)) {
            scanner_err(s, "string literal is not a valid UTF-8 string");
            return TOK_ERROR;
        }
        val->string.str = s->buf;
        val->string.len = s->buf_pos;
        return TOK_STRING;
    }

    /* Identifier or include. */
    if (is_alpha(peek(s)) || peek(s) == '_') {
        s->buf_pos = 0;
        while (is_alnum(peek(s)) || peek(s) == '_')
            buf_append(s, next(s));
        if (!buf_append(s, '\0')) {
            scanner_err(s, "identifier is too long");
            return TOK_ERROR;
        }

        if (streq(s->buf, "include"))
            return TOK_INCLUDE;

        val->string.str = s->buf;
        val->string.len = s->buf_pos;
        return TOK_IDENT;
    }

    /* Discard rest of line. */
    skip_to_eol(s);

    scanner_err(s, "unrecognized token");
    return TOK_ERROR;
}
