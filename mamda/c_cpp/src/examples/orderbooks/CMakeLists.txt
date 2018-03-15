cmake_minimum_required (VERSION 2.8.3)
project (mamda_examples_books)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

get_target_property(MAMDA_BOOK_INCLUDES mamdabook INCLUDE_DIRECTORIES)

include_directories(
	${MAMDA_BOOK_INCLUDES}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CURSES_INCLUDE_DIR}
)

mamda_example(atomicbookbuilder mamdabook)
mamda_example(atomicbookticker mamdabook)
mamda_example(bookchurn mamdabook)
mamda_example(bookpublisher mamdabook)
mamda_example(bookticker mamdabook)
mamda_example(bookviewer mamdabook ${CURSES_LIBRARIES})
mamda_example(listenerBookPublisher mamdabook)
