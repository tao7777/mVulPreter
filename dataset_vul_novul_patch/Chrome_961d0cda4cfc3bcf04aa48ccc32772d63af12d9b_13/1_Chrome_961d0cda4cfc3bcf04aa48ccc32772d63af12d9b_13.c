void RecordGenerationDialogDismissal(bool accepted) {
  UMA_HISTOGRAM_BOOLEAN("KeyboardAccessory.GeneratedPasswordDialog", accepted);
}
