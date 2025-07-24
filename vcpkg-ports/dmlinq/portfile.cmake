
vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO brinkqiang/dmlinq # <-- 替换为你的 GitHub 用户名/仓库名
    REF v1.0.1                 # <-- 替换为代码版本的 Git Tag
    SHA512 43a68e8f467f8de8deeacf2d31f2489d7a72f08f40b044015e37c57cc6b67480ed41a0c5b8f4bad2986dd8470a05f8ec22556ffbd90a7d0bb46179e076ccce66
    HEAD_REF master              # <-- 开发版本对应的分支
)

vcpkg_cmake_configure(
    SOURCE_PATH ${SOURCE_PATH}
    OPTIONS
        # 你的 CMakeLists.txt 会通过 PROJECT_IS_TOP_LEVEL 
        # 自动跳过测试和示例的构建，所以这里不需要额外参数。
        # 如果有其他需要关闭的选项，可以在这里添加。
        # -DOPTION_NAME=OFF
)

vcpkg_cmake_install()

vcpkg_copy_pdbs()

# 安装 LICENSE 文件和 usage 文件
file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)
file(INSTALL ${CURRENT_PORT_DIR}/usage DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT})