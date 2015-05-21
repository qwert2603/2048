#include "stdafx.h"
#include "Field2048.h"
#include <SFML\Graphics.hpp>

// размер плитки
const double tile_size = 128;
// размер поля
const unsigned N = 5;

int main() {
	field_2048::Field2048<N> f;	// поле
	
	// само окно
	sf::RenderWindow window(sf::VideoMode(tile_size*N, tile_size*N), "_2048");

	// для картинок плиток
	sf::Image image;
	image.loadFromFile("images/tiles.png");
	image.createMaskFromColor(sf::Color::White);
	sf::Texture texture;
	texture.loadFromImage(image);
	sf::Sprite sprite(texture);
	// для пустоты
	sf::RectangleShape emptiness(sf::Vector2f(tile_size, tile_size));
	emptiness.setFillColor(sf::Color::White);

	bool is_game = true;	// ? игра идет (еще не завалился)

	while (window.isOpen()) {
		sf::Event _event;
		while (window.pollEvent(_event)) {
			if (_event.type == sf::Event::Closed) {
				window.close();
			}
			if (_event.type == sf::Event::KeyPressed) {
				if (is_game) {
					if (_event.key.code == sf::Keyboard::W) {
						is_game = f.move_up();
					}
					else if (_event.key.code == sf::Keyboard::S) {
						is_game = f.move_down();
					}
					else if (_event.key.code == sf::Keyboard::A) {
						is_game = f.move_left();
					}
					else if (_event.key.code == sf::Keyboard::D) {
						is_game = f.move_right();
					}
				}
			}
			if (_event.type == sf::Event::MouseButtonPressed) {
				if (_event.mouseButton.button == sf::Mouse::Middle) {
					f.restart();
					is_game = true;
				}
			}
			if (_event.type == sf::Event::Resized) {
				// размер окна менять нельзя
				window.setSize(sf::Vector2u(tile_size*N, tile_size*N));
			}
		}

		auto tiles = f.get_tiles();

		window.clear(is_game ? sf::Color::White : sf::Color::Red);
		double x = 0, y = 0;
		for (const auto &one_row : tiles) {
			for (const auto &one_tile : one_row) {
				if (one_tile) {
					sprite.setPosition(x, y);
					sprite.setTextureRect(sf::IntRect(tile_size*((one_tile - 1) % 4), tile_size*((one_tile - 1) / 4), tile_size, tile_size));
					window.draw(sprite);
				}
				else {
					emptiness.setPosition(x, y);
					window.draw(emptiness);
				}
				x += tile_size;
			}
			x = 0;
			y += tile_size;
		}
		
		window.display();
	}

	return 0;
}
