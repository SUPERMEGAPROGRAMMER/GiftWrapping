#include "stdafx.h"
#include "Parser.h"


Parser::Parser(	const char* in_filename, 
				const char* out_filename_points,
				const char* out_filename_faces)
{
	if (in_filename == nullptr || 
		out_filename_points == nullptr ||
		out_filename_faces == nullptr)
	{
		in_stream.open("data/source_points.txt");
		out_stream_points.open("data/result_points.txt");
		out_stream_faces.open("data/result_faces.txt");
		//out_stream_stat_info.open("data/result_stat_info.txt");
	}
	else
	{
		in_stream.open(in_filename);
		out_stream_points.open(out_filename_points);
		out_stream_faces.open(out_filename_faces);
		//out_stream_stat_info.open(out_filename_stat_info);
	}

	open_correct = (in_stream.is_open() && 
					out_stream_points.is_open() &&
					out_stream_faces.is_open());
}

Parser::~Parser()
{
	PrintErrorMessage();

	if (open_correct)
	{
		in_stream.close();
		out_stream_points.close();
		out_stream_faces.close();
	}
}

void Parser::read()
{
	size_t dimension;
	in_stream >> dimension;

	MathVector v(dimension);

	in_stream >> v;

	while (!in_stream.eof())
	{
		in_info.push_back(v);
		in_stream >> v;
	}
}

void Parser::write()
{
	for (auto point : out_info.points)
		out_stream_points << point << std::endl;

	for (auto faces : out_info.faces.m_data)
		out_stream_faces << faces << std::endl;
}

void Parser::PrintErrorMessage()
{
	if (!open_correct)
	{
		std::cerr << "GiftWrapping error: input file does not exist ..." << std::endl;
	}
	else
	if (!parse_correct)
	{
		std::cerr << "GiftWrapping error: parse error of input file ..." << std::endl;
	}
}