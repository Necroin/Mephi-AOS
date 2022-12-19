#include "libraries.h"

bool working = true;

class Client {
public:
    enum class AdressFamily : int
    {
        ipv4 = PF_INET
    };
    
    enum class SocketType : int
    {
        UDP = SOCK_DGRAM,
        TCP = SOCK_STREAM
    };

    struct Options
    {
        AdressFamily family = AdressFamily::ipv4;
        SocketType type = SocketType::TCP;
        int reader_buffer_size = 8192;
        std::ostream& log_ostream = std::cout;
    };
private:
    int _connect_socket;
    std::string _address;
	int _port;
    Options _options;
    Logger _logger;
    bool _play_state = false;
public:
    Client(std::string address, int port, Options options):
        _address(address), 
		_port(port), 
		_options(options),
        _logger(options.log_ostream)
    {
        _connect_socket = socket(static_cast<int>(options.family), static_cast<int>(options.type), 0);

        if (_connect_socket == -1) {
            throw std::runtime_error("[Client] [Error] Connect socket create failed");
        }

        in_addr converted_address;
		int convert_address_error = inet_pton(static_cast<int>(options.family), _address.c_str(), &converted_address);
		if (convert_address_error <= 0) {
			throw std::runtime_error("[Client] [Error] IP translation to special numeric format");
		}

        sockaddr_in server_info;
		memset(&server_info, 0, sizeof(server_info));

		server_info.sin_family = static_cast<int>(options.family);
		server_info.sin_addr = converted_address;
		server_info.sin_port = htons(_port);

        int connect_error = connect(_connect_socket, (sockaddr*)&server_info, sizeof(server_info));
        if (connect_error != 0) {
			throw std::runtime_error("[Client] [Error] Connection to server failed");
		}

        _logger.info("[Client] Created with connection socket: ", _connect_socket);
    }

    ~Client() {
        //close(_connect_socket);
    }

    void start() {
        _logger.info("[Client] Started");
        SocketReader reader(_connect_socket, _options.reader_buffer_size, _logger);
        SocketWriter writer(_connect_socket, _logger);

        try {
            Message access_request(MessageTypes::ACCESS_REQUEST, "");
            writer.send(access_request);

            Message access_response = reader.receive();

            if (access_response.type != MessageTypes::ACCESS_RESPONSE) {
                _logger.error("[Client] [Error] received data without access confirm");
				return;
            }

            _logger.info("[Client] acess granted");

            std::cout << "Commands:"      << std::endl
                    << "\tcreate  <room>" << std::endl
                    << "\tconnect <room>" << std::endl
                    << "\texit"           << std::endl;

            while(working) {
                std::cout << "Input: ";
                std::string input;
                std::getline(std::cin, input);
                
                if (input == "exit") {
                    Message close_request(MessageTypes::CLOSE_REQUEST, "exit command");
                    writer.send(close_request);

                    Message close_response = reader.receive();
                    if (close_response.type != MessageTypes::CLOSE_RESPONSE){

                    }
                    return;
                }

                std::stringstream sin(input);
			    std::string action;
			    sin >> action;

                if (action == "asnwer" && _play_state != true) {
                    std::cout << "You are not in room" << std::endl;
                    continue;
                }

                if ((action == "create" || action == "connect") && _play_state == true) {
                    std::cout << "You already in room" << std::endl;
                    continue;
                }

                Message request(MessageTypes::REUQUEST, input);
                writer.send(request);

                if (action == "create") {
                    Message response = reader.receive();
                    std::cout << response.text << std::endl;
                    if (response.text == std::string(create_error)) {
                        continue;
                    }
                    _play_state = true;
                }

                Message response = reader.receive();
                std::cout << response.text << std::endl;

                if (response.text == std::string(win_message) ) {
                    _play_state = false;
                }

                if (response.text == std::string(lose_message)) {
                    _play_state = false;
                    Message response = reader.receive();
                    std::cout << response.text << std::endl;
                }

                if (response.type == MessageTypes::CLOSE_REQUEST) {
                    _logger.info("[Client] server requested connection closing, reason: ", response.text);
                    Message close_response(MessageTypes::CLOSE_RESPONSE, "");
                    writer.send(close_response);
				    return;
                }
            }
        }
        catch (std::exception &exception) {
		    _logger.error("[Client] [Error] ", exception.what());
        }
    }
};

void interrupt_signal_handler(int value) {
	working = false;
}

int main(int argc, char* argv[]) {
	//signal(SIGINT, interrupt_signal_handler);
    try {
        std::ostream *log_ostream = &std::cout;
        std::ofstream file(std::string("logs/client") + argv[1] + std::string(".log"), std::ios_base::trunc);
        if (file.is_open()) {
            log_ostream = &file;
        }
        decltype(auto) client = Client(
            "127.0.0.1", 
            3301, 
            Client::Options{
				Client::AdressFamily::ipv4,
				Client::SocketType::TCP,
				1000,
				*log_ostream
			}
        );
        client.start();
        file.close();
    } catch(std::exception& exception) {
        std::cout << exception.what() << std::endl;
    }
    return 0;
}