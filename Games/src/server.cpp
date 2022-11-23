// cpp libraries
#include <string>
#include <iostream>
#include <map>
// unix libraries
#include <sys/types.h>
#include <sys/socket.h>

namespace log {
    class Logger {
	private:
		std::ostream& _out;
	public:
		Logger(std::ostream& out) : _out(out) {}

		template<class... _LogArgs>
		void info(_LogArgs&&... logs) {
			((_out << std::forward<_LogArgs>(logs)), ...);
			_out << std::endl;
		}

		template<class... _LogArgs>
		void error(_LogArgs&&... logs) {
			((_out << std::forward<_LogArgs>(logs)), ...);
			_out << std::endl;
		}
	};
}

class Server {
public:
    class ConnectionWorker {
	private:
		Server& _server;
		SOCKET _socket;
		thread::WaitGroup& _connect_wait;
	public:
		ConnectionWorker(Server& server, SOCKET socket, thread::WaitGroup& connect_wait) : 
			_server(server), _socket(socket), _connect_wait(connect_wait)
		{}

		void start() {
            int pid = fork();
            if (pid == 0) {
			    _server._log.info("[Server] [", _socket, "] ", "Client connection processor started");
			    
                
			    _server._log.info("[Server] [", _socket, "] ", "Client disconnected");
			    _server._log.info("[Server] [", _socket, "] ", "Client connection processor finished");
			    closesocket(_socket);
			    _connect_wait.done();
            }
		}
	};

    friend class ConnectionWorker;

private:
    bool _is_working = true;
    SOCKET _listen_socket;

    enum class AdressFamily : int
    {
        ipv4 = AF_INET
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
        std::ostream &log_ostream = std::cout;
    };

    log::Logger _log;
public:
    Server(std::string address, int port, Options options = Options()):
        _log(options.log_ostream)
    {
        _listen_socket = socket(static_cast<int>(options.family), static_cast<int>(options.type), 0);

        if (_listen_socket == INVALID_SOCKET) {
            throw std::runtime_error("[Server] [Error] Listen socket create failed");
        }

        in_addr converted_address;
		decltype(auto) convert_address_error = inet_pton(static_cast<int>(options.family), _address.c_str(), &converted_address);
		if (convert_address_error <= 0) {
			throw std::runtime_error("[Server] [Error] IP translation to special numeric format");
		}

        sockaddr_in server_info;
		ZeroMemory(&server_info, sizeof(server_info));

		server_info.sin_family = static_cast<int>(options.family);
		server_info.sin_addr = converted_address;
		server_info.sin_port = htons(_port);

		decltype(auto) bind_error = bind(_listen_socket, (sockaddr*)&server_info, sizeof(server_info));
		if (bind_error != 0) {
			throw std::runtime_error("[Server] [Error] Socket binding to server failed");
		}

		_log.info("[Server] Started with listen socket: ", _listen_socket);
    }

    void start() 
    {
        int listen_error = listen(_listen_socket, SOMAXCONN);
		
		if (listen_error != 0) {
			throw std::runtime_error("[Server] [Error] Can't start to listen");
		}

		_log.info("[Server] Server listen started");

		while (_is_working) {
			sockaddr_in client_info;
			ZeroMemory(&client_info, sizeof(client_info));

			int client_info_size = sizeof(client_info);

			SOCKET client_connection = accept(_listen_socket, (sockaddr*)&client_info, &client_info_size);

			if (client_connection == INVALID_SOCKET) {
				continue;
			}

			_server._log.info("[Server] Client connected with socket: ", client_connection);
            ConnectionWorker(*this, client_connection, )
			
		}
    }
};

int main(int argc, char *argv[]) {
    if (argc != 1) {
        std::cout << "usage: server" << std::endl;
        return 0;
    }

    try {
        decltype(auto) server = Server("127.0.0.1",3301);
        server.start();
    }
    catch (std::exception &exception) {
        std::cout << exception.what() << std::endl;
    }

    return 0;
}