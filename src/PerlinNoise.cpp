#include "PerlinNoise.h"

namespace Noise {
	PerlinNoise::PerlinNoise(unsigned int Seed)
        : m_Seed(Seed)
	{
        InitPermutation();
	}



    // TABLE DE PERMUTATION
    // Cette table sert à générer un "pseudo-random" déterministe basé sur la position
    // → même entrée → même sortie → bruit cohérent dans l’espace
    void PerlinNoise::InitPermutation() {
        // On crée une table de 256 valeurs
        m_Permutation.resize(256);

        // On remplit avec 0,1,2,...,255
        // → base avant mélange
        std::iota(m_Permutation.begin(), m_Permutation.end(), 0);

        // On mélange aléatoirement
        // → crée une permutation pseudo-aléatoire
        std::mt19937 gen(m_Seed);
        std::shuffle(m_Permutation.begin(), m_Permutation.end(), gen);

        // On duplique la table
        // → permet d’éviter les modulo lors des accès (optimisation classique)
        m_Permutation.insert(m_Permutation.end(), m_Permutation.begin(), m_Permutation.end());
    }



    // FONCTION FADE
    // Polynôme de degré 5 : 6t^5 - 15t^4 + 10t^3
    // → garantit continuité des dérivées jusqu’à l’ordre 2 (C²)
    // → évite les cassures entre les cellules
    float PerlinNoise::Fade(float t) const {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }



    // INTERPOLATION LINEAIRE (LERP)
    // Interpolation entre a et b
    // t = 0 → a
    // t = 1 → b
    float PerlinNoise::Lerp(float t, float a, float b) const {
        return a + t * (b - a);
    }



    // GRADIENT
    // Cette fonction génère un vecteur gradient pseudo-aléatoire
    // et calcule le produit scalaire avec le vecteur distance
    float PerlinNoise::Grad(int hash, float x, float y) const {

        // On limite à 8 directions possibles (3 bits)
        int h = hash & 7;

        // Selon h, on choisit comment combiner x et y
        // → permet d’éviter cos/sin (plus rapide)
        float u = h < 4 ? x : y;
        float v = h < 4 ? y : x;

        // On change les signes selon les bits
        // → simule des directions différentes
        return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f * v : 2.0f * v);
    }



    // PERLIN NOISE 2D
    float PerlinNoise::Perlin(float x, float y) const {

        // ===============================
        // 1. IDENTIFIER LA CELLULE
        // ===============================

        // Coordonnées entières de la cellule
        int X = (int)std::floor(x) & 255;
        int Y = (int)std::floor(y) & 255;

        // ===============================
        // 2. POSITION LOCALE DANS LA CELLULE
        // ===============================

        // On garde uniquement la partie fractionnaire
        // → position dans [0,1]
        x -= std::floor(x);
        y -= std::floor(y);

        // ===============================
        // 3. POIDS D’INTERPOLATION (FADE)
        // ===============================

        // On applique la fonction de lissage
        float u = Fade(x);
        float v = Fade(y);

        // ===============================
        // 4. HASH DES COINS
        // ===============================

        // On récupère des indices pseudo-aléatoires pour chaque coin
        int A = m_Permutation[X] + Y;
        int B = m_Permutation[X + 1] + Y;

        // ===============================
        // 5. CALCUL DES CONTRIBUTIONS
        // ===============================

        // Chaque appel à grad = g_ij ⋅ d_ij
        // → produit scalaire entre gradient et vecteur distance

        float bottomLeft = Grad(m_Permutation[A], x, y);
        float bottomRight = Grad(m_Permutation[B], x - 1, y);
        float topLeft = Grad(m_Permutation[A + 1], x, y - 1);
        float topRight = Grad(m_Permutation[B + 1], x - 1, y - 1);

        // ===============================
        // 6. INTERPOLATION
        // ===============================

        // Étape 1 : interpolation horizontale (x)
        float interpBottom = Lerp(u, bottomLeft, bottomRight);
        float interpTop = Lerp(u, topLeft, topRight);

        // Étape 2 : interpolation verticale (y)
        float result = Lerp(v, interpBottom, interpTop);

        // ===============================
        // 7. NORMALISATION
        // ===============================

        // Résultat initial ∈ [-1,1]
        // → on le remap en [0,1] comme Unity
        return (result + 1.0f) / 2.0f;
    }
}