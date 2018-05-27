protoc -I=. --python_out=./server ratPacket.proto
protoc -I=. --cpp_out=./client ratPacket.proto
