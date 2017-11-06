add_swift_unittest(SwiftSemaTests
  SubTypeTests.cpp
)

target_link_libraries(SwiftSemaTests
   swiftAST
   swiftParse
   swiftSema
)
