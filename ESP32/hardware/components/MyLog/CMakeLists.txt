idf_component_register(SRCS "my_log.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES "MyConstant"
                       PRIV_REQUIRES log )