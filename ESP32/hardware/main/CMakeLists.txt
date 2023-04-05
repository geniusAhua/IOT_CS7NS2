set(COMPONENT_ADD_INCLUDEDIRS
	"."
	"${CMAKE_CURRENT_LIST_DIR}"
	)

idf_component_register(SRCS "main.cpp"
                       INCLUDE_DIRS ${COMPONENT_ADD_INCLUDEDIRS})
