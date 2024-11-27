#include <ccomplex>
#include <thread>
#include "SFML/Graphics.hpp"
#include "mandelbrot.h"

const bool multithreading = true;
float zoomFactor = 1.5f;

/* --Vereinfachte Schreibweise. */
using Complex = std::complex<double>;

Complex c0 = { -2.0, 1.0 };
Complex c1 = { 1.0, -1.0 };


int main() {
	// fenstergr��e auf maximale aufl�sung des bildschirms setzen
	unsigned int window_width = sf::VideoMode::getDesktopMode().width;
	unsigned int window_height = sf::VideoMode::getDesktopMode().height;

	sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Mandelbrot", sf::Style::Fullscreen);

    Mandelbrot MB = Mandelbrot(c0, c1);

    sf::Rect<float> area(0, 0, static_cast<float>(window_width), static_cast<float>(window_height));

    sf::VertexArray VA(sf::Points, window_width * window_height);

    //VA = MB.calculate(area);

    // multithreading stuff
    // viel aus foren und mit hilfe von chatGPT geschrieben
    unsigned int numThreads;
    if (multithreading) {
        // maximale anzahl möglicher threads vom system abfragen
        numThreads = std::thread::hardware_concurrency();
    }
    else {
        numThreads = 1;
    }
    
    // um zugriffsfehler zu vermeiden muss der bereich auf die threads aufgeteilt werden
    // weil alle threads schreiben auf das selbe VertexArray
    unsigned int rowsPerThread = window_height / numThreads;

    std::vector<std::thread> threads;

    for (unsigned int t = 0; t < numThreads; ++t) {
        // start und endpunkt für neuen thread bestimmen
        unsigned int startY = t * rowsPerThread;
        // letzter thread bekommt als endwert einfach "window_height"
        // wird gebraucht falls "window_height" nicht genau durch die anzahl der threads teilbar ist
        unsigned int endY = (t == numThreads - 1) ? window_height : startY + rowsPerThread;

        threads.emplace_back(&Mandelbrot::calculatePartial, &MB, std::ref(VA), std::ref(area), startY, endY, 255);
    }


    // Haupt-Event-Loop
    while (window.isOpen())
    {
        // SFML even handler logik
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            // Esc soll fenster schlie�en und programm beenden
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
            }

            if (event.type == sf::Event::MouseWheelScrolled) {
                // zoom richtung und maus position bestimmen
                float zoomFactorDirectional = (event.mouseWheelScroll.delta > 0)
                    ? 1.0f / zoomFactor
                    : zoomFactor;
                sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(window));

                // umrechnung auf komplexe koordinaten
                Complex zoomCenter = {
                    c0.real() + (c1.real() - c0.real()) * (mousePos.x / window_width),
                    c0.imag() + (c1.imag() - c0.imag()) * (mousePos.y / window_height)
                };

                double newWidth = (c1.real() - c0.real()) * zoomFactorDirectional;
                double newHeight = (c1.imag() - c0.imag()) * zoomFactorDirectional;

                // Setze neuen Bereich
                c0 = Complex(zoomCenter.real() - newWidth / 2.0, zoomCenter.imag() - newHeight / 2.0);
                c1 = Complex(zoomCenter.real() + newWidth / 2.0, zoomCenter.imag() + newHeight / 2.0);

                MB.set(c0, c1);

                // threads neu starten
                for (auto& thread : threads) {
                    if (thread.joinable()) {
                        thread.join();  // Warten, bis der Thread beendet ist
                    }
                }
                threads.clear();  // Jetzt ist es sicher, die Liste zu leeren

                for (unsigned int t = 0; t < numThreads; ++t) {
                    // start und endpunkt für neuen thread bestimmen
                    unsigned int startY = t * rowsPerThread;
                    // letzter thread bekommt als endwert einfach "window_height"
                    // wird gebraucht falls "window_height" nicht genau durch die anzahl der threads teilbar ist
                    unsigned int endY = (t == numThreads - 1) ? window_height : startY + rowsPerThread;

                    threads.emplace_back(&Mandelbrot::calculatePartial, &MB, std::ref(VA), std::ref(area), startY, endY, 255);
                }
            }
        }


        window.clear();
        window.draw(VA);
        window.display();
    }

    for (auto& thread : threads) {
        thread.join();  // Hier wartet das Programm, bis alle Threads beendet sind
    }

	return 0;
}