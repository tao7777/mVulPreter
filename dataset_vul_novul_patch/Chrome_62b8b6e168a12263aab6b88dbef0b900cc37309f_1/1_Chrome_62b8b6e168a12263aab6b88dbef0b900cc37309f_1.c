void PaletteTool::RegisterToolInstances(PaletteToolManager* tool_manager) {
   tool_manager->AddTool(base::MakeUnique<CaptureRegionAction>(tool_manager));
   tool_manager->AddTool(base::MakeUnique<CaptureScreenAction>(tool_manager));
   tool_manager->AddTool(base::MakeUnique<CreateNoteAction>(tool_manager));
 }
