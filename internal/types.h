#ifndef TYPES_H_
#define TYPES_H_

#include <stddef.h>
#include "../lib/const_strings.h"
#include "../lib/arena_strings.h"

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

#endif // TYPES_H_
