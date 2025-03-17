app: app.c lib internal
	cc -Wall -Wextra -ggdb -o app app.c lib/arena_strings.c lib/http.c lib/arena.c lib/const_strings.c lib/json/frozen.c internal/serializers.c
