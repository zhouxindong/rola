# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("rola")

# Include sub-projects.
add_subdirectory ("base")
add_subdirectory ("mpl")
add_subdirectory ("stlex")
add_subdirectory ("utils")
add_subdirectory ("easy_signal")
add_subdirectory ("easy_log")
add_subdirectory ("lingex")
add_subdirectory ("csp")
add_subdirectory ("atm")
add_subdirectory ("drop")
add_subdirectory ("optimize")
add_subdirectory ("easy_socket")
add_subdirectory ("easy_async")
add_subdirectory ("easy_bus")
add_subdirectory ("csp2")
add_subdirectory ("gsl")
add_subdirectory ("jsnader")
add_subdirectory ("tlpi")
add_subdirectory ("safecpp")
add_subdirectory ("easy_socket2")
add_subdirectory ("easy_pipo")