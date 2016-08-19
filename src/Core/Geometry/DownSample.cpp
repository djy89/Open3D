// ----------------------------------------------------------------------------
// -                        Open3D: www.open3d.org                            -
// ----------------------------------------------------------------------------
// The MIT License (MIT)
//
// Copyright (c) 2015 Qianyi Zhou <Qianyi.Zhou@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------------------------------------------------------

#include "PointCloud.h"

#include <unordered_map>
#include <tuple>

#include <Core/Utility/Helper.h>
#include <Core/Utility/Console.h>

namespace three{

namespace {

typedef std::tuple<int, int, int> VoxelIndex3;

class AccumulatedPoint
{
public:
	AccumulatedPoint() :
			num_of_points(0),
			point(0.0, 0.0, 0.0),
			normal(0.0, 0.0, 0.0),
			color(0.0, 0.0, 0.0)
	{
	}
	
public:
	void AddPoint(const PointCloud &cloud, size_t index)
	{
		point += cloud.points_[index];
		if (cloud.HasNormals()) {
			normal += cloud.normals_[index];
		}
		if (cloud.HasColors()) {
			color += cloud.colors_[index];
		}
		num_of_points++;
	}

	Eigen::Vector3d GetAveragePoint()
	{
		return point / double(num_of_points);
	}

	Eigen::Vector3d GetAverageNormal()
	{
		return normal.normalized();
	}

	Eigen::Vector3d GetAverageColor()
	{
		return color / double(num_of_points);
	}

private:
	int num_of_points;
	Eigen::Vector3d point;
	Eigen::Vector3d normal;
	Eigen::Vector3d color;
};

}	// unnamed namespace

bool VoxelDownSample(const PointCloud &input_cloud, double voxel_size,
		PointCloud &output_cloud)
{
	output_cloud.Clear();
	if (input_cloud.HasPoints() == false) {
		PrintDebug("[VoxelDownSample] Input point cloud has no points.\n");
		return false;
	}
	if (voxel_size <= 0.0) {
		PrintDebug("[VoxelDownSample] voxel_size <= 0.\n");
		return false;
	}
	Eigen::Vector3d voxel_size3(voxel_size, voxel_size, voxel_size);
	Eigen::Vector3d voxel_min_bound = input_cloud.GetMinBound() - 
			voxel_size3 * 0.5;
	Eigen::Vector3d voxel_max_bound = input_cloud.GetMaxBound() +
			voxel_size3 * 0.5;
	if (voxel_size * std::numeric_limits<int>::max() < 
			(voxel_max_bound - voxel_min_bound).maxCoeff()) {
		PrintDebug("[VoxelDownSample] voxel_size is too small.\n");
		return false;
	}
	std::unordered_map<VoxelIndex3, AccumulatedPoint, 
			hash_tuple::hash<VoxelIndex3>> voxelindex_to_accpoint;
	for (size_t i = 0; i < input_cloud.points_.size(); i++) {
		Eigen::Vector3d ref_coord = (input_cloud.points_[i] - voxel_min_bound) /
				voxel_size;
		VoxelIndex3 voxel_index = std::make_tuple(
				int(floor(ref_coord(0))),
				int(floor(ref_coord(1))),
				int(floor(ref_coord(2)))
				);
		voxelindex_to_accpoint[voxel_index].AddPoint(input_cloud, i);
	}
	bool has_normals = input_cloud.HasNormals();
	bool has_colors = input_cloud.HasColors();
	for (auto accpoint : voxelindex_to_accpoint) {
		output_cloud.points_.push_back(accpoint.second.GetAveragePoint());
		if (has_normals) {
			output_cloud.normals_.push_back(accpoint.second.GetAverageNormal());
		}
		if (has_colors) {
			output_cloud.colors_.push_back(accpoint.second.GetAverageColor());
		}
	}
	
	PrintAlways("[VoxelDownSample] Down sampled from %d points to %d points.\n",
			input_cloud.points_.size(), output_cloud.points_.size());
	return true;
}

bool UniformDownSample(const PointCloud &input_cloud, int every_k_points,
		PointCloud &output_cloud)
{
	output_cloud.Clear();
	if (input_cloud.HasPoints() == false) {
		PrintDebug("[UniformDownSample] Input point cloud has no points.\n");
		return false;
	}
	if (every_k_points <= 0) {
		PrintDebug("[UniformDownSample] Illegal sample rate.\n");
		return false;
	}
	bool has_normals = input_cloud.HasNormals();
	bool has_colors = input_cloud.HasColors();
	for (int i = 0; i < static_cast<int>(input_cloud.points_.size());
			i += every_k_points) {
		output_cloud.points_.push_back(input_cloud.points_[i]);
		if (has_normals) {
			output_cloud.normals_.push_back(input_cloud.normals_[i]);
		}
		if (has_colors) {
			output_cloud.colors_.push_back(input_cloud.colors_[i]);
		}
	}
	PrintAlways("[UniformDownSample] Down sampled from %d points to %d points.\n",
			input_cloud.points_.size(), output_cloud.points_.size());
	return true;
}

bool ClipPointCloud(const PointCloud &input_cloud,
		const Eigen::Vector3d &min_bound, const Eigen::Vector3d &max_bound,
		PointCloud &output_cloud)
{
	output_cloud.Clear();
	if (input_cloud.HasPoints() == false) {
		PrintDebug("[ClipPointCloud] Input point cloud has no points.\n");
		return false;
	}
	if (min_bound(0) > max_bound(0) || min_bound(1) > max_bound(1) ||
			min_bound(2) > max_bound(2)) {
		PrintDebug("[ClipPointCloud] Illegal boundary clipped all points.\n");
		return false;
	}
	bool has_normals = input_cloud.HasNormals();
	bool has_colors = input_cloud.HasColors();
	for (size_t i = 0; i < input_cloud.points_.size(); i++) {
		const auto &point = input_cloud.points_[i];
		if (point(0) >= min_bound(0) && point(0) <= max_bound(0) &&
				point(1) >= min_bound(1) && point(1) <= max_bound(1) &&
				point(2) >= min_bound(2) && point(2) <= max_bound(2)) {
			output_cloud.points_.push_back(input_cloud.points_[i]);
			if (has_normals) {
				output_cloud.normals_.push_back(input_cloud.normals_[i]);
			}
			if (has_colors) {
				output_cloud.colors_.push_back(input_cloud.colors_[i]);
			}
		}
	}
	PrintAlways("[ClipPointCloud] Clipped %d points from %d points, %d points remaining.\n",
			input_cloud.points_.size() - output_cloud.points_.size(),
			input_cloud.points_.size(), output_cloud.points_.size());
	return true;
}

}	// namespace three