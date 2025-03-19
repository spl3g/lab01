#include "lib/const_strings.h"
#include "lib/http.h"
#include "lib/json/frozen.h"
#include "internal/types.h"
#include "internal/serializers.h"

void contacts_get(context *ctx, struct request req, struct response *resp) {
  http_log(HTTP_INFO, "GET contacts\n");
  arena *arena = get_context_value(ctx, CS("arena"));

  string_builder output = {0};
  struct json_out out = JSON_OUT_BUILDER(output);
  
  contact *contact = get_context_value(ctx, CS("contact"));
  json_printf(&out, "%M", contact_to_json, *contact);

  resp_set_code(resp, OK);
  resp_set_json(arena, resp, arena_sb_to_cs(output));
}

void contact_get(context *ctx, struct request req, struct response *resp) {
  const_string *id = get_context_value(ctx, CS("id"));
  http_log(HTTP_INFO, "GET contact id=%.*s\n", id->len, id->data);
  
  arena *arena = get_context_value(ctx, CS("arena"));

  string_builder output = {0};
  struct json_out out = JSON_OUT_BUILDER(output);

  contact *contact = get_context_value(ctx, CS("contact"));
  json_printf(&out, "%M", contact_to_json, *contact);

  resp_set_code(resp, OK);
  resp_set_json(arena, resp, arena_sb_to_cs(output));
}

void contact_post(context *ctx, struct request req, struct response *resp) {
  http_log(HTTP_INFO, "POST contact\n");
  arena *arena = get_context_value(ctx, CS("arena"));

  string_builder output = {0};
  struct json_out out = JSON_OUT_BUILDER(output);

  contact *contact = get_context_value(ctx, CS("contact"));
  json_printf(&out, "%M", contact_to_json, *contact);

  resp_set_code(resp, CREATED);
  resp_set_json(arena, resp, arena_sb_to_cs(output));
}

void contact_put(context *ctx, struct request req, struct response *resp) {
  http_log(HTTP_INFO, "PUT contact\n");
  arena *arena = get_context_value(ctx, CS("arena"));

  string_builder output = {0};
  struct json_out out = JSON_OUT_BUILDER(output);

  contact *contact = get_context_value(ctx, CS("contact"));
  json_printf(&out, "%M", contact_to_json, *contact);

  resp_set_code(resp, OK);
  resp_set_json(arena, resp, arena_sb_to_cs(output));
}

void contact_delete(context *ctx, struct request req, struct response *resp) {
  http_log(HTTP_INFO, "DELETE contact\n");
  resp->code = NO_CONTENT;
}

void groups_get(context *ctx, struct request req, struct response *resp) {
  arena *arena = get_context_value(ctx, CS("arena"));

  string_builder output = {0};
  struct json_out out = JSON_OUT_BUILDER(output);

  contact *group = get_context_value(ctx, CS("group"));
  json_printf(&out, "%M", group_to_json, *group);

  resp_set_code(resp, OK);
  resp_set_json(arena, resp, arena_sb_to_cs(output));
}

void group_get(context *ctx, struct request req, struct response *resp) {
  arena *arena = get_context_value(ctx, CS("arena"));

  string_builder output = {0};
  struct json_out out = JSON_OUT_BUILDER(output);

  contact *group = get_context_value(ctx, CS("group"));
  json_printf(&out, "%M", group_to_json, *group);

  resp_set_code(resp, OK);
  resp_set_json(arena, resp, arena_sb_to_cs(output));
}

void group_post(context *ctx, struct request req, struct response *resp) {
  arena *arena = get_context_value(ctx, CS("arena"));

  string_builder output = {0};
  struct json_out out = JSON_OUT_BUILDER(output);

  contact *group = get_context_value(ctx, CS("group"));
  json_printf(&out, "%M", group_to_json, *group);

  resp_set_code(resp, CREATED);
  resp_set_json(arena, resp, arena_sb_to_cs(output));
}

void group_put(context *ctx, struct request req, struct response *resp) {
  arena *arena = get_context_value(ctx, CS("arena"));

  string_builder output = {0};
  struct json_out out = JSON_OUT_BUILDER(output);

  contact *group = get_context_value(ctx, CS("group"));
  json_printf(&out, "%M", group_to_json, *group);

  resp_set_code(resp, OK);
  resp_set_json(arena, resp, arena_sb_to_cs(output));
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

  handle_path(&serv, CS("GET"), CS("/api/v1/contact"), contacts_get);
  handle_path(&serv, CS("GET"), CS("/api/v1/contact/:id"), contact_get);
  handle_path(&serv, CS("POST"), CS("/api/v1/contact"), contact_post);
  handle_path(&serv, CS("PUT"), CS("/api/v1/contact/:id"), contact_put);
  handle_path(&serv, CS("DELETE"), CS("/api/v1/contact/:id"), contact_delete);

  handle_path(&serv, CS("GET"), CS("/api/v1/group"), groups_get);
  handle_path(&serv, CS("GET"), CS("/api/v1/group/:id"), group_get);
  handle_path(&serv, CS("POST"), CS("/api/v1/group"), group_post);
  handle_path(&serv, CS("PUT"), CS("/api/v1/group/:id"), group_put);
  handle_path(&serv, CS("DELETE"), CS("/api/v1/group/:id"), group_delete);
  listen_and_serve(&serv);

  return 0;
}
