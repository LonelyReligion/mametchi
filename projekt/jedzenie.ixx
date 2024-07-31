#include <iostream>
#include <filesystem>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

export module jedzenie;

export class produkt {
private:
	std::string nazwa; //unikalna nazwa dania
	int wartosc_odzywcza;
	int radosc;
	int cena;

	std::filesystem::path sciezka;
	sf::Texture tekstura;
	sf::Sprite duch;
public:
	produkt() : wartosc_odzywcza(0), radosc(0), nazwa("") {};
	produkt(const int & wo, const int & r, std::filesystem::path p, const std::string & n, int ects) : wartosc_odzywcza(wo), radosc(r), nazwa(n) {
		sciezka = p;
		cena = ects;
		wczytaj(sciezka);
	};

	int zwroc_wo() const { return wartosc_odzywcza; };
	int zwroc_r() const { return radosc; };
	std::string zwroc_nazwa() { return nazwa;  };
	void wczytaj(const std::filesystem::path sciezka) {
		if (!tekstura.loadFromFile(sciezka.string())) {
			std::cout << "ladowanie tekstury jedzenia zakonczone niepowodzeniem" << std::endl;
		};
		tekstura.setSmooth(false);
	};

	void rysuj(sf::RenderWindow& okno, sf::Vector2f gdzie) {
		duch.setTexture(tekstura);
		duch.setOrigin(gdzie);
		okno.draw(duch);
	};

	int zwroc_cene() { return cena; }
	std::string zwroc_opis(){ 
		return (nazwa + "\nwartosc odzywcza: " + std::to_string(wartosc_odzywcza) + "\nradosc: " + std::to_string(radosc));
	};
};