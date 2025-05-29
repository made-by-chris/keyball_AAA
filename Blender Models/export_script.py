import bpy
import os

filepath = bpy.data.filepath
directory = os.path.dirname(filepath)
k = 0.15 #scale constant

bpy.ops.transform.resize(value=(k,k,k))
bpy.ops.object.transform_apply(scale=True)
def export_all_fbx(exportFolder):
    objects = bpy.data.collections["target"].all_objects
    for object in objects:
        bpy.ops.object.select_all(action='DESELECT')
        object.select_set(state=True)

        bpy.ops.transform.resize(value=(k,k,k))
        bpy.ops.object.transform_apply(scale=True)
        
        newfile_name = os.path.join( directory , object.name + '.fbx')

        bpy.ops.export_scene.fbx(filepath=newfile_name, use_selection=True)
        
export_all_fbx("./output/")