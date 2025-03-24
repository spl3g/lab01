#include <stdlib.h>

#include <chttp/const_strings.h>
#include <chttp/http.h>
#include <frozen/frozen.h>
#include "types.h"
#include "serializers.h"

void contacts_get(http_request req) {
  arena *arena = req.arena;

  string_builder output = {0};
  struct json_out out = JSON_OUT_BUILDER(arena, output);
  
  contact *contact = get_context_value(req.ctx, "contact");
  json_printf(&out, "%M", contact_to_json, *contact);

  req.resp->code = OK;
  http_send_json(req, arena_sb_to_cs(output));
}

void contact_get(http_request req) {
  arena *arena = req.arena;

  string_builder output = {0};
  struct json_out out = JSON_OUT_BUILDER(arena, output);

  contact *contact = get_context_value(req.ctx, "contact");

  const_string *id_cs = get_context_value(req.ctx, "id");
  char id_str[id_cs->len+1];
  long id =  strtol(id_str, NULL, 10);
  sprintf(id_str, CS_FMT, CS_ARG(*id_cs));
  
  contact->id = id;
  
  json_printf(&out, "%M", contact_to_json, *contact);

  req.resp->code = OK;
  http_send_json(req, arena_sb_to_cs(output));
}

void contact_post(http_request req) {
  arena *arena = req.arena;

  string_builder output = {0};
  struct json_out out = JSON_OUT_BUILDER(arena, output);

  contact *contact = get_context_value(req.ctx, "contact");
  
  json_printf(&out, "%M", contact_to_json, *contact);

  req.resp->code = CREATED;
  http_send_json(req, arena_sb_to_cs(output));
}

void contact_put(http_request req) {
  arena *arena = req.arena;

  string_builder output = {0};
  struct json_out out = JSON_OUT_BUILDER(arena, output);

  contact *contact = get_context_value(req.ctx, "contact");

  const_string *id_cs = get_context_value(req.ctx, "id");
  char id_str[id_cs->len+1];
  sprintf(id_str, CS_FMT, CS_ARG(*id_cs));
  long id =  strtol(id_str, NULL, 10);

  contact->id = id;
  
  json_printf(&out, "%M", contact_to_json, *contact);

  req.resp->code = OK;
  http_send_json(req, arena_sb_to_cs(output));
}

void contact_delete(http_request req) {
  req.resp->code = NO_CONTENT;
  http_send(req);
}

void groups_get(http_request req) {
  arena *arena = req.arena;

  string_builder output = {0};
  struct json_out out = JSON_OUT_BUILDER(arena, output);

  contact *group = get_context_value(req.ctx, "group");
  json_printf(&out, "%M", group_to_json, *group);

  req.resp->code = OK;
  http_send_json(req, arena_sb_to_cs(output));
}

void group_get(http_request req) {
  arena *arena = req.arena;

  string_builder output = {0};
  struct json_out out = JSON_OUT_BUILDER(arena, output);

  contact *group = get_context_value(req.ctx, "group");
  
  const_string *id_cs = get_context_value(req.ctx, "id");
  char id_str[id_cs->len+1];
  sprintf(id_str, CS_FMT, CS_ARG(*id_cs));
  long id =  strtol(id_str, NULL, 10);

  group->id = id;
  json_printf(&out, "%M", group_to_json, *group);

  req.resp->code = OK;
  http_send_json(req, arena_sb_to_cs(output));
}

void group_post(http_request req) {
  arena *arena = req.arena;

  string_builder output = {0};
  struct json_out out = JSON_OUT_BUILDER(arena, output);

  contact *group = get_context_value(req.ctx, "group");
  json_printf(&out, "%M", group_to_json, *group);

  req.resp->code = CREATED;
  http_send_json(req, arena_sb_to_cs(output));
}

void group_put(http_request req) {
  arena *arena = req.arena;

  string_builder output = {0};
  struct json_out out = JSON_OUT_BUILDER(arena, output);

  contact *group = get_context_value(req.ctx, "group");

  const_string *id_cs = get_context_value(req.ctx, "id");
  char id_str[id_cs->len+1];
  sprintf(id_str, CS_FMT, CS_ARG(*id_cs));
  long id =  strtol(id_str, NULL, 10);

  group->id = id;
  json_printf(&out, "%M", group_to_json, *group);

  req.resp->code = OK;
  http_send_json(req, arena_sb_to_cs(output));
}

void group_delete(http_request req) {
  req.resp->code = NO_CONTENT;
  http_send(req);
}

void logging_middleware(http_middleware *self, http_request req) {
  http_run_next(self, req);
  http_log(HTTP_INFO, "\""CS_FMT" "CS_FMT" "CS_FMT"\""" %ld\n", CS_ARG(req.method), CS_ARG(req.path), CS_ARG(req.version), req.resp->code);
}

int main() {
  arena arena = {0};
  http_server serv = {0};
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
  http_context ctx = {0};
  set_context_value(&arena, &ctx, (http_context_value){CS("contact"), &ivan});
  set_context_value(&arena, &ctx, (http_context_value){CS("group"), &friends});
  serv.global_ctx = ctx;

  http_register_global_middleware(&serv, logging_middleware);

  http_handle_path(&serv, "GET", "/api/v1/contact", contacts_get);
  http_handle_path(&serv, "GET", "/api/v1/contact/:id", contact_get);
  http_handle_path(&serv, "POST", "/api/v1/contact", contact_post);
  http_handle_path(&serv, "PUT", "/api/v1/contact/:id", contact_put);
  http_handle_path(&serv, "DELETE", "/api/v1/contact/:id", contact_delete);

  http_handle_path(&serv, "GET", "/api/v1/group", groups_get);
  http_handle_path(&serv, "GET", "/api/v1/group/:id", group_get);
  http_handle_path(&serv, "POST", "/api/v1/group", group_post);
  http_handle_path(&serv, "PUT", "/api/v1/group/:id", group_put);
  http_handle_path(&serv, "DELETE", "/api/v1/group/:id", group_delete);
  listen_and_serve(&serv);

  return 0;
}
