int main(int argc, char *argv[])
{
    time_t timep;
    struct tm *timeptr;
    char *now;

    if (argc < 3) {
        send_error(1);
        return -1;
    } else if (argc > 3) {
        send_error(5);
        return -1;
    }

    build_needs_escape();

    if (argv[2] == NULL)
        index_directory(argv[1], argv[1]);
    else
        index_directory(argv[1], argv[2]);

    time(&timep);
#ifdef USE_LOCALTIME
    timeptr = localtime(&timep);
#else
     timeptr = gmtime(&timep);
 #endif
     now = strdup(asctime(timeptr));
	if (!now) {
		return -1;
	}
     now[strlen(now) - 1] = '\0';
 #ifdef USE_LOCALTIME
     printf("</table>\n<hr noshade>\nIndex generated %s %s\n"
           "<!-- This program is part of the Boa Webserver Copyright (C) 1991-2002 http://www.boa.org -->\n"
           "</body>\n</html>\n", now, TIMEZONE(timeptr));
#else
    printf("</table>\n<hr noshade>\nIndex generated %s UTC\n"
            "<!-- This program is part of the Boa Webserver Copyright (C) 1991-2002 http://www.boa.org -->\n"
            "</body>\n</html>\n", now);
 #endif
	free(now);
     return 0;
 }
