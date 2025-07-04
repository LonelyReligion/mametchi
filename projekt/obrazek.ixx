#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <filesystem>
#include <iostream>

import klikalny;

export module obrazek;

export class obrazek : public klikalny{
private:
	std::filesystem::path sciezka;
	sf::Texture tekstura;
	sf::Sprite duch;
protected:

public:
	obrazek(std::filesystem::path p) { 
		sciezka = p;
		wczytaj(p); 
	}

	void wczytaj(const std::filesystem::path sciezka) {
		if (!tekstura.loadFromFile(sciezka.string())) {
			std::cout << "ladowanie tekstury obrazka zakonczone niepowodzeniem" << std::endl;
		};
		tekstura.setSmooth(false);
	};

	void rysuj(sf::RenderWindow& okno, sf::Vector2f gdzie, bool odbij_w_pionie) {
		duch.setTexture(tekstura);
		duch.setPosition(-gdzie);

		if (odbij_w_pionie) {
			duch.setTextureRect(sf::IntRect(duch.getLocalBounds().height, 0, -duch.getLocalBounds().width, duch.getLocalBounds().height));
		}
		okno.draw(duch);

		guzikx = -gdzie.x;
		guziky = -gdzie.y;

		xplusszer = guzikx + duch.getLocalBounds().width;
		ypluswys = guziky + duch.getLocalBounds().height;
	};


};