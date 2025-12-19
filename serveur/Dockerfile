# --- ETAPE 1: Build Stage (Compilation) ---
FROM gcc:11 AS build
WORKDIR /app
COPY . .
RUN gcc -o serveur serveur.c

# --- ETAPE 2: Final Stage (Image legere) ---
FROM debian:bookworm-slim
WORKDIR /usr/local/bin
COPY --from=build /app/serveur .
CMD ["./serveur", "6000"]
