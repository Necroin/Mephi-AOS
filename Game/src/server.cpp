#include "libraries.h"

bool process_working = true;
std::set<int> subprocesses;

struct Room {
	int id;
	int answer;
	int creator;
	int visitor;
};

union RoomBytes {
	Room room;
	char bytes[sizeof(Room)];
};

enum class GameAction: long {
	create = 1,
	connect = 2,
	answer = 3,
};


class ConnectionWorker {
private:
	int socket;
	int _pid;
	MsgQueue server_game_queue;
	MsgQueue response_game_queue;
	Logger logger;
	Room room;
	SocketReader socket_reader;
	SocketWriter socket_writer;
private:
	void handle_request(Message message) {
		if (message.type == MessageTypes::REUQUEST) {
			std::stringstream sin(message.text);
			std::string action;
			size_t room_id;
			sin >> action;
			sin >> room_id;

			if (action == "create") {
				room.id = static_cast<int>(room_id);
				room.answer = 0;
				room.creator = _pid;
				room.visitor = -1;

				RoomBytes room_bytes;
				room_bytes.room = room;
				
				server_game_queue.send(static_cast<long>(GameAction::create), room_bytes.bytes, sizeof(Room));
				return;
			}

			if (action == "connect") {
				room.id = static_cast<int>(room_id);
				room.answer = 0;
				room.creator = -1;
				room.visitor = _pid;

				RoomBytes room_bytes;
				room_bytes.room = room;

				server_game_queue.send(static_cast<long>(GameAction::connect), room_bytes.bytes, sizeof(Room));
				return;
			}

			if (action == "answer") {
				room.answer = static_cast<int>(room_id);

				RoomBytes room_bytes;
				room_bytes.room = room;

				server_game_queue.send(static_cast<long>(GameAction::answer), room_bytes.bytes, sizeof(Room));
				return;
			}

			socket_writer.send(Message(MessageTypes::RESPONSE, "[Error] invalid action"));
			return;
		}

		socket_writer.send(Message(MessageTypes::RESPONSE, "[Error] invalid message type"));
	}

	void worker_loop() {
		_pid = getpid();
		try{
			Message client_request = socket_reader.receive();

			if (client_request.type != ACCESS_REQUEST) {
				logger.error("[Server] [Error] received data without access request");
				return;
			}

			Message access_response(MessageTypes::ACCESS_RESPONSE, "");
			socket_writer.send(access_response);

			int pid = fork();

			if (pid == 0) {
				while(process_working) {
					auto [type, text] = response_game_queue.receive(_pid);
					socket_writer.send(Message(MessageTypes::RESPONSE, std::string(text)));
				}
				exit(0);
			}
			subprocesses.insert(pid);
			std::cout << "worker msg " << pid << std::endl;


			while (process_working) {
				client_request = socket_reader.receive();

				if (client_request.type == MessageTypes::CLOSE_REQUEST) {
                    logger.info("[Server] Client requested connection closing, reason: ", client_request.text);
                    socket_writer.send(Message(MessageTypes::CLOSE_RESPONSE, ""));
					return;
				}

				if (client_request.type == MessageTypes::CLOSE_RESPONSE) {
					return;
				}

				handle_request(client_request);
			}
		}
        catch (std::exception &exception) {
			logger.error("[Server] [Error] -> ", exception.what());
		}
	}
public:
	ConnectionWorker(int socket, size_t buffer_size, MsgQueue server_game_queue, MsgQueue response_game_queue, Logger logger): 
		logger(logger), 
		socket(socket), 
		server_game_queue(server_game_queue),
		response_game_queue(response_game_queue),
		socket_reader(socket, buffer_size, logger),
		socket_writer(socket, logger)
	{}

