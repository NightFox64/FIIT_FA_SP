add_executable(
        mp_os_arthmtc_frctn_tests
        fraction_tests.cpp)

target_link_libraries(
        mp_os_arthmtc_frctn_tests
        PRIVATE
        mp_os_arthmtc_frctn)

add_test(
        NAME
        mp_os_arthmtc_frctn_tests
        COMMAND
        mp_os_arthmtc_frctn_tests)