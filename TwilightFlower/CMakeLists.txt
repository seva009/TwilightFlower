# CMakeList.txt: проект CMake для TwilightFlower; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#

# Добавьте источник в исполняемый файл этого проекта.
add_executable (TwilightFlower "TwilightFlower.cpp" "io.cpp" "aes256/aes256.cpp" "crc32.cpp" "EasyDTP.cpp" "pkt_io_hdl.cpp" "iopool.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET TwilightFlower PROPERTY CXX_STANDARD 20)
endif()

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