	void start() {
		logger.info("[Server] [", socket, "] ", "Worker started");

        int pid = fork();

		if (pid == -1) {
			logger.error("[Server] [Error] failed to create worker fork");
		}

        if (pid == 0) {
			subprocesses.clear();
		    logger.info("[Server] [", socket, "] ", "Client connection processor started");
		    worker_loop();
		    logger.info("[Server] [", socket, "] ", "Client disconnected");
		    logger.info("[Server] [", socket, "] ", "Client connection processor finished");
		    close(socket);
			for(auto&& pid: subprocesses) {
				std::cout << "worker kill " << pid << std::endl; 
				kill(pid, SIGKILL);
			}
			exit(0);
        }
		subprocesses.insert(pid);
		std::cout << "worker " << pid << std::endl;

	}
};


class Server {
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
        size_t reader_buffer_size = 8192;
        std::ostream& log_ostream = std::cout;
    };
private:
    bool _is_working = true;
    int _listen_socket;
	std::string _address;
	int _port;
	Options _options;
    Logger _logger;
	std::map<int, Room> _rooms;
	MsgQueue _listen_game_queue;
	MsgQueue _response_game_queue;
public:
    Server(std::string address, int port, Options options):
		_address(address), 
		_port(port), 
		_options(options),
        _logger(options.log_ostream),
		_listen_game_queue("/home/students/c/churkin.ki/Laboratories/AOS/Game", 1, Logger(options.log_ostream)),
		_response_game_queue("/home/students/c/churkin.ki/Laboratories/AOS/Game", 2, Logger(options.log_ostream))

    {
        _listen_socket = socket(static_cast<int>(options.family), static_cast<int>(options.type), 0);

        if (_listen_socket == -1) {
            throw std::runtime_error("[Server] [Error] Listen socket create failed");
        }

        in_addr converted_address;
		int convert_address_error = inet_pton(static_cast<int>(options.family), _address.c_str(), &converted_address);
		if (convert_address_error <= 0) {
			throw std::runtime_error("[Server] [Error] IP translation to special numeric format");
		}

        sockaddr_in server_info;
		memset(&server_info, 0, sizeof(server_info));

		server_info.sin_family = static_cast<int>(options.family);
		server_info.sin_addr = converted_address;
		server_info.sin_port = htons(_port);

		int bind_error = bind(_listen_socket, (sockaddr*)&server_info, sizeof(server_info));
		if (bind_error != 0) {
			throw std::runtime_error("[Server] [Error] socket binding to server failed");
		}

		_logger.info("[Server] Created with listen socket: ", _listen_socket);
    }

    void start() 
    {
		int pid = fork();

		if (pid == 0) {
			try {
				while(process_working) {
					auto [type, data] = _listen_game_queue.receive(0);
					RoomBytes room_bytes;
					memcpy(room_bytes.bytes, data, sizeof(Room));

					if (type == static_cast<long>(GameAction::create)) {
						if (_rooms.find(room_bytes.room.id) != _rooms.end()) {
							_response_game_queue.send(room_bytes.room.creator, create_error, sizeof(create_error));
							continue;
						}

						std::random_device rd;
    					std::mt19937 gen(rd()); 
    					std::uniform_int_distribution<> distrib(1, 100);
						int number = distrib(gen);
						room_bytes.room.answer = number;
						_rooms.insert({room_bytes.room.id, room_bytes.room});

						_logger.info("[Server] Game ", room_bytes.room.id, " started with answer ", number);

						_response_game_queue.send(room_bytes.room.creator, create_response, sizeof(create_response));
					}

					if (type == static_cast<long>(GameAction::connect)) {
						auto room_it = _rooms.find(room_bytes.room.id);
						if (room_it == _rooms.end()) {
							_response_game_queue.send(room_bytes.room.visitor, connect_error, sizeof(connect_error));
							continue;
						}

						auto& room = room_it->second;
						room.visitor = room_bytes.room.visitor;

						_response_game_queue.send(room.visitor, connect_response, sizeof(connect_response));

						_response_game_queue.send(room.creator, other_connect_response, sizeof(other_connect_response));
					}

					if (type == static_cast<long>(GameAction::answer)) {
						int sender = -1;

						if (room_bytes.room.creator != -1) { sender = room_bytes.room.creator; }
						if (room_bytes.room.visitor != -1) { sender = room_bytes.room.visitor; }

						auto room_it = _rooms.find(room_bytes.room.id);
						if (room_it == _rooms.end()) {
							char message[] = "[Error] Room not exist";
							_response_game_queue.send(sender, message, sizeof(message));
							continue;
						}

						auto& room = room_it->second;

						int other = -1;

						if (room_bytes.room.creator != -1) { other = room.visitor; }
						if (room_bytes.room.visitor != -1) { other = room.creator; }

						if (room_bytes.room.answer == room.answer) {
							_response_game_queue.send(sender, win_message, sizeof(win_message));
							_response_game_queue.send(other, lose_message, sizeof(lose_message));
							_rooms.erase(room.id);
						}

						if (room_bytes.room.answer > room.answer) {
							char message[] = "Less";
							_response_game_queue.send(sender, message, sizeof(message));
						}

						if (room_bytes.room.answer < room.answer) {
							char message[] = "More";
							_response_game_queue.send(sender, message, sizeof(message));
						} 
					}

				}
			}
			catch (std::exception &exception) {
				_logger.error("[Server] [Error] -> ", exception.what());
			}
			exit(0);
		}

		subprocesses.insert(pid);
		std::cout << "start " << pid << std::endl;
		


        int listen_error = listen(_listen_socket, SOMAXCONN);
		
		if (listen_error != 0) {
			throw std::runtime_error("[Server] [Error] Can't start to listen");
		}

		_logger.info("[Server] Server listen started");

		while (process_working) {
			sockaddr_in client_info;
			memset(&client_info, 0, sizeof(client_info));

			socklen_t client_info_size = sizeof(client_info);

			int client_connection = accept(_listen_socket, (sockaddr*)&client_info, &client_info_size);
			if (client_connection == -1) {
		    	_logger.info("[Server] [Error] accept error");
				continue;
			}

			_logger.info("[Server] Client connected with socket: ", client_connection);
            ConnectionWorker(client_connection, _options.reader_buffer_size, _listen_game_queue, _response_game_queue, _logger).start();
			close(client_connection);
		}
    }
};

