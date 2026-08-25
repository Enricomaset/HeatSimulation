#include <GLFW/glfw3.h>

#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>

constexpr double COLD_TEMPERATURE = 20.0;
constexpr double HOT_TEMPERATURE = 100.0;

// Inizializza la piastra
void initialize_plate(
    std::vector<double>& current,
    std::vector<double>& next,
    int N
) {
    std::fill(
        current.begin(),
        current.end(),
        COLD_TEMPERATURE
    );

    // Quadrato caldo centrale
    const int start = 2 * N / 5;
    const int end = 3 * N / 5;

    for (int i = start; i < end; ++i) {
        for (int j = start; j < end; ++j) {
            current[i * N + j] = HOT_TEMPERATURE;
        }
    }

    next = current;
}

// Esegue un passo temporale
void step(
    std::vector<double>& current,
    std::vector<double>& next,
    int N,
    double r
) {
    for (int i = 1; i < N - 1; ++i) {
        for (int j = 1; j < N - 1; ++j) {

            const int center = i * N + j;

            next[center] =
                (1.0 - 4.0 * r) * current[center]
                + r * current[(i + 1) * N + j]
                + r * current[(i - 1) * N + j]
                + r * current[i * N + (j + 1)]
                + r * current[i * N + (j - 1)];
        }
    }

    std::swap(current, next);

    // Mantiene la sorgente centrale a 500 °C
    const int start = 2 * N / 5;
    const int end = 3 * N / 5;

    for (int i = start; i < end; ++i) {
        for (int j = start; j < end; ++j) {
            current[i * N + j] = HOT_TEMPERATURE;
        }
    }
}

// Converte la temperatura in un colore
void temperature_to_color(
    double temperature,
    float& red,
    float& green,
    float& blue
) {
    // Converte [COLD_TEMPERATURE, HOT_TEMPERATURE] nell'intervallo [0, 1]
    double value =
        (temperature - COLD_TEMPERATURE)
        / (HOT_TEMPERATURE - COLD_TEMPERATURE);

    value = std::clamp(value, 0.0, 1.0);

    // Gradiente blu -> giallo -> rosso
    if (value < 0.5) {
        red = static_cast<float>(2.0 * value);
        green = static_cast<float>(2.0 * value);
        blue = static_cast<float>(1.0 - 2.0 * value);
    } else {
        red = 1.0f;
        green =
            static_cast<float>(2.0 * (1.0 - value));
        blue = 0.0f;
    }
}

// Disegna la piastra
void draw_plate(
    const std::vector<double>& current,
    int N
) {
    const float cell_size =
        2.0f / static_cast<float>(N);

    glBegin(GL_QUADS);

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {

            const double temperature =
                current[i * N + j];

            float red;
            float green;
            float blue;

            temperature_to_color(
                temperature,
                red,
                green,
                blue
            );

            glColor3f(red, green, blue);

            const float left =
                -1.0f + j * cell_size;

            const float right =
                left + cell_size;

            const float bottom =
                -1.0f + i * cell_size;

            const float top =
                bottom + cell_size;

            glVertex2f(left, bottom);
            glVertex2f(right, bottom);
            glVertex2f(right, top);
            glVertex2f(left, top);
        }
    }

    glEnd();
}

int main() {

    const int N = 100;
    const double r = 0.2;

    if (r <= 0.0 || r > 0.25) {
        std::cerr
            << "Errore: r deve essere compreso "
            << "tra 0 e 0.25\n";

        return 1;
    }

    std::vector<double> current(N * N);
    std::vector<double> next(N * N);

    initialize_plate(current, next, N);

    if (!glfwInit()) {
        std::cerr
            << "Errore durante l'inizializzazione "
            << "di GLFW\n";

        return 1;
    }

    GLFWwindow* window = glfwCreateWindow(
        800,
        800,
        "Heat diffusion",
        nullptr,
        nullptr
    );

    if (window == nullptr) {
        std::cerr
            << "Impossibile creare la finestra\n";

        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    while (!glfwWindowShouldClose(window)) {

        if (
            glfwGetKey(window, GLFW_KEY_ESCAPE)
            == GLFW_PRESS
        ) {
            glfwSetWindowShouldClose(
                window,
                GLFW_TRUE
            );
        }

        step(current, next, N, r);

        int width;
        int height;

        glfwGetFramebufferSize(
            window,
            &width,
            &height
        );

        glViewport(0, 0, width, height);

        glClearColor(
            0.05f,
            0.05f,
            0.05f,
            1.0f
        );

        glClear(GL_COLOR_BUFFER_BIT);

        draw_plate(current, N);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
