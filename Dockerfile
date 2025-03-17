FROM alpine:3.21 AS builder
RUN apk add --no-cache gcc musl-dev make

WORKDIR /cbuild

COPY ./lib ./lib
COPY ./app.c ./
COPY ./Makefile ./

RUN make


FROM alpine:3.21

WORKDIR /contacts

COPY --from=builder /cbuild/app ./

EXPOSE 7080

CMD [ "/contacts/app" ]
