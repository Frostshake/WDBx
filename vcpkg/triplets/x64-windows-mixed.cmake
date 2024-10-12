set(VCPKG_TARGET_ARCHITECTURE x64)

if(${PORT} MATCHES "tree-sitter|wdbreader|casclib|stormlib|bzip2|zlib")
    set(VCPKG_CRT_LINKAGE dynamic)
	set(VCPKG_LIBRARY_LINKAGE static)
else()
    set(VCPKG_CRT_LINKAGE dynamic)
    set(VCPKG_LIBRARY_LINKAGE dynamic)
endif()