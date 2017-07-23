
CC=gcc
CCFLAGS=
LDFLAGS=-ljson-c -lwebsockets -lpaho-mqtt3c

SRC_DIR=src
BLD_DIR=build
OBJ_DIR=build/obj

TARGET=$(BLD_DIR)/telephone-app

SOURCES=$(SRC_DIR)/main.c \
	$(SRC_DIR)/mqtt_wrapper.c \
	$(SRC_DIR)/serial_port.c \
	$(SRC_DIR)/log.c

XOBJECTS=$(SOURCES:.c=.o)
OBJECTS=$(subst $(SRC_DIR),$(OBJ_DIR),$(XOBJECTS))

all: $(TARGET)

clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(BLD_DIR)

docker-up:
	docker-compose -f docker/dev/docker-compose.yml up -d

docker-down:
	docker-compose -f docker/dev/docker-compose.yml stop
	docker-compose -f docker/dev/docker-compose.yml down

docker-build:
	docker exec test-test make -C /usr/local/src/test clean all

docker-restart:
	docker-compose -f docker/dev/docker-compose.yml restart

docker-logs:
	docker-compose -f docker/dev/docker-compose.yml logs

$(TARGET): $(BLD_DIR) $(OBJ_DIR) $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

$(BLD_DIR):
	mkdir -p $(BLD_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $< $(CCFLAGS) -o $@
