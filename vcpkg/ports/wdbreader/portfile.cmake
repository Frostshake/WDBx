vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO Frostshake/WDBReader
    REF 48d31f03a94368555842ead6b12029c194dd854b
    SHA512 8fa819d8b8f3edaefa74bdcd41e86848cff36c2389e6f2c4708dbbc0c7bdb698d8dfad2fecb4499081dfc2b975590310c767c1655a1e9ac75f150b760d44b56c
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