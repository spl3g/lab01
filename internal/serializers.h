#ifndef SERIALIZERS_H_
#define SERIALIZERS_H_

#include "../lib/arena.h"
#include "../lib/arena_strings.h"
#include "../lib/json/frozen.h"
#include "types.h"

struct json_builder {
  arena* arena;
  string_builder *da;
};

int json_sb_printer(struct json_out *out, const char *str, size_t len);

int int_da_to_json(struct json_out *out, va_list *ap);

int const_strings_to_fields(struct json_out *out, va_list *ap);
int const_string_da_to_json(struct json_out *out, va_list *ap);

int phone_to_json(struct json_out *out, va_list *ap);
int phone_da_to_json(struct json_out *out, va_list *ap);

int contact_to_json(struct json_out *out, va_list *ap);
int group_to_json(struct json_out *out, va_list *ap);

#define JSON_OUT_BUILDER(da) {json_sb_printer, {.data = &(struct json_builder){arena, &da}}}

#endif //SERIALIZERS_H_
