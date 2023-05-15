add_library(hlc
  src/allocator.c
  src/comparator.c
  src/darray.c
  src/layout.c
  src/map.c
  src/printer.c
  src/sarray.c
)

target_include_directories(hlc
  PUBLIC include
)
