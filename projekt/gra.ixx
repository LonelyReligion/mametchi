#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <filesystem>
#include <iostream>
#include <random>

import interfejs;
import zwierzak;

export module gra;
// singleton?
export class prawo_lewo {
private:
	sf::Sprite stworek;
	sf::Sprite stworek_prawo;
	sf::Sprite stworek_w_gorze;
	sf::Sprite stworek_przesuniety;

	sf::Texture bobas_lewo;//nie musi byc bobas
	sf::Texture bobas_prawo;

	ekran widok_gry;
	std::vector<przycisk> przyciski;
	
	sf::Sprite obroty[2];
	sf::Sprite skoki[2];
	sf::Sprite trzesienie[2];
public:
	int wynik = 0; //0 - gramy, 1 - wygralismy, -1 - przegralismy

	prawo_lewo(sf::Font& font) {
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

		obroty[0] = stworek;
		obroty[1] = stworek_prawo;

		stworek_w_gorze = stworek;
		stworek_w_gorze.setOrigin(sf::Vector2f(-300.f, -180.f));
		skoki[0] = stworek;
		skoki[1] = stworek_w_gorze;

		stworek_przesuniety = stworek;
		stworek_przesuniety.setOrigin(sf::Vector2f(-320.f, -200.f));
		trzesienie[0] = stworek;
		trzesienie[1] = stworek_przesuniety;
	};

	std::vector<przycisk*> zwroc_przyciski() {
		return widok_gry.zwroc_przyciski();
	};

	//moze zmienic po prostu na licznik i modulo :')
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

		if (wynik == 0)
			okno.draw(obroty[animacja_stworka()]);
		else if(wynik == 1)
			okno.draw(skoki[animacja_stworka()]);
		else
			okno.draw(trzesienie[animacja_stworka()]);
	};

	int zwroc_nagrode(bool obstawiana_wartosc, stworzenie * gracz) {
		if (obstawiana_wartosc == animacja_stworka()) {
			wynik = 1;
			wczytaj_sprite((*gracz).pobierz_wygrana());
			
			(*gracz).ustaw_szczescie((*gracz).zwroc_szczescie() + 1);
			int tmp = (*gracz).get_wygrane_pod_rzad();
			(*gracz).inkrementuj_wygrane_pod_rzad();
			return 100 + tmp*10;
		}
		else {
			wynik = -1;
			wczytaj_sprite((*gracz).pobierz_przegrana());
			
			(*gracz).zeruj_wygrane_pod_rzad();
			return 0;
		};
	};
};;