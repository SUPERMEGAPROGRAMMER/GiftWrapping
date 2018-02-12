#pragma once

#include "stdafx.h"
#include "MathVector.h"
#include "ConvexHull.h"

class Parser
{
public:
	Parser(	const char* in_filename,
			const char* out_filename_points,
			const char* out_filename_faces);
	~Parser();

	void read();
	void write();

	std::vector<MathVector> in_info;
	ConvexHull out_info;
	bool open_correct;
	bool parse_correct;

private:
	void PrintErrorMessage();

	std::ifstream in_stream;
	std::ofstream out_stream_points;
	std::ofstream out_stream_faces;
	//std::ofstream out_stream_stat_info;
};

