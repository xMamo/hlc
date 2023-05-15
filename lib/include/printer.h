#ifndef PRINTER_H
#define PRINTER_H

#include <stdarg.h>
#include <stdio.h>

typedef struct Printer_methods {
  int (*print)(void* data, const char* string);
  int (*printf)(void* data, const char* format, ...);
  int (*vprintf)(void* data, const char* format, va_list ap);
} Printer_methods;

typedef struct Printer {
  void* data;
  const Printer_methods* methods;
} Printer;

extern const Printer_methods stream_printer_methods;

static inline int printer_print(Printer printer, const char* string) {
  return printer.methods->print(printer.data, string);
}

static inline int printer_printf(Printer printer, const char* string, const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  int result = printer.methods->printf(printer.data, string);
  va_end(ap);
  return result;
}

static inline int printer_vprintf(Printer printer, const char* format, va_list ap) {
  return printer.methods->vprintf(printer.data, format, ap);
}

int stream_print(FILE* stream, const char* string);

int stream_printf(FILE* stream, const char* format, ...);

int stream_vprintf(FILE* stream, const char* format, va_list ap);

#endif
