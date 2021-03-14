#include <iostream>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <map>
#include <vector>
#include <variant>
#include <cstring>
#include <random>

#include "gamestate.hpp"
#include "gamedrawer.hpp"
#include "event.hpp"
#include "config.hpp"
#include "gameconfig.hpp"
#include "utils.hpp"

void try_receiveing_events(std::vector<Event>&, std::unique_ptr<sf::UdpSocket>&);
void send_our_events(std::vector<Event>&, GameConfig&, std::unique_ptr<sf::UdpSocket>&);
std::vector<Event> handle_input(GameState& game_state, sf::RenderWindow& window, u64, bool, u64);
std::pair<u64, std::unique_ptr<sf::UdpSocket>> connect_to_server(GameConfig& config);

int run_game(GameConfig& config) {
    std::cout << "Running game!\n";

    std::vector<Event> events;
    GameState game_state;

    auto&& [player_id, sock] = connect_to_server(config);
    std::cout << "I got player_id: " << player_id << '\n';

    {
        sf::TcpSocket tcp_sock;
        tcp_sock.connect(config.server_address, sock->getLocalPort());
        while (true) {
            sf::Packet pack;
            auto status = tcp_sock.receive(pack);
            if (status != sf::Socket::Status::Done)
                break;
            Event e;
            pack >> e;
            events.push_back(e);
        }
        tcp_sock.disconnect();
    }
    std::cout << "applying " << events.size() << " events\n";
    game_state.apply_events(events);
    events.clear();


	unsigned window_width = 1024;
	unsigned window_height = 1024;

	sf::RenderWindow window(sf::VideoMode(window_width, window_height), "SFML works!");

    GameDrawer game_drawer;
	game_drawer.fill_dirt(window);


	sf::Clock delta_clock;
	float dt = 0.0f;

    sf::Clock tick_clock;

    bool wasclick = false;

    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;
    bool first = true;

    while (window.isOpen())
    {
		dt = delta_clock.restart().asMilliseconds();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::Resized)
			{
				auto sz = window.getSize();
				window_width = sz.x;
				window_height = sz.y;
			}
			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Escape)
				{
					window.close();
				}
			}

            if(event.type == sf::Event::MouseButtonPressed) {
                wasclick = true;
            }
        }

        try_receiveing_events(events, sock);
        if (first) {
            std::cout << "wszystkie = " << events.size() << std::endl;
            first = false;
        }

        if(tick_clock.getElapsedTime().asSeconds() >= TICK_TIME) {
            tick_clock.restart();

            game_state.apply_events(events);
            events.clear();

            std::vector<Event> our_events = handle_input(game_state, window, player_id, wasclick, dis(gen));

            game_state.apply_events(our_events);

            send_our_events(our_events, config, sock);

            wasclick = false;
        }

        game_drawer.draw(game_state, window, player_id);
    }

    return 0;
}

void try_receiveing_events(std::vector<Event>& cur_events, std::unique_ptr<sf::UdpSocket>& sock) {
    sf::Packet packet;
    sf::IpAddress sender_addr;
    u16 sender_port;
    while(sock->receive(packet, sender_addr, sender_port) == sf::Socket::Done) {
        Event event;
        packet >> event;
        cur_events.push_back(event);
    }
}

void send_our_events(std::vector<Event>& our_events, GameConfig& config,
                     std::unique_ptr<sf::UdpSocket>& sock) {
    for(auto&& event: our_events) {
        sf::Packet packet;
        packet << event;
        sock->send(packet, config.server_address, config.server_port);
    }
}

std::vector<Event> handle_input(GameState& game_state, sf::RenderWindow& window,
                                u64 player_id, bool wasclick, u64 bullet_id) {

    vec2f mouse_pos = vec2f(sf::Mouse::getPosition(window));
    vec2f window_center = vec2f(window.getSize())/2.f;
    vec2f look_dir = normalize(mouse_pos - window_center);

    vec2f moving_dir(0.0f, 0.0f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        moving_dir += vec2f(0.0f, -1.0f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        moving_dir += vec2f(1.0f, 0.0f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        moving_dir += vec2f(-1.0f, 0.0f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        moving_dir += vec2f(0.0f, 1.0f);

    float moving_dir_len = (float)moving_dir.x * moving_dir.x + moving_dir.y * moving_dir.y;
    moving_dir_len = sqrt(moving_dir_len);

    if (moving_dir_len > 0.0f)
    {
        moving_dir /= moving_dir_len;
    }

    f32 sped = PLAYER_SPEED;

    vec2f my_new_pos = game_state.players[player_id].pos + float(TICK_TIME) * sped *  moving_dir;

    std::vector<Event> my_events;

	if (my_new_pos.x >= 0 && my_new_pos.x < 1024 && my_new_pos.y >= 0 && my_new_pos.y < 1024)
	{
		my_events.push_back(Event {
			.tick = 3,
			.content = PlayerPos {
				.player_id = player_id,
				.pos = my_new_pos,
                .look_dir = look_dir
			}
		});
	}

    if(wasclick) {
        vec2f bullet_pos = my_new_pos + 50.f * look_dir;

        my_events.push_back(Event {
            .tick = 3,
            .content = BulletShot {
                .bullet_id = bullet_id,
                .pos = bullet_pos,
                .direction = look_dir
            }
        });
    }

    return my_events;
}

std::pair<u64, std::unique_ptr<sf::UdpSocket>> connect_to_server(GameConfig& config) {
    std::cout << "Connecting to srever on: " << config.server_address << ":"
        << config.server_port << " with username: " << config.username << '\n';

    std::unique_ptr<sf::UdpSocket> sock = std::make_unique<sf::UdpSocket>();

    Event hello_event = Event{
        .tick = 0,
        .content = Hello {
            .username = config.username
        }
    };
    sf::Packet hello_packet;
    hello_packet << hello_event;

    sock->send(hello_packet, config.server_address, config.server_port);

    sf::Packet response_packet;
    sf::IpAddress remote_addr;
    u16 remote_port;

    sock->receive(response_packet, remote_addr, remote_port);

    Event response_event;
    response_packet >> response_event;

    HelloResponse hello_response = std::get<HelloResponse>(response_event.content);


    PlayerStatsChange ps = PlayerStatsChange {
        .player_id = hello_response.player_id,
        .dead = false,
        .score = 0,
        .name = config.username,
    };

    Event stateevent = Event  {
        .tick = 0,
        .content = ps
    };

    sf::Packet state_pack;
    state_pack << stateevent;

    sock->send(state_pack, config.server_address, config.server_port);

    sock->setBlocking(false);

    std::pair<u64, std::unique_ptr<sf::UdpSocket>> para;
    para.first = hello_response.player_id;
    para.second = std::move(sock);

    return para;
}
