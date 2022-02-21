#ifndef UTILS_H
#define UTILS_H

typedef void (*UpdateFunction)();
typedef void (*DrawFunction)();

// Defined in .ino file. Declared here for convenience
extern UpdateFunction updateFunction;
extern DrawFunction drawFunction;

#endif
