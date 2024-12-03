#include "mandelbrot.h"

/* --Vereinfachte Schreibweise. */
using Complex = std::complex<double>;

// zus�tzliche funktion um farben zu konvertieren
// gro�teil von ChatGPT aber mit einigen anpassungen
sf::Color hsvToColor(float h, float s, float v) {
	float r = 0, g = 0, b = 0;

    // werte in gültigen bereich clampen
    h = (h < 0.0f) ? 0.0f : (h > 360.0f ? 360.0f : h); // h in [0, 360]
    s = (s < 0.0f) ? 0.0f : (s > 1.0f ? 1.0f : s);    // s in [0, 1]
    v = (v < 0.0f) ? 0.0f : (v > 1.0f ? 1.0f : v);    // v in [0, 1]

	int i = static_cast<int>(h / 60.0f);
	float f = (h / 60.0f) - i;
	float p = v * (1 - s);
	float q = v * (1 - f * s);
	float t = v * (1 - (1 - f) * s);

	switch (i) {
	case 0: r = v; g = t; b = p; break;
	case 1: r = q; g = v; b = p; break;
	case 2: r = p; g = v; b = t; break;
	case 3: r = p; g = q; b = v; break;
	case 4: r = t; g = p; b = v; break;
	case 5: r = v; g = p; b = q; break;
	}

	return sf::Color(static_cast<int>(r * 255), static_cast<int>(g * 255), static_cast<int>(b * 255));
}



sf::VertexArray Mandelbrot::calculate(const sf::Rect<float>& area, unsigned char iterations) const {
	// gr��e des bereichs in pixeln
	float width = area.width;
	float height = area.height;

	sf::VertexArray pixels(sf::Points, width * height);

	for (unsigned int py = 0; py < height; ++py) {
		for (unsigned int px = 0; px < width; ++px) {
			// von pixelkoordinaten zu komplex-koordinaten transformieren
			double real = _c0.real() + (px / width) * (_c1.real() - _c0.real());
			double imag = _c0.imag() + (py / height) * (_c1.imag() - _c0.imag());
			Complex c(real, imag);

			// anzahl iterationen bestimmen
			unsigned char count = iterate(c, iterations);

			// auf log skala skalieren
			float countScaled = std::log(count) / std::log(iterations);
			unsigned int iterationsScaled = countScaled * 360;

			sf::Color colour = hsvToColor(iterationsScaled, 1.0f, 1.0f);

			// Setze den Pixel
			pixels[py * width + px] = sf::Vertex(sf::Vector2f(px, py), colour);
		}
	}

	return pixels;
}

void Mandelbrot::calculatePartial(	sf::VertexArray& pixels, 
									const sf::Rect<float>& area, 
									unsigned int startY, 
									unsigned int endY, 
									unsigned char iterations) const {
	for (unsigned int py = startY; py < endY; ++py) {
		for (unsigned int px = 0; px < area.width; ++px) {
			// von pixelkoordinaten zu komplex-koordinaten transformieren
			double real = _c0.real() + (px / area.width) * (_c1.real() - _c0.real());
			double imag = _c0.imag() + (py / area.height) * (_c1.imag() - _c0.imag());
			Complex c(real, imag);

			// anzahl iterationen bestimmen
			unsigned char count = iterate(c, iterations);

			// log skala
			float countLogScaled = std::log(count) / std::log(iterations);

			float iterationsScaled = countLogScaled * 360.0f;

            // full RGB
			//sf::Color colour = hsvToColor(iterationsScaled, 1.0f, 1.0f);

            // black and white
            //sf::Color colour = hsvToColor(iterationsScaled, 0.0f, countLogScaled);

			// blau monochrom
			sf::Color colour = hsvToColor(	240.0f, 
											1-countLogScaled, 
											(count == iterations) ? 0.0f : countLogScaled);

			// pixel setzen
			pixels[py * area.width + px] = sf::Vertex(sf::Vector2f(px, py), colour);
		}
	}
}

unsigned char Mandelbrot::iterate(const Complex& c, unsigned char iterations) const {
	// startwert Z0 = 0
	Complex z = 0;
	unsigned char count = 0;

	while ((std::abs(z) <= 2.0f) && (count < iterations)) {
		z = z * z + c;
		count++;
	}

	return count;
}