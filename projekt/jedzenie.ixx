#include <iostream>
#include <filesystem>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

export module jedzenie;

export class produkt {
private:
	int wartosc_odzywcza;
	int radosc;

	sf::Texture tekstura;
	sf::Sprite duch;
	sf::Vector2f pozycja;
public:
	produkt(const int & wo, const int & r, const std::filesystem::path & p) : wartosc_odzywcza(wo), radosc(r){
		wczytaj(p);
	};

	int zwroc_wo() const { return wartosc_odzywcza; };
	int zwroc_r() const { return radosc; };

	void wczytaj(const std::filesystem::path& sciezka) {
		if (!tekstura.loadFromFile(sciezka.string())) {
			std::cout << "ladowanie tekstury tla zakonczone niepowodzeniem" << std::endl;
		};
		tekstura.setSmooth(false);
		duch.setTexture(tekstura);
	};

	void rysuj(sf::RenderWindow& okno, sf::Vector2f gdzie) {
		duch.setOrigin(gdzie);
		okno.draw(duch);
	};
};