# BufferLib
A C++ header-only library with safer buffer classes.
All buffer classes are available in the `BufferLib` namespace.

| Class | Based on container | Syntax | Comments |
| ---   | ---                | ---    | ---      |
| `array_buffer` | `std::array<>` | `BufferLib::array_buffer<512>` | Works with all arithmetric types, _e.g._ `char`, `std::uint8_t`, `std::int64_t`, `double` |
