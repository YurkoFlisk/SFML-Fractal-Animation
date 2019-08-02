#include <complex>
#include <SFML/Graphics.hpp>

using Complex = std::complex<float>;
constexpr int WIDTH = 1001, HEIGHT = 1001, MAX_CHECK_ITERS = 100;
constexpr float X_RANGE = 2.0f, Y_RANGE = 2.0f, // -RANGE to +RANGE
	ANIMATION_TIME = 20.0; // seconds
int mandelbrotIters[HEIGHT][WIDTH]; // for precalculation of iterations up to MAX_CHECK_ITERS

// Returns number of iteration on which first time abs(z) was > 2
// If the point is not in the Mandelbrot set, returns MAX_CHECK_ITERS
inline int calcMandelbrotIters(Complex point)
{
	Complex z = Complex(0, 0);
	for (int i = 0; i < MAX_CHECK_ITERS; ++i)
	{
		z = z * z + point;
		if (std::abs(z) > 2)
			return i;
	}
	return MAX_CHECK_ITERS;
}

int main()
{
	// Precalculation with max check iterations
	for (int i = 0; i <= HEIGHT / 2; ++i)
		for (int j = 0; j < WIDTH; ++j)
		{
			const Complex point(
				(j - WIDTH / 2) * X_RANGE / (WIDTH / 2),
				(i - HEIGHT / 2) * Y_RANGE / (HEIGHT / 2));
			mandelbrotIters[i][j] = mandelbrotIters[HEIGHT - 1 - i][j]
				= calcMandelbrotIters(point);
		}

	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Mandelbrot fractal");
	window.setFramerateLimit(60);

	sf::Sprite sprite;
	sf::Texture texture;
	sf::Image im;
	im.create(WIDTH, HEIGHT);

	sf::Font arialFont;
	arialFont.loadFromFile("arial.ttf");

	sf::Clock clock;
	float totalSecs = 0.0f, elapsedSecs;
	int mandelbrotCheckIters = 1; // Currently modelled check iterations
	
	bool coloringNormMax = true;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::Key::T)
					coloringNormMax = !coloringNormMax;
				break;
			}
		}

		sf::Time elapsed = clock.restart();
		elapsedSecs = elapsed.asSeconds();
		totalSecs += elapsedSecs;
		if (totalSecs > ANIMATION_TIME)
			totalSecs = 0;
		mandelbrotCheckIters = 1 + (MAX_CHECK_ITERS - 1) * totalSecs / ANIMATION_TIME;
		const float coloringNorm = coloringNormMax ? MAX_CHECK_ITERS : mandelbrotCheckIters;

		window.clear(sf::Color::Black);
		for (int i = 0; i <= HEIGHT / 2; ++i)
			for (int j = 0; j < WIDTH; ++j)
				if (const int iters = std::min(mandelbrotIters[i][j],
					mandelbrotCheckIters); iters != 0)
				{
					const int intensity = iters * 255.0f / coloringNorm;
					const sf::Color color(intensity, intensity, intensity);
					im.setPixel(j, i, color);
					im.setPixel(j, HEIGHT - 1 - i, color);
				}
		texture.loadFromImage(im);
		sprite.setTexture(texture, true);
		window.draw(sprite);

		sf::String infoStr("Toggle coloring mode: T\nAnimation time: " + std::to_string(totalSecs));
		sf::Text animInfo(infoStr, arialFont, 16);
		animInfo.setFillColor(sf::Color::Green);
		window.draw(animInfo);

		window.display();
	}

	return 0;
}