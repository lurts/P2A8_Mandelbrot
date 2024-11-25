#include <ccomplex>
#include <thread>
#include "SFML/Graphics.hpp"
#include "mandelbrot.h"

const bool multithreading = true;

/* --Vereinfachte Schreibweise. */
using Complex = std::complex<double>;

Complex c0 = { -2.0, 1.0 };
Complex c1 = { 1.0, -1.0 };


int main() {
	// fenstergröße auf maximale auflösung des bildschirms setzen
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
        numThreads = std::thread::hardware_concurrency();
    }
    else {
        numThreads = 1;
    }
    
    unsigned int rowsPerThread = window_height / numThreads;
    std::vector<std::thread> threads;

    for (unsigned int t = 0; t < numThreads; ++t) {
        unsigned int startY = t * rowsPerThread;
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

            // Esc soll fenster schließen und programm beenden
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
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