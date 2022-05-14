 static void snd_usb_mixer_free(struct usb_mixer_interface *mixer)
 {
	/* kill pending URBs */
	snd_usb_mixer_disconnect(mixer);

 	kfree(mixer->id_elems);
 	if (mixer->urb) {
 		kfree(mixer->urb->transfer_buffer);
		usb_free_urb(mixer->urb);
	}
	usb_free_urb(mixer->rc_urb);
	kfree(mixer->rc_setup_packet);
	kfree(mixer);
}
