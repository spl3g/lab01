FROM alpine:3.21 AS builder
RUN apk add --no-cache build-base

WORKDIR /cbuild

COPY ./include ./include
COPY ./src ./src
COPY ./third_party ./third_party
COPY ./Makefile ./

RUN make


FROM alpine:3.21

WORKDIR /contacts

COPY --from=builder /cbuild/app ./

EXPOSE 7080

CMD [ "/contacts/app" ]
