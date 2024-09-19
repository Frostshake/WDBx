vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO Frostshake/WDBReader
    REF d032d9e0edc852606eb0a3666bb7768ed66e13b5
    SHA512 90cedd6d2423a3abbe20ba5eb89d5374988e5ddb312d56bed6c8dcf8abc2d61608775b8ef9eff04d136206c3ef0dff5c0b0c49eb19f50b8c5c6bd7b6735fb98a
    HEAD_REF main 
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DBUILD_APPS=OFF
        -DBUILD_TESTING=OFF
        -DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=TRUE
)

vcpkg_cmake_install()
vcpkg_copy_pdbs()
vcpkg_fixup_pkgconfig()

vcpkg_cmake_config_fixup(CONFIG_PATH lib/cmake/WDBReader)

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)