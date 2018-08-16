.. _reconstruction_system_integrate_scene:

Integrate scene
-------------------------------------

The final step of the system is to integrate all RGBD images into a single TSDF volume and extract a mesh as the result.

Input arguments
``````````````````````````````````````

.. code-block:: python

    if __name__ == "__main__":
        parser = argparse.ArgumentParser(description=
                "integrate the whole RGBD sequence using estimated camera pose")
        parser.add_argument("path_dataset", help="path to the dataset")
        parser.add_argument("-path_intrinsic",
                help="path to the RGBD camera intrinsic")
        args = parser.parse_args()

        if args.path_intrinsic:
            intrinsic = read_pinhole_camera_intrinsic(args.path_intrinsic)
        else:
            intrinsic = PinholeCameraIntrinsic(
                    PinholeCameraIntrinsicParameters.PrimeSenseDefault)
        scalable_integrate_rgb_frames(args.path_dataset, intrinsic)

The script runs with ``python integrate_scene.py [path]``. ``[path]`` should have subfolders *image* and *depth* in which frames are synchronized and aligned. The optional argument ``-path_intrinsic`` specifies path to a json file that has a camera intrinsic matrix (See :ref:`reading_camera_intrinsic` for details). If it is not given, the PrimeSense factory setting is used.

Integrate RGBD frames
``````````````````````````````````````

.. code-block:: python

    def scalable_integrate_rgb_frames(path_dataset, intrinsic):
        [color_files, depth_files] = get_rgbd_file_lists(path_dataset)
        n_files = len(color_files)
        n_frames_per_fragment = 100
        n_fragments = int(math.ceil(float(n_files) / n_frames_per_fragment))
        volume = ScalableTSDFVolume(voxel_length = 3.0 / 512.0,
                sdf_trunc = 0.04, color_type = TSDFVolumeColorType.RGB8)

        pose_graph_fragment = read_pose_graph(
                path_dataset + template_global_posegraph_optimized)

        for fragment_id in range(len(pose_graph_fragment.nodes)):
            pose_graph_rgbd = read_pose_graph(path_dataset +
                    template_fragment_posegraph_optimized % fragment_id)

            for frame_id in range(len(pose_graph_rgbd.nodes)):
                frame_id_abs = fragment_id * n_frames_per_fragment + frame_id
                print("Fragment %03d / %03d :: integrate rgbd frame %d (%d of %d)."
                        % (fragment_id, n_fragments-1, frame_id_abs, frame_id+1,
                        len(pose_graph_rgbd.nodes)))
                color = read_image(color_files[frame_id_abs])
                depth = read_image(depth_files[frame_id_abs])
                rgbd = create_rgbd_image_from_color_and_depth(color, depth,
                        depth_trunc = 3.0, convert_rgb_to_intensity = False)
                pose = np.dot(pose_graph_fragment.nodes[fragment_id].pose,
                        pose_graph_rgbd.nodes[frame_id].pose)
                volume.integrate(rgbd, intrinsic, np.linalg.inv(pose))

        mesh = volume.extract_triangle_mesh()
        mesh.compute_vertex_normals()
        draw_geometries([mesh])

        mesh_name = path_dataset + template_global_mesh
        write_triangle_mesh(mesh_name, mesh, False, True)

This function first reads the alignment results from both :ref:`reconstruction_system_make_fragments` and :ref:`reconstruction_system_register_fragments`, then computes the pose of each RGBD image in the global space. After that, RGBD images are integrated using :ref:`rgbd_integration`.


Results
``````````````````````````````````````
This is a printed log from the volume integration script.

.. code-block:: sh

    Fragment 000 / 013 :: integrate rgbd frame 0 (1 of 100).
    Fragment 000 / 013 :: integrate rgbd frame 1 (2 of 100).
    Fragment 000 / 013 :: integrate rgbd frame 2 (3 of 100).
    Fragment 000 / 013 :: integrate rgbd frame 3 (4 of 100).
    :
    Fragment 013 / 013 :: integrate rgbd frame 1360 (61 of 64).
    Fragment 013 / 013 :: integrate rgbd frame 1361 (62 of 64).
    Fragment 013 / 013 :: integrate rgbd frame 1362 (63 of 64).
    Fragment 013 / 013 :: integrate rgbd frame 1363 (64 of 64).
    Writing PLY: [========================================] 100%

The following images show final scene reconstruction.

.. image:: ../../_static/ReconstructionSystem/integrate_scene/scene.png
    :width: 500px
