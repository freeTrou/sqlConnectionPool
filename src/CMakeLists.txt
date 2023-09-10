# 定义一个SRC_LIST变量，包含了该目录下所有的源文件
aux_source_directory(. SRC_LIST)

# 指定生成可执行文件
add_executable(sqlConnectionPool ${SRC_LIST})
# 指定可执行文件链接时所需要依赖库文件 -l 链接库
target_link_libraries(sqlConnectionPool mysqlclient pthread)  