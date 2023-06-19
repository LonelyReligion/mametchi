#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <filesystem>
#include <iostream>
#include <random>

import interfejs;

export module gra;
// singleton?
export class prawo_lewo {
private:
	sf::Sprite stworek;
	sf::Sprite stworek_prawo;
	
	sf::Texture bobas_lewo;//nie musi byc bobas
	sf::Texture bobas_prawo;

	ekran widok_gry;
	std::vector<przycisk> przyciski;
	std::vector<sf::Sprite> obroty;
public:
	prawo_lewo(std::filesystem::path p, sf::Font& font) {
		wczytaj_sprite(p);

		sf::Text tytul("Prawo czy Lewo?", font, 50);
		tytul.setFillColor(sf::Color(0, 77, 0));
		tytul.setOrigin(sf::Vector2f(-225.f, -100.f));

		przyciski.push_back(przycisk("PRAWO", { 150,75 }, 30, sf::Color(238, 255, 204), sf::Color(17, 26, 0), { 450, 425 }, font));
		przyciski.push_back(przycisk("LEWO", { 150, 75 }, 30, sf::Color(238, 255, 204), sf::Color(17, 26, 0), { 200, 425 }, font));

		widok_gry.wczytaj_tlo("OBRAZKI/gaming.png");
		widok_gry.ustaw_teksty({ tytul });
		widok_gry.ustaw_przyciski({ &przyciski[1], &przyciski[0]});
	};

	void wczytaj_sprite(std::filesystem::path p) {
		if (!bobas_lewo.loadFromFile(p.string())) {
			std::cout << "ladowanie tekstury stworka bioracego udzial w grze zakonczone niepowodzeniem" << std::endl;
		};
		bobas_lewo.setSmooth(false);
		stworek.setTexture(bobas_lewo);
		stworek.setOrigin(sf::Vector2f(-300.f, -200.f));
		stworek_prawo = stworek;
		stworek_prawo.setTextureRect(sf::IntRect(200, 0, -200, 200));
		obroty.push_back(stworek);
		obroty.push_back(stworek_prawo);
	};

	std::vector<przycisk*> zwroc_przyciski() {
		return widok_gry.zwroc_przyciski();
	};

	bool animacja_stworka() {
		static bool moneta = 1;
		static int licznik = 50;
		if (!licznik) {
			moneta = !moneta;
			licznik = 50;
		}else
			licznik--;
		return moneta;
	};

	void ustaw_ekran(ekran& e) { widok_gry = e; };

	void rysuj(sf::RenderWindow& okno) {
		widok_gry.rysuj_tlo(okno);
		okno.draw(obroty[animacja_stworka()]);
	};

	int zwroc_nagrode(bool obstawiana_wartosc) {
		if (obstawiana_wartosc == animacja_stworka()) {
			wczytaj_sprite("OBRAZKI/POSTACI/NIEMOWLE_RADOSC.png");
			return 100;
		}
		else {
			wczytaj_sprite("OBRAZKI/POSTACI/NIEMOWLE_SMUTEK.png");
			return 0;
		};
	};
};;