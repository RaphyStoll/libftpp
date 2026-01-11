# libftpp — Helpers String / IO / Utils (C++98)

Ces fonctions complètent les primitives `char` de `<cctype>` 
en offrant les versions `std::string`, et ajoutent des helpers 
pratiques absents du standard (sans `<algorithm>`, compatible 42).

---

## 🧵 String Cleaning

| Prototype | Description |
|----------|-------------|
| `std::string trim(const std::string &s);` | Retire espaces / tabulations au début et à la fin. |
| `std::string ltrim(const std::string &s);` | Retire espaces au début uniquement. |
| `std::string rtrim(const std::string &s);` | Retire espaces à la fin uniquement. |

---

## ✂️ String Transformation

| Prototype | Description |
|----------|-------------|
| `std::vector<std::string> split(const std::string &s, char delimiter);` | Découpe une chaîne selon un séparateur. |
| `bool startsWith(const std::string &s, const std::string &prefix);` | Vérifie si `s` commence par `prefix`. |
| `bool endsWith(const std::string &s, const std::string &suffix);` | Vérifie si `s` se termine par `suffix`. |

---

## 🔍 String Validation (version `string` de `<cctype>`)

| Prototype | Description |
|----------|-------------|
| `bool isDigit(const std::string &s);` | Retourne vrai si `s` contient uniquement `0–9`. |
| `bool isAlpha(const std::string &s);` | Retourne vrai si `s` contient uniquement des lettres A-Z / a-z. |
| `bool isAlnum(const std::string &s);` | Retourne vrai si `s` contient uniquement lettres et/ou chiffres. |
| `bool isSpace(const std::string &s);` | Retourne vrai si `s` ne contient que des espaces / tab / newline. |
| `bool isNumber(const std::string &s);` | Version stricte `isDigit(s)` (utile pour parse d'arguments ints). |

> **Note :** Ces fonctions **n'existent pas en version string** dans le standard → elles sont légitimes dans libftpp.

---

## 🧮 Math Helpers

| Prototype | Description |
|----------|-------------|
| `float clamp(float value, float min, float max);` | Contraint `value` dans `[min, max]`. *(C++17 only → donc version maison ici)* |

---

## 📂 File Helpers

| Prototype | Description |
|----------|-------------|
| `bool fileExists(const std::string &path);` | Test via `std::ifstream` si un fichier est accessible. |
| `std::string readFile(const std::string &path);` | Retourne le contenu complet du fichier en string. |

---

## ✅ Résumé

Ces fonctions sont :
- Autorisées à 42 (pas besoin de `<algorithm>`, `<regex>`, etc.)
- Pas présentes dans le standard C++98
- Réutilisables dans **PhoneBook, Webserv, Cub3D, miniRT, Config Parser**, etc.

Cette lib est donc **réellement utile** → tu es dans la bonne direction 👍

---