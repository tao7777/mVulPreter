void * adminchild(struct clientparam* param) {
 int i, res;
 char * buf;
 char username[256];
  char *sb;
  char *req = NULL;
  struct printparam pp;
 unsigned contentlen = 0;
  int isform = 0;
 
  pp.inbuf = 0;
 pp.cp = param;

 buf = myalloc(LINESIZE);
 if(!buf) {RETURN(555);}
 i = sockgetlinebuf(param, CLIENT, (unsigned char *)buf, LINESIZE - 1, '\n', conf.timeouts[STRING_S]);
 if(i<5 || ((buf[0]!='G' || buf[1]!='E' || buf[2]!='T' || buf[3]!=' ' || buf[4]!='/') && 
	   (buf[0]!='P' || buf[1]!='O' || buf[2]!='S' || buf[3]!='T' || buf[4]!=' ' || buf[5]!='/')))
 {
	RETURN(701);
 }
 buf[i] = 0;
 sb = strchr(buf+5, ' ');
 if(!sb){
	RETURN(702);
 }
 *sb = 0;
 req = mystrdup(buf + ((*buf == 'P')? 6 : 5));
 while((i = sockgetlinebuf(param, CLIENT, (unsigned char *)buf, LINESIZE - 1, '\n', conf.timeouts[STRING_S])) > 2){
	buf[i] = 0;
	if(i > 19 && (!strncasecmp(buf, "authorization", 13))){
		sb = strchr(buf, ':');
		if(!sb)continue;
		++sb;
		while(isspace(*sb))sb++;
		if(!*sb || strncasecmp(sb, "basic", 5)){
			continue;
		}
		sb+=5;
		while(isspace(*sb))sb++;
		i = de64((unsigned char *)sb, (unsigned char *)username, 255);
		if(i<=0)continue;
		username[i] = 0;
		sb = strchr((char *)username, ':');
		if(sb){
			*sb = 0;
			if(param->password)myfree(param->password);
			param->password = (unsigned char *)mystrdup(sb+1);
		}
		if(param->username) myfree(param->username);
		param->username = (unsigned char *)mystrdup(username);
		continue;
	}
 	else if(i > 15 && (!strncasecmp(buf, "content-length:", 15))){
 		sb = buf + 15;
 		while(isspace(*sb))sb++;
		sscanf(sb, "%u", &contentlen);
		if(contentlen > LINESIZE*1024) contentlen = 0;
 	}
 	else if(i > 13 && (!strncasecmp(buf, "content-type:", 13))){
 		sb = buf + 13;
		while(isspace(*sb))sb++;
		if(!strncasecmp(sb, "x-www-form-urlencoded", 21)) isform = 1;
	}
 }
 param->operation = ADMIN;
 if(isform && contentlen) {
	printstr(&pp, "HTTP/1.0 100 Continue\r\n\r\n");
	stdpr(&pp, NULL, 0);
 }
 res = (*param->srv->authfunc)(param);
 if(res && res != 10) {
	printstr(&pp, authreq);
	RETURN(res);
 }
 if(param->srv->singlepacket || param->redirected){
	if(*req == 'C') req[1] = 0;
	else *req = 0;
 }
 sprintf(buf, ok, conf.stringtable?(char *)conf.stringtable[2]:"3proxy", conf.stringtable?(char *)conf.stringtable[2]:"3[APA3A] tiny proxy", conf.stringtable?(char *)conf.stringtable[3]:"");
 if(*req != 'S') printstr(&pp, buf);
 switch(*req){
	case 'C':
		printstr(&pp, counters);
		{
			struct trafcount *cp; 
			int num = 0;
			for(cp = conf.trafcounter; cp; cp = cp->next, num++){
			 int inbuf = 0;

			 if(cp->ace && (param->srv->singlepacket || param->redirected)){
				if(!ACLmatches(cp->ace, param))continue;
			 }
			 if(req[1] == 'S' && atoi(req+2) == num) cp->disabled=0;
			 if(req[1] == 'D' && atoi(req+2) == num) cp->disabled=1;
			 inbuf += sprintf(buf,	"<tr>"
						"<td>%s</td><td><A HREF=\'/C%c%d\'>%s</A></td><td>",
						(cp->comment)?cp->comment:"&nbsp;",
						(cp->disabled)?'S':'D',
						num,
						(cp->disabled)?"NO":"YES"
					);
			 if(!cp->ace || !cp->ace->users){
				inbuf += sprintf(buf+inbuf, "<center>ANY</center>");
			 }
			 else {
				inbuf += printuserlist(buf+inbuf, LINESIZE-800, cp->ace->users, ",<br />\r\n");
			 }
			 inbuf += sprintf(buf+inbuf, "</td><td>");
			 if(!cp->ace || !cp->ace->src){
				inbuf += sprintf(buf+inbuf, "<center>ANY</center>");
			 }
			 else {
				inbuf += printiplist(buf+inbuf, LINESIZE-512, cp->ace->src, ",<br />\r\n");
			 }
			 inbuf += sprintf(buf+inbuf, "</td><td>");
			 if(!cp->ace || !cp->ace->dst){
				inbuf += sprintf(buf+inbuf, "<center>ANY</center>");
			 }
			 else {
				inbuf += printiplist(buf+inbuf, LINESIZE-512, cp->ace->dst, ",<br />\r\n");
			 }
			 inbuf += sprintf(buf+inbuf, "</td><td>");
			 if(!cp->ace || !cp->ace->ports){
				inbuf += sprintf(buf+inbuf, "<center>ANY</center>");
			 }
			 else {
				inbuf += printportlist(buf+inbuf, LINESIZE-128, cp->ace->ports, ",<br />\r\n");
			 }
			 if(cp->type == NONE) {
			  inbuf += sprintf(buf+inbuf,	
					"</td><td colspan=\'6\' align=\'center\'>exclude from limitation</td></tr>\r\n"
				 );
			 }
			 else {
			  inbuf += sprintf(buf+inbuf,	
					"</td><td>%"PRINTF_INT64_MODIFIER"u</td>"
					"<td>MB%s</td>"
					"<td>%"PRINTF_INT64_MODIFIER"u</td>"
					"<td>%s</td>",
				 cp->traflim64 / (1024 * 1024),
				 rotations[cp->type],
				 cp->traf64,
				 cp->cleared?ctime(&cp->cleared):"never"
				);
			 inbuf += sprintf(buf + inbuf,
					"<td>%s</td>"
					"<td>%i</td>"
					"</tr>\r\n",

				 cp->updated?ctime(&cp->updated):"never",
				 cp->number
				);
			 }
			 printstr(&pp, buf);
			}

		}
		printstr(&pp, counterstail);
		break;
		
	case 'R':
		conf.needreload = 1;
		printstr(&pp, "<h3>Reload scheduled</h3>");
		break;
	case 'S':
		{
			if(req[1] == 'X'){
				printstr(&pp, style);
				break;
			}
			printstr(&pp, xml);
			printval(conf.services, TYPE_SERVER, 0, &pp);
			printstr(&pp, postxml);
		}
			break;
	case 'F':
		{
			FILE *fp;
			char buf[256];

			fp = confopen();
			if(!fp){
				printstr(&pp, "<h3><font color=\"red\">Failed to open config file</font></h3>");
 				break;
 			}
 				printstr(&pp, "<h3>Please be careful editing config file remotely</h3>");
				printstr(&pp, "<form method=\"POST\" action=\"/U\" enctype=\"application/x-www-form-urlencoded\"><textarea cols=\"80\" rows=\"30\" name=\"conffile\">");
 				while(fgets(buf, 256, fp)){
 					printstr(&pp, buf);
 				}
				if(!writable) fclose(fp);
				printstr(&pp, "</textarea><br><input type=\"Submit\"></form>");
			break;
 		}
 	case 'U':
 		{
			unsigned l=0;
 			int error = 0;
 
			if(!writable || !contentlen || fseek(writable, 0, 0)){
 				error = 1;
 			}
			while(l < contentlen && (i = sockgetlinebuf(param, CLIENT, (unsigned char *)buf, (contentlen - l) > LINESIZE - 1?LINESIZE - 1:contentlen - l, '+', conf.timeouts[STRING_S])) > 0){
 				if(i > (contentlen - l)) i = (contentlen - l);
 				if(!l){
					if(i<9 || strncasecmp(buf, "conffile=", 9)) error = 1;
 				}
 				if(!error){
					buf[i] = 0;
 					decodeurl((unsigned char *)buf, 1);
 					fprintf(writable, "%s", l? buf : buf + 9);
 				}
 				l += i;
 			}
 			if(writable && !error){
 				fflush(writable);
#ifndef _WINCE
				ftruncate(fileno(writable), ftell(writable));
#endif
			}
			printstr(&pp, error?    "<h3><font color=\"red\">Config file is not writable</font></h3>Make sure you have \"writable\" command in configuration file":
						"<h3>Configuration updated</h3>");

		}
		break;
	default:
		printstr(&pp, (char *)conf.stringtable[WEBBANNERS]);
		break;
 }
 if(*req != 'S') printstr(&pp, tail);

CLEANRET:


 printstr(&pp, NULL);
 if(buf) myfree(buf);
 (*param->srv->logfunc)(param, (unsigned char *)req);
 if(req)myfree(req);
 freeparam(param);
 return (NULL);
}
