/*********************************************************************
 * @file  gra.ixx
 *
 * @brief Implementacja i deklaracja klasy prawo_lewo.
 *********************************************************************/

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <filesystem>
#include <iostream>
#include <random>

import interfejs;
import zwierzak;

export module gra;
// singleton?
/**
* @class prawo_lewo
* @brief Klasa reprezentujaca gre prawo-lewo.
*/
export class prawo_lewo {
private:
	sf::Sprite stworek;
	sf::Sprite stworek_prawo;
	
	//nie musi byc bobas
	sf::Texture bobas_lewo; ///< tektura stworzenia (niekoniecznie bobasa) obroconego w lewo
	sf::Texture bobas_prawo;

	ekran widok_gry;
	std::vector<przycisk> przyciski;
	std::vector<sf::Sprite> obroty;

	sf::Font czcionka;
public:
	/**
	* @brief Wywoluje wczytaj_sprite(sciezka) i  wczytuje ekran gry.
	*
	* @param p to sciezka wskazujaca na sprite stworzenia obroconego w lewo.
	* @param font to uzywany font.
	*/
	prawo_lewo(std::filesystem::path p, sf::Font& font) {
		wczytaj_sprite(p);

		czcionka = font; 

		przyciski.push_back(przycisk("PRAWO", { 150,75 }, 30, sf::Color(238, 255, 204), sf::Color(17, 26, 0), { 450, 425 }, font));
		przyciski.push_back(przycisk("LEWO", { 150, 75 }, 30, sf::Color(238, 255, 204), sf::Color(17, 26, 0), { 200, 425 }, font));
		
		widok_gry.ustaw_przyciski({ &przyciski[1], &przyciski[0]});
	};

	/**
	* @brief Wczytuje sprite stworzenia bioracego udzial w grze. Stworzenie obrocone w prawo to lustrzane odbicie wczytanej tekstury.
	*
	* @param p to sciezka wskazujaca na sprite stworzenia obroconego w lewo.
	*/
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

	/**
	* @brief Zwraca informacje czy stworzenie ma byc obrocone w prawo czy w lewo.
	*
	*/
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

	/**
	* @brief Wywoluje wczytaj_sprite(sciezka) i  wczytuje ekran gry.
	*
	* @param obstawiana_wartosc reprezentuje wybor gracza.
	* @param gracz - wskaznik na stworzenie biorace udzial w grze.
	* 
	* @return punkty, ktore w mainie zostana przypisane do konta uzytkownika.
	*/
	int zwroc_nagrode(bool obstawiana_wartosc, stworzenie * gracz) {
		if (obstawiana_wartosc == animacja_stworka()) {
			widok_gry.wczytaj_tlo("OBRAZKI/sala_gier/prawo_lewo/gaming_wygrana.png");
			wczytaj_sprite("OBRAZKI/POSTACI/NIEMOWLE_RADOSC.png");
			widok_gry.ustaw_teksty({ sf::Text() });
			(*gracz).ustaw_szczescie((*gracz).zwroc_szczescie() + 1);
			return 100;
		}
		else {
			widok_gry.wczytaj_tlo("OBRAZKI/sala_gier/prawo_lewo/gaming_przegrana.png");
			wczytaj_sprite("OBRAZKI/POSTACI/NIEMOWLE_SMUTEK.png");
			widok_gry.ustaw_teksty({ sf::Text() });
			return 0;
		};
	};

	void reset() {
		sf::Text tytul("Prawo czy Lewo?", czcionka, 50);
		tytul.setFillColor(sf::Color(0, 77, 0));
		tytul.setOrigin(sf::Vector2f(-225.f, -100.f));

		widok_gry.wczytaj_tlo("OBRAZKI/sala_gier/prawo_lewo/gaming.png");
		widok_gry.ustaw_teksty({ tytul });
	};
};;