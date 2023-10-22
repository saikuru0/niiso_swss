#include <iostream>

#include "simplewss/client_wss.hpp"
#include "niiso/niiso.hpp"

#ifdef DEV
#define DEBUG(x) std::cout << x << std::endl
#else
#define DEBUG(x)
#endif

using WssClient = SimpleWeb::SocketClient<SimpleWeb::WSS>;

namespace config {
	std::string uri;
	std::string uid;
	std::string auth;
};

int main(int argc, char** argv) {
	if (argc != 4) {
		std::cout << "Wrong usage, needs exactly 3 args: " << argv[0] << " chat_url user_id auth_key\n";
		exit(EXIT_FAILURE);
	}
	config::uri =  argv[1];
	config::uid = argv[2];
	config::auth = argv[3];
	Niiso* niiso = new Niiso(config::uri, config::uid, config::auth);
	std::shared_ptr<WssClient::Connection> conn;
	WssClient client(config::uri, true);

	client.on_message = [&niiso](std::shared_ptr<WssClient::Connection> connection, std::shared_ptr<WssClient::InMessage> in_message) {
		niiso->add(in_message->string());
		// connection->send_close(1000);
	};

	client.on_open = [&niiso, &conn](std::shared_ptr<WssClient::Connection> connection) {
		// sleep(1);
		niiso->join();
		conn = connection;
	};

	client.on_close = [](std::shared_ptr<WssClient::Connection> /*connection*/, int status, const std::string & /*reason*/) {
		std::cout << "WSS: Closed connection with status code " << status << std::endl;
		exit(0);
	};

	client.on_error = [](std::shared_ptr<WssClient::Connection> /*connection*/, const SimpleWeb::error_code &ec) {
		std::cout << "WSS: Error: " << ec << ", error message: " << ec.message() << std::endl;
		exit(-1);
	};

	std::thread ping([&niiso]() {
		while(1) {
			sleep(20);
			niiso->ping();
		}
	});

	std::thread gui([&niiso]() {
		std::string input("");
		while(1) {
			getline(std::cin, input);
			if (input == "/dc") {
				continue;
			}
			if (input == "/join") {
				continue;
			}
			if (input == "/exit") {
				continue;
			}
			niiso->send(input);
		}
	});

	std::thread bot([&niiso, &conn]() {
		while(1) {
			usleep(250000);
			niiso->serve();
			std::string msg = niiso->next_msg();
			while(msg != "") {
				conn->send(msg);
				msg = niiso->next_msg();
			}
		}
	});

	client.start();

	bot.join();
	gui.join();
	ping.join();

	return 0;
}