void child_signal_handler(int value) {
	int pid;
    pid = waitpid(-1, NULL, 0);
	std::cout << "erase " << pid << std::endl;
	subprocesses.erase(pid);
	std::cout << "child end" << std::endl;

}

void interrupt_signal_handler(int value) {
	process_working = false;
	for(auto&& pid: subprocesses) {
		kill(pid, SIGINT);
	}
	exit(0);
}


void sighup_signal_handler(int value) {
	for(auto&& pid: subprocesses) {
		kill(pid, SIGHUP);
	}
}

int main(int argc, char *argv[]) {
	struct sigaction signal_action;

    signal_action.sa_flags = SA_RESTART;
    signal_action.sa_restorer = NULL;

    signal_action.sa_handler = child_signal_handler;
    sigemptyset(&signal_action.sa_mask);
    sigaction(SIGCHLD, &signal_action, NULL);

    signal_action.sa_handler = interrupt_signal_handler;
    sigemptyset(&signal_action.sa_mask);
    sigaction(SIGINT, &signal_action, NULL);

    signal_action.sa_handler = sighup_signal_handler;
    sigemptyset(&signal_action.sa_mask);
    sigaction(SIGHUP, &signal_action, NULL);

    if (argc != 1) {
        std::cout << "usage: server" << std::endl;
        return 0;
    }

    try {
		std::ostream *log_ostream = &std::cout;
        std::ofstream file("logs/server.log", std::ios_base::trunc);
        if (file.is_open()) {
            log_ostream = &file;
        }
        decltype(auto) server = Server(
			"127.0.0.1", 
			3301, 
			Server::Options{
				Server::AdressFamily::ipv4,
				Server::SocketType::TCP,
				1000,
				*log_ostream
			}
		);
        server.start();
    }
    catch (std::exception &exception) {
        std::cout << exception.what() << std::endl;
    }

    return 0;
}