# Define the test executable
add_executable(schmetterling_tests
    main_test.cpp
    # bitboard/test_bitBoardUtil.cpp
    board/test_board.cpp
    board/test_coord.cpp
    # board/test_moves.cpp
    # board/test_piece.cpp
    # board/test_pieceList.cpp
    helpers/test_fen.cpp
    # state/test_state.cpp
)


# Include directories
target_include_directories(schmetterling_tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

# Discover tests for CTest
include(GoogleTest)

# Link with GoogleTest and schmetterling library
target_link_libraries(
  schmetterling_tests
  GTest::gtest_main
  schmetterling
)

include(GoogleTest)
gtest_discover_tests(schmetterling_tests)
