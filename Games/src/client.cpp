#include <string>
#include <iostream>

class Client {
private:

public:
    Client(std::string ardess, int port) {
        
    }

    void start() {

    }
};

int main(int argc, char* argv[]) {
    try {
        decltype(auto) client = Client("127.0.0.1",3301);
        client.start();
    } catch(std::exception& exception) {
        std::cout << exception.what() << std::endl;
    }
    return 0;
}