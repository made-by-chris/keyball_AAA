import unreal

# /Users/basiclaser/projects/keyball/Content/Keys/vw-1
# /Users/basiclaser/projects/keyball/Content/data/DT_KeyboardData.uasset
folder_path = "/Game/Keys/vw-1"
data_table_path = "/Game/data/DT_KeyboardData"
row_name = "vw-1"

# Load the DataTable
data_table = unreal.EditorAssetLibrary.load_asset(data_table_path)
if not data_table:
    unreal.log_error(f"Could not find DataTable at {data_table_path}")
    raise Exception("Missing DataTable")

# Find all StaticMesh assets in the folder
asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()
assets = asset_registry.get_assets_by_path(folder_path, recursive=False)
static_meshes = [a.get_asset() for a in assets if a.asset_class == "StaticMesh"]

# Sort or filter if needed
static_meshes.sort(key=lambda mesh: mesh.get_name())

# Create the row struct
row_struct = unreal.StructBase()
row_struct.set_editor_property("Meshes", static_meshes)

# Insert row into DataTable
data_table.remove_row(row_name)  # Optional: clean up existing
data_table.add_row(row_name, row_struct)

unreal.log("✅ DataTable row populated with static meshes.")
