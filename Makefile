CXXFLAGS += -std=c++11

ifeq ($(SYSTEM),nadiros)
LDFLAGS += -L/usr/local/lib `pkg-config --libs protobuf grpc++ `\
	-ldl\
		-lpaho-mqtt3c -lpaho-mqttpp3 -lpaho-mqtt3as 
else
LDFLAGS = -L/recipe-sysroot/usr/bin  `pkg-config --libs protobuf grpc++ `\
           -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed\
           -ldl\
		   -lpaho-mqtt3c -lpaho-mqttpp3 -lpaho-mqtt3as 
endif

PROTOC = protoc
CPPFLAGS += `pkg-config --cflags protobuf grpc`


GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= ~/poky/rpibuild/tmp/sysroots-components/cortexa72-poky-linux/grpc-native/usr/bin/grpc_cpp_plugin

all: client server

client: myServer.pb.o myServer.grpc.pb.o client.o
	$(CXX) $^ $(LDFLAGS) -o $@

server: myServer.pb.o myServer.grpc.pb.o server.o
	$(CXX) $^ $(LDFLAGS) -o $@

%.grpc.pb.cc: %.proto
	protoc --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

%.pb.cc: %.proto
	protoc --cpp_out=. $<

clean:
	rm -f *.o *.pb.cc *.pb.h client server
