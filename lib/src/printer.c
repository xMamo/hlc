#include "printer.h"

#include <stdarg.h>
#include <wchar.h>

static int _stream_print(void* stream, const char* string) {
  return stream_print(stream, string);
}

static int _stream_printf(void* stream, const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  int result = stream_vprintf(stream, format, ap);
  va_end(ap);
  return result;
}

static int _stream_vprintf(void* stream, const char* format, va_list ap) {
  return stream_vprintf(stream, format, ap);
}

const Printer_methods stream_printer_info = {
  .print = _stream_print,
  .printf = _stream_printf,
  .vprintf = _stream_vprintf,
};

int stream_print(FILE* stream, const char* string) {
  return fwide(stream, 0) <= 0 ? fputs(string, stream) : fwprintf(stream, L"%s", string);
}

int stream_printf(FILE* stream, const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  int result = _stream_vprintf(stream, format, ap);
  va_end(ap);
  return result;
}

int stream_vprintf(FILE* stream, const char* format, va_list ap) {
  if (fwide(stream, 0) <= 0)
    return vfprintf(stream, format, ap);

  int n;

  {
    char buffer[BUFSIZ];
    n = vsnprintf(buffer, BUFSIZ, format, ap);

    if (n < 0)
      return n;
    else if (n < BUFSIZ)
      return fwprintf(stream, L"%s", buffer);
  }

  {
    char buffer[n + 1];
    n = vsnprintf(buffer, n + 1, format, ap);

    if (n < 0)
      return n;
    else
      return fwprintf(stream, L"%s", buffer);
  }
}
