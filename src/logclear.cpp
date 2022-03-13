/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "logclear.h"

#include <iostream>
#include <string>
#include <chrono>
#include <vector>

template<typename Clock>
bool is_older_than( const std::filesystem::path& path, int hrs, std::chrono::time_point<Clock> now)
{
	return std::chrono::duration_cast<std::chrono::hours>( now - std::filesystem::last_write_time(path) ).count() > hrs ;
}

template<typename Clock>
std::vector<std::filesystem::path> files_older_than(std::filesystem::path dir, std::chrono::time_point<Clock> now, int hrs)
{
	std::vector<std::filesystem::path> result ;
	for( const auto& p : std::filesystem::recursive_directory_iterator(dir) ){
		if (std::filesystem::is_regular_file(p) && is_older_than(p, hrs, now) ){
			result.push_back(p);
		}
	}
	return result;
}

bool remove_files_older_than( std::filesystem::path dir, unsigned int maxhours )
{
	int cnt = 0 ;
	auto now = std::filesystem::file_time_type::clock::now() ;
	try {
		for( const auto& p : files_older_than(dir, now, maxhours ) )
		{
			std::filesystem::remove(p);
		}
		return true;
	} catch( const std::exception& ) {
		return false;
	}
}

