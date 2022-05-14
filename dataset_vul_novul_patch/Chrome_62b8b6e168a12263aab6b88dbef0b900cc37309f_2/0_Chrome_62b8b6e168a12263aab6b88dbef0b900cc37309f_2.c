 void PaletteTray::UpdateTrayIcon() {
   gfx::VectorIconId icon = palette_tool_manager_->GetActiveTrayIcon(
       palette_tool_manager_->GetActiveTool(ash::PaletteGroup::MODE));
  icon_->SetImage(CreateVectorIcon(icon, kShelfIconSize, kShelfIconColor));
 }
