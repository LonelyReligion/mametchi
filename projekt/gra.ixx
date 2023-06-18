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

	ekran widok_gry;
	bool wynik;

	std::vector<przycisk> przyciski;
public:
	prawo_lewo(std::filesystem::path p, sf::Font& font) {
		wczytaj_sprite(p);

		sf::Text tytul("Prawo czy Lewo?", font, 50);
		tytul.setFillColor(sf::Color(0, 77, 0));
		tytul.setOrigin(sf::Vector2f(-225.f, -100.f));

		przyciski.push_back(przycisk("PRAWO", { 200,100 }, 20, sf::Color(238, 255, 204), sf::Color(17, 26, 0), { 0,0 }, font));
		przyciski.push_back(przycisk("LEWO", { 200, 100 }, 20, sf::Color(238, 255, 204), sf::Color(17, 26, 0), { 0,0 }, font));

		widok_gry.wczytaj_tlo("OBRAZKI/gaming.png");
		widok_gry.ustaw_teksty({ tytul });
		widok_gry.ustaw_przyciski({ &przyciski[0], &przyciski[1]});
		//ustalamy wynik gry
	};

	virtual void wczytaj_sprite(std::filesystem::path & p) {
		if (!bobas_lewo.loadFromFile(p.string())) {
			std::cout << "ladowanie tekstury stworka bioracego udzial w grze zakonczone niepowodzeniem" << std::endl;
		};
		bobas_lewo.setSmooth(false);
		stworek.setTexture(bobas_lewo);
		stworek.setOrigin(sf::Vector2f(-300.f, -275.f));
		stworek_prawo = stworek;
		stworek_prawo.setTextureRect(sf::IntRect(200, 0, -200, 200));
		stworek_prawo.setOrigin(sf::Vector2f(-300.f, -275.f));
	};

	sf::Sprite animacja_stworka() {
		static std::vector<sf::Sprite> obroty = { stworek, stworek_prawo };
		static bool moneta = 1;
		static int licznik = 50;
		if (!licznik) {
			moneta = !moneta;
			licznik = 50;
		}else
			licznik--;
		return obroty[moneta];
	};

	void ustaw_ekran(ekran& e) { widok_gry = e; };

	void rysuj(sf::RenderWindow& okno) {
		widok_gry.rysuj_tlo(okno);
		okno.draw(animacja_stworka());
	};

	int zwroc_nagrode(bool & obstawiana_wartosc) {
		if (obstawiana_wartosc == wynik)
			return 100;
		else
			return 0;
	};
};;