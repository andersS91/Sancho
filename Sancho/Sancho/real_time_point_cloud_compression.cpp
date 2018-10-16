#pragma once

#include "real_time_point_cloud_compression.h"

double max_distance;

void real_time_point_cloud_compression(PointCloud& points, Octree& tree, Settings& settings) {
	Timer t;
	
	t.start();
	// create octree
	create_octree(settings, tree, points);
	t.stop();
	std::cout << "Creating octree: " << t.get_time() << "ms" << std::endl;

	t.start();
	// build octree
	tree.subdivide(settings);
	t.stop();
	std::cout << "Subdividing octree: " << t.get_time() << "ms" << std::endl;

	// show
	std::cout << "Tree contains: " << std::endl;
	std::cout << "Centroid: " << tree.m_centroid.data()[0] << " " << tree.m_centroid.data()[1] << " " << tree.m_centroid.data()[2] << std::endl;
	std::cout << "Levels: " << tree.m_level << std::endl;
	std::cout << "Middle: " << tree.m_middle.data()[0] << " " << tree.m_middle.data()[1] << " " << tree.m_middle.data()[2] << std::endl;
	std::cout << "Size: " << tree.m_size << std::endl;
}

void create_octree(Settings &settings, Octree &tree, PointCloud& points) {
	std::cout << "Creating octree" << std::endl;

	tree.m_middle = Eigen::Vector4d(0.0, 0.0, 0.0, 1.0);
	tree.m_level = 0;
	tree.m_root = &tree;

	const int limit = points.no_of_points;
	const int coords = points.no_of_coords;

	double mix, miy, miz, max, may, maz, temp_hack;
	mix = miy = miz = max = may = maz = temp_hack = 0.0;

	int point_num = 0;
	int i = 0;

	for (; point_num < limit; i += coords) {
		Eigen::Vector4d point(points.points[i], points.points[i+1], points.points[i+2], 1.0);
		tree.m_points.push_back(point);
		tree.m_centroid += point;
		tree.m_indexes.push_back(point_num++);
		
		temp_hack = point.data()[0];
		mix = std::min(mix, temp_hack);
		max = std::max(max, temp_hack);

		temp_hack = point.data()[1];
		miy = std::min(miy, temp_hack);
		may = std::max(may, temp_hack);

		temp_hack = point.data()[2];
		miz = std::min(miz, temp_hack);
		maz = std::max(maz, temp_hack);

	}


	settings.s_ms = settings.s_ps * tree.m_points.size();

	Eigen::Vector4d centroid = tree.m_centroid / tree.m_points.size();

	for (Eigen::Vector4d & v : tree.m_points)
	{
		v -= centroid;
		max_distance = std::max(max_distance, abs(v.data()[0]));
		max_distance = std::max(max_distance, abs(v.data()[1]));
		max_distance = std::max(max_distance, abs(v.data()[2]));
		settings.max_point_distance = std::max(settings.max_point_distance, v.norm());
	}

	tree.m_centroid = Eigen::Vector4d();
	tree.m_size = max_distance * 2.0;
}