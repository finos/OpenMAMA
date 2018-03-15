cmake_minimum_required (VERSION 2.8.3)
project (mamda_examples_news)

#get_target_property(MAMDA_INCLUDES mamda INCLUDE_DIRECTORIES)
get_target_property(MAMDA_NEWS_INCLUDES mamdanews INCLUDE_DIRECTORIES)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${MAMDA_NEWS_INCLUDES}
)

mamda_example(newsticker mamdanews)
