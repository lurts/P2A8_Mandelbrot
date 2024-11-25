#pragma once
#include <ccomplex>
#include "SFML/Graphics.hpp"

class Mandelbrot {
public:
	/* --Vereinfachte Schreibweise. */
	using Complex = std::complex<double>;

	/* --Konstruktor mit Rechteck (Rechengebiet). */
	Mandelbrot(const Complex& c0, const Complex& c1) :_c0(c0), _c1(c1) {}

	/* --Berechne ein Bild in Form einzelner Knoten. */
	sf::VertexArray calculate(const sf::Rect<float>& area, unsigned char iterations = 255) const;

	// berechne einen ausschitt
	// wird von der jetzigen version genutzt um multithreading zu realisieren
	void calculatePartial(	sf::VertexArray& pixels, 
							const sf::Rect<float>& area, 
							unsigned int startY, unsigned int endY, 
							unsigned char iterations) const;

	/* --Setze das Berechnungsgebiet. */
	void set(const Complex& c0, const Complex& c1) { _c0 = c0; _c1 = c1; }

private:
	/* --Wertebereich. */
	Complex _c0;
	Complex _c1;

	/* --Liefert Anzahl der Iterationen zurück. */
	unsigned char iterate(const Complex& c, unsigned char iterations = 255) const;
};

