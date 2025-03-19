#include "serializers.h"

#define CS_TO_CSTR(varname, cs) \
  char varname[cs.len + 1]; \
  sprintf(varname, "%.*s", (int)(cs).len, (cs).data)

int const_strings_to_fields(struct json_out *out, va_list *ap) {
  size_t n = va_arg(*ap, size_t);

  int len = 0;
  for (size_t i = 0; i < n; i++) {
	char *name = va_arg(*ap, char*);
	const_string cs = va_arg(*ap, const_string);
	CS_TO_CSTR(value, cs);
	if (i > 0) len += json_printf(out, ", ");

	len += json_printf(out, "%Q: %Q", name, value);
  };

  return len;
}

int const_string_da_to_json(struct json_out *out, va_list *ap) {
  const_string_da *p = va_arg(*ap, const_string_da *);
  int len = 0;

  len += json_printf(out, "[", 1);
  for (size_t i = 0; i < p->len; i++) {
	const_string cs = *(p->data + i);
	CS_TO_CSTR(str, cs);
	if (i > 0) len += json_printf(out, ", ");
	len += json_printf(out, "%Q", str);
  }
  len += json_printf(out, "]", 1);
  return len;
}

int phone_to_json(struct json_out *out, va_list *ap) {
  phone *p = va_arg(*ap, phone *);

  return json_printf(out,
					 "{ type_id: %d, country_code: %d, operator: %d, number: %d }",
					 p->type_id, p->country_code, p->operator, p->number);
}

int phone_da_to_json(struct json_out *out, va_list *ap) {
  phone_da *p = va_arg(*ap, phone_da *);
  int len = 0;

  len += json_printf(out, "[", 1);
  for (size_t i = 0; i < p->len; i++) {
	if (i > 0) len += json_printf(out, ", ");
	len += json_printf(out, "%M", phone_to_json, p->data+i);
  }
  len += json_printf(out, "]", 1);
  return len;
}

int contact_to_json(struct json_out *out, va_list *ap) {
  contact c = va_arg(*ap, contact);
  return json_printf(out,
					 "{id: %d, %M, phone: %M, email: %M}",
					 c.id,
					 const_strings_to_fields, 4,
					 "username", c.username,
					 "given_name", c.given_name,
					 "family_name", c.family_name,
					 "birthdate", c.birthdate,
					 phone_da_to_json, &c.phone,
					 const_string_da_to_json, &c.email
					 );
}

int int_da_to_json(struct json_out *out, va_list *ap) {
  int_da *p = va_arg(*ap, int_da *);
  int len = 0;

  len += json_printf(out, "[", 1);
  for (size_t i = 0; i < p->len; i++) {
	int *num = p->data+i;
	if (i > 0) len += json_printf(out, ", ");
	len += json_printf(out, "%d", *num);
  }
  len += json_printf(out, "]", 1);
  return len;
}

int group_to_json(struct json_out *out, va_list *ap) {
  group g = va_arg(*ap, group);
  return json_printf(out,
					 "{id: %d, %M, contacts: %M}",
					 g.id,
					 const_strings_to_fields, 2,
					 "title", g.title,
					 "description", g.description,
					 int_da_to_json, g.contacts
					 );
}

int json_sb_printer(struct json_out *out, const char *str, size_t len) {
  struct json_builder *builder = out->u.data;
  arena_sb_append_buf(builder->arena, builder->da, str, len);
  return len;
}
