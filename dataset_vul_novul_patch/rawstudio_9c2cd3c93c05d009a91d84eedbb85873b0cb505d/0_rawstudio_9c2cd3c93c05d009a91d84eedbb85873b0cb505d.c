 rs_filter_graph(RSFilter *filter)
 {
 	g_return_if_fail(RS_IS_FILTER(filter));
	gchar *dot_filename;
	gchar *png_filename;
	gchar *command_line;
 	GString *str = g_string_new("digraph G {\n");
 
 	rs_filter_graph_helper(str, filter);
 
 	g_string_append_printf(str, "}\n");
 
	/* Here we would like to use g_mkdtemp(), but due to a bug in upstream, that's impossible */
	dot_filename = g_strdup_printf("/tmp/rs-filter-graph.%u", g_random_int());
	png_filename = g_strdup_printf("%s.%u.png", dot_filename, g_random_int());

	g_file_set_contents(dot_filename, str->str, str->len, NULL);

	command_line = g_strdup_printf("dot -Tpng >%s <%s", png_filename, dot_filename);
	if (0 != system(command_line))
 		g_warning("Calling dot failed");
	g_free(command_line);

	command_line = g_strdup_printf("gnome-open %s", png_filename);
	if (0 != system(command_line))
 		g_warning("Calling gnome-open failed.");
	g_free(command_line);
 
	g_free(dot_filename);
	g_free(png_filename);
 	g_string_free(str, TRUE);
 }
