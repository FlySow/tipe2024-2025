# INTX_T implementation

intx_t "documentation" in intx_t.h

Basically implements int1024_t, int2048_t, etc, by changing the preprocessor directive INT_SIZE in intx_t.h. INT_SIZE represents the number of chunks of uint32_t there is in the architecture, for example: INT_SIZE = 4, means a 128bits representation.
The "library" implements basic signed operations, like add, sub, mul, div, mod, don't forget to free because most of the functions allocate new integer (might work on an other implementation later down the line).
