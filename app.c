#include "lib/const_strings.h"
#include "lib/http.h"
#include "lib/json/frozen.h"

typedef struct {
  size_t type_id;
  size_t country_code;
  size_t operator;
  int number;
} phone;

typedef struct {
  phone *data;
  size_t len;
  size_t cap;
} phone_da;

typedef struct {
   size_t id;
   const_string username;
   const_string given_name;
   const_string family_name;
   phone_da phone;
   const_string_da email;
   const_string birthdate;
} contact;

typedef struct {
  int *data;
  size_t len;
  size_t cap;
} int_da;

typedef struct {
  int id;
  const_string title;
  const_string description;
  int_da *contacts;
} group;

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

struct json_builder {
  arena* arena;
  const_string_da *da;
};
  
int json_sb_printer(struct json_out *out, const char *str, size_t len) {
  struct json_builder *builder = out->u.data;
  const_string cs = arena_cs_init(builder->arena, len);
  memcpy(cs.data, str, len);
  arena_da_append(builder->arena, builder->da, cs);
  return len;
}
  
void contact_get(context *ctx, struct request req, struct response *resp) {
  printf("GET contact");
  const_string_da output_da = {0};
  arena *arena = get_context_value(ctx, CS("arena"));
  
  struct json_builder builder = {
	.arena = arena,
	.da = &output_da,
  };
  
  struct json_out out = {json_sb_printer, {.data = (void *)&builder}};

  contact *contact = get_context_value(ctx, CS("contact"));
  json_printf(&out, "%M", contact_to_json, *contact);
  arena_da_append(arena, &output_da, CS("\r\n"));
  const_string output = arena_cs_concat(arena, output_da, CS(""));

  resp_set_code(resp, OK);
  resp_set_json(arena, resp, output);
}

void contact_post(context *ctx, struct request req, struct response *resp) {
  printf("POST contact");
  const_string_da output_da = {0};
  arena *arena = get_context_value(ctx, CS("arena"));

  struct json_builder builder = {
	.arena = arena,
	.da = &output_da,
  };

  struct json_out out = {json_sb_printer, {.data = (void *)&builder}};

  contact *contact = get_context_value(ctx, CS("contact"));
  json_printf(&out, "%M", contact_to_json, *contact);
  arena_da_append(arena, &output_da, CS("\r\n"));
  const_string output = arena_cs_concat(arena, output_da, CS(""));

  resp_set_code(resp, CREATED);
  resp_set_json(arena, resp, output);
}

void contact_put(context *ctx, struct request req, struct response *resp) {
  printf("PUT contact");
  const_string_da output_da = {0};
  arena *arena = get_context_value(ctx, CS("arena"));

  struct json_builder builder = {
	.arena = arena,
	.da = &output_da,
  };

  struct json_out out = {json_sb_printer, {.data = (void *)&builder}};

  contact *contact = get_context_value(ctx, CS("contact"));
  json_printf(&out, "%M", contact_to_json, *contact);
  arena_da_append(arena, &output_da, CS("\r\n"));
  const_string output = arena_cs_concat(arena, output_da, CS(""));

  resp_set_code(resp, OK);
  resp_set_json(arena, resp, output);
}

void contact_delete(context *ctx, struct request req, struct response *resp) {
  printf("DELETE contact");
  resp->code = NO_CONTENT;
}

void group_get(context *ctx, struct request req, struct response *resp) {
  const_string_da output_da = {0};
  arena *arena = get_context_value(ctx, CS("arena"));

  struct json_builder builder = {
	.arena = arena,
	.da = &output_da,
  };

  struct json_out out = {json_sb_printer, {.data = (void *)&builder}};

  contact *group = get_context_value(ctx, CS("group"));
  int len = json_printf(&out, "%M", group_to_json, *group);
  arena_da_append(arena, &output_da, CS("\r\n"));
  const_string output = arena_cs_concat(arena, output_da, CS(""));

  resp_set_code(resp, OK);
  resp_set_json(arena, resp, output);
}

void group_post(context *ctx, struct request req, struct response *resp) {
  resp->code = CREATED;
}

void group_put(context *ctx, struct request req, struct response *resp) {
  resp->code = OK;
}

void group_delete(context *ctx, struct request req, struct response *resp) {
  resp->code = NO_CONTENT;
}

int main() {
  arena arena = {0};
  struct server serv = {0};
  if (init_server(&arena, &serv, NULL, "7080") != 0) {
	return 1;
  }

  phone_da phones = {0};
  phone phone = {
	.type_id = 5,
	.country_code = 8,
	.operator = 800,
	.number = 5553535,
  };
  arena_da_append(&arena, &phones, phone);

  const_string_da emails = {0};
  const_string email = CS("admin@example.com");
  arena_da_append(&arena, &emails, email);
  
  contact ivan = {
	.id = 0,
	.username = CS_STATIC("ivan"),
	.given_name = CS_STATIC("Ivan"),
	.family_name = CS_STATIC("Ivanov"),
	.phone = phones,
	.email = emails,
	.birthdate = CS("2001"),
  };

  int_da contact_da = {0};
  int contact = 1;
  arena_da_append(&arena, &contact_da, contact);
  
  group friends = {
	.id = 0,
	.title = CS("test"),
	.description = CS("desc"),
	.contacts = &contact_da,
  };
  context ctx = {0};
  set_context_value(&arena, &ctx, (context_value){CS("contact"), &ivan});
  set_context_value(&arena, &ctx, (context_value){CS("group"), &friends});
  serv.global_ctx = ctx;
  /* char *str = json_asprintf("%M", group_to_json, test); */
  /* printf("%s", str); */

  handle_path(&serv, CS("GET"), CS("/api/v1/contact"), contact_get);
  handle_path(&serv, CS("POST"), CS("/api/v1/contact"), contact_post);
  handle_path(&serv, CS("PUT"), CS("/api/v1/contact"), contact_put);
  handle_path(&serv, CS("DELETE"), CS("/api/v1/contact"), contact_delete);

  handle_path(&serv, CS("GET"), CS("/api/v1/group"), group_get);
  handle_path(&serv, CS("POST"), CS("/api/v1/group"), group_post);
  handle_path(&serv, CS("PUT"), CS("/api/v1/group"), group_put);
  handle_path(&serv, CS("DELETE"), CS("/api/v1/group"), group_delete);
  listen_and_serve(&serv);

  return 0;
}
