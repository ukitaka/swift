add_swift_unittest(SwiftSemaTests
    SubTypeTests.cpp
    TestContext.cpp
)

include_directories(BEFORE
    ${SWIFT_SOURCE_DIR}/lib/Sema
)

target_link_libraries(SwiftSemaTests
    swiftAST
    swiftParse
    swiftSema
)
