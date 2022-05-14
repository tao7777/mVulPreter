void handle_pxe_menu(cmd_tbl_t *cmdtp, struct pxe_menu *cfg)
{
	void *choice;
	struct menu *m;
	int err;

#ifdef CONFIG_CMD_BMP
 	/* display BMP if available */
 	if (cfg->bmp) {
 		if (get_relfile(cmdtp, cfg->bmp, image_load_addr)) {
			if (CONFIG_IS_ENABLED(CMD_CLS))
				run_command("cls", 0);
 			bmp_display(image_load_addr,
 				    BMP_ALIGN_CENTER, BMP_ALIGN_CENTER);
 		} else {
			printf("Skipping background bmp %s for failure\n",
			       cfg->bmp);
		}
	}
#endif

	m = pxe_menu_to_menu(cfg);
	if (!m)
		return;

	err = menu_get_choice(m, &choice);

	menu_destroy(m);

	/*
	 * err == 1 means we got a choice back from menu_get_choice.
	 *
	 * err == -ENOENT if the menu was setup to select the default but no
	 * default was set. in that case, we should continue trying to boot
	 * labels that haven't been attempted yet.
	 *
	 * otherwise, the user interrupted or there was some other error and
	 * we give up.
	 */

	if (err == 1) {
		err = label_boot(cmdtp, choice);
		if (!err)
			return;
	} else if (err != -ENOENT) {
		return;
	}

	boot_unattempted_labels(cmdtp, cfg);
}
