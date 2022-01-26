
cmake_minimum_required(VERSION 3.20)

set (header_files
	delegate.hpp
	multicastdelegate.hpp
	eventhandler.hpp
	event.hpp
	delegate.hpp
)

add_library(EventHandling INTERFACE ${header_files})
