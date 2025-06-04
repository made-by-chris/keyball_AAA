import bpy

# User-defined offset for numbering
offset = 0  # Change to desired starting number
base_name = "Renamed"  # Optional: consistent base name for all objects

# Get all selected objects
selected_objects = bpy.context.selected_objects

# Loop through and assign unique names using a consistent base name
for i, obj in enumerate(selected_objects):
    new_name = f"{base_name}_{i + offset}"
    obj.name = new_name


# import bpy
# import os

# # === CONFIGURATION ===
# export_path = "/Users/basiclaser/projects/keyball/Content/Keys/vw-1/"  # Change this to your desired export folder

# # Ensure the export path exists
# if not os.path.exists(export_path):
#     os.makedirs(export_path)

# # Export each selected object individually
# selected_objects = bpy.context.selected_objects

# for obj in selected_objects:
#     # Deselect all, select current object
#     bpy.ops.object.select_all(action='DESELECT')
#     obj.select_set(True)
#     bpy.context.view_layer.objects.active = obj

#     # Build export file path using the object's current name
#     export_file = os.path.join(export_path, f"{obj.name}.fbx")

#     # Export the selected object as FBX
#     bpy.ops.export_scene.fbx(filepath=export_file, use_selection=True)

# print(f"Exported {len(selected_objects)} object(s) to: {export_path}")
